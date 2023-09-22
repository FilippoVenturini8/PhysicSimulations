#include "colliders.h"
#include <cmath>


/*
 * Plane
 */

bool ColliderPlane::testCollision(const Particle* p) const
{
    return this->planeN.dot(p->pos) + this->planeD * this->planeN.dot(p->prevPos) + this->planeD <= 0;
}

void ColliderPlane::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    double positionScalar = this->planeN.dot(p->pos) + this->planeD;

    Vec3 collisionPos = p->pos;

    p->pos = collisionPos - 2 * positionScalar * this->planeN;

    Vec3 collisionVel = p->vel;

    p->vel =  collisionVel - 2 * (this->planeN.dot(collisionVel)) * (this->planeN);
}
