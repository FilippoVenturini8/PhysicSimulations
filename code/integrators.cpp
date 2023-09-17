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
    // TODO
}
