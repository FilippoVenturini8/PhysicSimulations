#include "integrators.h"


void IntegratorEuler::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd x1 = x0 + dt*dx;
    system.setState(x1);
}


void IntegratorSymplecticEuler::step(ParticleSystem &system, double dt) {
    // TODO
}


void IntegratorMidpoint::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd eulerStep = x0 + dt*dx;
    Vecd fMid = x0 + eulerStep/2;
    Vecd x1 = x0 + fMid;
    system.setState(x1);
}


void IntegratorVerlet::step(ParticleSystem &system, double dt) {
    // TODO
}
