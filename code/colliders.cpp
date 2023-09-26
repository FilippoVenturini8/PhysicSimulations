#include "colliders.h"
#include <cmath>
#include <iostream>


/*
 * Plane
 */

bool ColliderPlane::testCollision(const Particle* p) const
{
    return this->planeN.dot(p->pos) + this->planeD * this->planeN.dot(p->prevPos) + this->planeD <= 0;
}

void ColliderPlane::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    Vec3 collisionPos = p->pos;

    p->pos = collisionPos - (1 + kElastic) * (this->planeN.dot(collisionPos) + this->planeD) * this->planeN;

    Vec3 collisionVel = p->vel;

    p->vel = collisionVel - (1 + kElastic) * (this->planeN.dot(collisionVel)) * this->planeN;

    Vec3 normalVelocity = (this->planeN.dot(p->vel)) * this->planeN;

    Vec3 tangentVelocity = p->vel - normalVelocity;

    p->vel = p->vel - kFriction * tangentVelocity;
}


/*
 * Sphere
 */

bool ColliderSphere::testCollision(const Particle* p)const
{
    ColliderPlane tangentPlane = ColliderPlane(Vec3(p->pos.x() - this->center.x(), p->pos.y() - this->center.y(), p->pos.z() - this->center.z()), 0);
    if(tangentPlane.testCollision(p)){
        std::cout << "tangent planeN: " << tangentPlane.planeN << std::endl;
        std::cout << "collision pos: " << p->pos << std::endl;
        std::cout << "collision center: " << this->center << std::endl;
        return true;
    }
    return false;
}

void ColliderSphere::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    ColliderPlane tangentPlane = ColliderPlane(Vec3(p->pos.x() - this->center.x(), p->pos.y() - this->center.y(), p->pos.z() - this->center.z()), 0);
    tangentPlane.resolveCollision(p, kElastic, kFriction);
}
