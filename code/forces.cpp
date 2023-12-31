#include "forces.h"
#include <cmath>
#include <float.h>
#include <iostream>

void ForceConstAcceleration::apply() {
    for (Particle* p : particles) {
        p->force += this->getAcceleration();
    }
}

void ForceAirDrag::apply(){
    for (Particle* p : particles){
        p->force += (-this->k) * p->vel;
    }
}

void ForceGravitationalAttraction::apply(){
    for (Particle* p : particles){
        double distance = sqrt(pow(blackHolePos.x() - p->pos.x(), 2) + pow(blackHolePos.y() - p->pos.y(), 2) + pow(blackHolePos.z() - p->pos.z(), 2));
        p->force += ((G * blackHoleMass * p->mass) / pow(fmax(distance, 0.01), 3)) * (blackHolePos - p->pos);
    }
}

void ForceSpring::apply(){
    Particle* p0 = particles[0];
    Particle* p1 = particles[1];

    Vec3 posDiff = p1->pos - p0->pos;

    Vec3 springForce = (ks * (posDiff.norm() - this->l) + kd * (p1->vel - p0->vel).dot((posDiff/posDiff.norm()))) * (posDiff/posDiff.norm());

    p0->force += springForce;
    p1->force += -springForce;
}

double smoothingKernelPoly6(double r, double h){ // r = distance from current particle to neighbor
    if(r >= 0 && h >= r){
        return 315 / (64 * 3.141f * pow(h,9)) * pow(h*h - r*r, 3);
    }
    return 0;
}

double smoothingKernelSpiky(double r, double h){ // r = distance from current particle to neighbor
    if(r >= 0 && h >= r){
        return 15 / (3.141f * pow(h,6)) * pow(h - r, 3);
    }
    return 0;
}

Vec3* gradientSmoothingKernelSpiky(Vec3* r, double h){
    Vec3* gradient = new Vec3();

    gradient->x()= - 45 / (3.141f * pow(h,6)) * r->x() * pow(h - r->x(), 2);
    gradient->y()= - 45 / (3.141f * pow(h,6)) * r->y() * pow(h - r->y(), 2);
    gradient->z()= - 45 / (3.141f * pow(h,6)) * r->z() * pow(h - r->z(), 2);
    return gradient;
}

Vec3* laplacianSmoothingKernelViscosity(Vec3* r, double h){
    Vec3* gradient = new Vec3();

    gradient->x()= 45 / (3.141f * pow(h,5)) * (1 - (r->x() / h));
    gradient->y()= 45 / (3.141f * pow(h,5)) * (1 - (r->y() / h));
    gradient->z()= 45 / (3.141f * pow(h,5)) * (1 - (r->z() / h));
    return gradient;
}

double ForceNavierStockes::densityCalculation(Particle *p){
    double density = 0;

    for(int i = 0; i < p->neighbors->size(); i++){
        Particle* neighbor = p->neighbors->at(i);

        double substraction = (neighbor->pos - p->pos).norm();
        density += neighbor->mass * smoothingKernelPoly6(substraction, this->h);
    }

    density += p->mass * smoothingKernelPoly6(0, this->h);
    return density;
}

double ForceNavierStockes::pressureCalculation(double density){
    //return 35000 * (pow(density/this->waterDensity, 7) - 1);
    return ((density/0.2) - 1);
}

void ForceNavierStockes::pressureAccelerationCalculation(){
    for (Particle* pi : particles){
        Vec3* pressure = new Vec3();

        double densityPi = densityCalculation(pi);
        //std::cout << "densityPi: " <<densityPi << std::endl;
        double pressurePi = this->pressureCalculation(densityPi)/(densityPi * densityPi);
        //std::cout << "pressurePi: " <<pressurePi << std::endl;
        //std::cout <<"neighbors: " << pi->neighbors->size() << std::endl;

        for (Particle* pj : *pi->neighbors){

            double densityPj = densityCalculation(pj);
            double pressurePj = this->pressureCalculation(densityPj)/(densityPj * densityPj);

            double pressureTerm = -pj->mass * (pressurePi + pressurePj);

            Vec3* distance = new Vec3(pi->pos.x() - pj->pos.x(),
                                      pi->pos.y() - pj->pos.y(),
                                      pi->pos.z() - pj->pos.z());

            Vec3* gradient = gradientSmoothingKernelSpiky(distance, 2 * h); //kernel radius = 4*particleRadius

            //std::cout << "Gradient: " << gradient << std::endl;

            pressure->x() += gradient->x() * pressureTerm;
            pressure->y() += gradient->y() * pressureTerm;
            pressure->z() += gradient->z() * pressureTerm;
        }

        //std::cout << "PressureX: " << pressure->x() * pi->mass << std::endl;
        //std::cout << "PressureY: " << pressure->y() * pi->mass << std::endl;
        //std::cout << "PressureZ: " << pressure->z() * pi->mass << std::endl;

        pi->force.x() += pressure->x() * pi->mass;
        pi->force.y() += pressure->y() * pi->mass;
        pi->force.z() += pressure->z() * pi->mass;
    }
}

void ForceNavierStockes::viscosityAccelerationCalculation(){
    for (Particle* pi : particles){
        Vec3* viscosity = new Vec3();
        double densityPi = this->densityCalculation(pi);

        for (Particle* pj : *pi->neighbors){
            double densityPj = this->densityCalculation(pj);

            Vec3 vij = this->viscosity * pj->mass * (pj->vel - pi->vel)/(densityPi * densityPj);

            Vec3* distance = new Vec3(pi->pos.x() - pj->pos.x(),
                                      pi->pos.y() - pj->pos.y(),
                                      pi->pos.z() - pj->pos.z());

            Vec3* laplacian = laplacianSmoothingKernelViscosity(distance, h);

            viscosity->x() += laplacian->x() * vij.x();
            viscosity->y() += laplacian->y() * vij.y();
            viscosity->z() += laplacian->z() * vij.z();
        }

        std::cout << "ViscosityX: " << viscosity->x() * pi->mass << std::endl;
        std::cout << "ViscosityY: " << viscosity->y() * pi->mass << std::endl;
        std::cout << "ViscosityZ: " << viscosity->z() * pi->mass << std::endl;

        pi->force.x() += viscosity->x() * pi->mass;
        pi->force.y() += viscosity->y() * pi->mass;
        pi->force.z() += viscosity->z() * pi->mass;
    }
}

void ForceNavierStockes::apply(){
    this->pressureAccelerationCalculation();
    //this->viscosityAccelerationCalculation();
}



