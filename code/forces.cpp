#include "forces.h"

void ForceConstAcceleration::apply() {
    for (Particle* p : particles) {
        p->force = this->getAcceleration();
    }
}
