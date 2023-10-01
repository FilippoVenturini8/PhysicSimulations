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
