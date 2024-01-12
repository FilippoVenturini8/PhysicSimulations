#include "integrators.h"
#include <iostream>


void IntegratorEuler::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd x1 = x0 + dt*dx;
    system.setState(x1);
}


void IntegratorSymplecticEuler::step(ParticleSystem &system, double dt) {
    for (Particle* p : system.getParticles()) {
        Vec3 v0 = p->vel;
        p->vel = v0 + dt * (p->force/p->mass);
    }
    Vecd p0 = system.getPositions();
    Vecd v1 = system.getVelocities();
    Vecd p1 = p0 + dt * v1;
    system.setPositions(p1);
}

void IntegratorSymplecticEuler::stepWithoutPS(std::vector<Particle*> particles, double dt){
    for(int i = 0; i < particles.size(); i++){
        Vec3 v0 = particles.at(i)->vel;
        Vec3 x0 = particles.at(i)->pos;

        Vec3 v1 = v0 + dt * particles.at(i)->force/particles.at(i)->mass;
        Vec3 x1 = x0 + dt * v1;
        particles.at(i)->vel = v1;
        particles.at(i)->pos = x1;
    }
}


void IntegratorMidpoint::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd eulerStep = x0 + (dt/2)*dx;
    system.setState(eulerStep);
    Vecd dx2 = system.getDerivative();
    Vecd x1 = x0 + dt*dx2;
    system.setState(x1);
}


void IntegratorVerlet::step(ParticleSystem &system, double dt) {
    for(Particle* p : system.getParticles()){
        if(p->pos.x() == p->prevPos.x() && p->pos.y() == p->prevPos.y() && p->pos.z() == p->prevPos.z()){
            p->prevPos = p->pos - p->vel * dt;
        }
        Vec3 p1 = p->pos + kd * (p->pos - p->prevPos) + ((dt*dt)/p->mass) * p->force;
        p->prevPos = p->pos;
        p->pos = p1;
    }
}

void IntegratorRK2::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd k1 = system.getDerivative();

    // Calculate x1_temp without updating forces
    Vecd x1_temp = x0 + 0.5 * dt * k1;

    // Calculate k2 based on x1_temp, without updating forces
    system.setState(x1_temp, false);
    Vecd k2 = system.getDerivative();

    // Update the state using RK2 formula
    Vecd x1 = x0 + dt * k2;

    // Set the final state
    system.setState(x1);
}
