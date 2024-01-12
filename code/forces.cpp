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
        return 315 / (64 * M_PI * pow(h,9)) * pow(h*h - r*r, 3);
    }
    return 0;
}

double ForceNavierStockes::densityCalculation(Particle *p){
    double density = 0;

    for(Particle* neighbor : p->neighbors){
        double substraction = (neighbor->pos - p->pos).norm();
        density += neighbor->mass * smoothingKernelPoly6(substraction, this->h);
    }
    return density;
}

double ForceNavierStockes::pressureCalculation(double density){
    return GAS_CONST * (density - REST_DENS);
}

void ForceNavierStockes::accelerationCalculation(){
    double gradient = particles.at(0)->mass * 45.0f / (M_PI * pow(h, 6.f));
    double laplacian = VISC * particles.at(0)->mass * 40.f / (M_PI * pow(h, 5.f));

    for (Particle* pi : particles){
        Vec3 pressure = Vec3(0,0,0);
        Vec3 visc = Vec3(0,0,0);
        double densityPi = densityCalculation(pi);
        double pressurePi = this->pressureCalculation(densityPi)/(densityPi * densityPi);

        for (Particle* pj : pi->neighbors){
            double densityPj = densityCalculation(pj);
            double pressurePj = this->pressureCalculation(densityPj)/(densityPj * densityPj);

            Vec3 distance = pj->pos - pi->pos;
            pressure += distance.normalized()  * gradient * pow(h - distance.norm(), 2.0f)  * (pressurePi + pressurePj);
            visc += (pj->vel - pi->vel) / densityPj * laplacian * (h - distance.norm());
        }
        Vec3 force = Vec3(0,0,0);
        for(int i = 0; i < 3; i++){
            if(pressure[i] < 5.0f && pressure[i] > -5.0f){
                force[i] += pressure[i];
            }
            if(visc[i] < 5.0f && visc[i] > -5.0f){
                force[i] += visc[i];
            }
        }
        pi->force += force;
    }
}

void ForceNavierStockes::apply(){
    this->accelerationCalculation();
}



