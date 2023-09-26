#include "forces.h"
#include <cmath>
#include <float.h>
#include <iostream>

void ForceConstAcceleration::apply() {
    for (Particle* p : particles) {
        p->force += this->getAcceleration();
    }
}

void ForceGravitationalAttraction::apply(){
    Vec3 blackHolePos = Vec3(50, 50, 0);
    for (Particle* p : particles){
        double distance = sqrt(pow(blackHolePos.x() - p->pos.x(), 2) + pow(blackHolePos.y() - p->pos.y(), 2) + pow(blackHolePos.z() - p->pos.z(), 2));
        //std::cout << "num" << ((G * DBL_MAX * p->mass) / pow(distance, 3)) << std::endl;
        Vec3 direction = Vec3(p->pos.x() - blackHolePos.x(), p->pos.y() - blackHolePos.y(), p->pos.z() - blackHolePos.z());
        //std::cout << "dir" << ((G * DBL_MAX * p->mass) / pow(distance, 3)) * direction  << std::endl;

        //Con la massa infinita non va. Mettendo al coefficiente tipo 5 funziona male
        p->force += ((G * DBL_MAX * p->mass) / pow(distance, 3)) * Vec3(blackHolePos.x() - p->pos.x(), blackHolePos.y() - p->pos.y(), blackHolePos.z() - p->pos.z());
    }
}
