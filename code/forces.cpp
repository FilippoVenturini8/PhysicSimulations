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

    Vec3 springForce = (ks * (posDiff.norm() - 1.0) + kd * (p1->vel - p0->vel).dot((posDiff/posDiff.norm()))) * (posDiff/posDiff.norm());

    p0->force += springForce;
    p1->force += -springForce;
}
