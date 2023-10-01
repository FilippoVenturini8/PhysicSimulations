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
    double cond = (p->pos - this->center).dot((p->pos - this->center).transpose());

    return cond <= pow(radius, 2);
}

void ColliderSphere::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    Vec3 planeN = -(p->pos - this->center)/pow((p->pos - this->center).dot(p->pos - this->center), 0.5);
    double planD = -(planeN.dot(p->pos));
    ColliderPlane tangentPlane = ColliderPlane(planeN, planD);
    tangentPlane.resolveCollision(p, kElastic, kFriction);

    if(this->testCollision(p)){
        p->pos += 0.2 * p->vel;
    }
}

/*
 * Cube
 */


bool ColliderCube::testCollision(const Particle* p)const
{
    double x_max = this->position.x() + this->side/2;
    double x_min = this->position.x() - this->side/2;
    double y_max = this->position.y() + this->side/2;
    double y_min = this->position.y() - this->side/2;
    double z_max = this->position.z() + this->side/2;
    double z_min = this->position.z() - this->side/2;
    return (p->pos.x() <= x_max && p->pos.x() >= x_min) && (p->pos.y() <= y_max && p->pos.y() >= y_min) && (p->pos.z() <= z_max && p->pos.z() >= z_min);
}

void ColliderCube::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    // TODO
}

