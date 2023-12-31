#include "colliders.h"
#include <cmath>
#include <iostream>
#include <limits>  // For FLT_MAX

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
    p->prevPos = p->pos;

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
    Vec3 planeN = (this->center - p->pos) / (p->pos - this->center).norm();

    Vec3 sphereToParticle = p->pos - this->center;

    // Normalize the direction vector
    Vec3 normalizedDirection = sphereToParticle.normalized();

    // Compute the collision point on the sphere's surface
    Vec3 collisionPoint = this->center + this->radius * normalizedDirection;

    double planeD = -(planeN.dot(collisionPoint));
    ColliderPlane tangentPlane = ColliderPlane(planeN, planeD);
    tangentPlane.resolveCollision(p, kElastic, kFriction);
    if(this->testCollision(p)){
        p->prevPos = p->pos;
        p->pos += 0.2 * p->vel;
    }

}

/*
 * Cube
 */

bool ColliderAABB::isRayIntersecting(const Particle* p, Vec3 &intersectionPoint)const
{
    Vec3 dir = (p->pos - p->prevPos).normalized();
    Vec3 pos = p->pos;
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    double EPSILON = 0.000000001;

    if(dir.norm() == 0){
        return false;
    }

    for (int i = 0; i < 3; i++) {
        double a = this->position[i] - (this->dimension[i] / 2);
        double b = this->position[i] + (this->dimension[i] / 2);
        if (abs(dir[i]) < EPSILON) {
            if (pos[i] < a || pos[i] > b) return false;
        } else {
            float t1 = (a - pos[i])/dir[i];
            float t2 = (b - pos[i])/dir[i];

            if (t1 > t2) std::swap(t1, t2);
            if (t1 > tmin) tmin = t1;
            if (t2 < tmax) tmax = t2;
            if (tmin > tmax) return false;
        }
    }

    intersectionPoint = pos + dir * tmin;
    if(p->radius <= tmin){
        return false;
    }else{
        return true;
    }
}


bool ColliderAABB::testCollision(const Particle* p)const
{
    Vec3 intersectionPoint = Vec3(0.0f,0.0f,0.0f);

    bool intersection = this->isRayIntersecting(p, intersectionPoint);

    return intersection;
}

void ColliderAABB::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    Vec3 intersectionPoint = Vec3(0.0f, 0.0f, 0.0f);
    this->isRayIntersecting(p, intersectionPoint);
    p->prevPos = p->pos;

    for(int i = 0; i < 3; i++){
        double a = position[i] - (this->dimension[i] / 2);
        double b = position[i] + (this->dimension[i] / 2);
        ColliderPlane collisionSide = ColliderPlane();
        Vec3 planeN = Vec3(0,0,0);
        planeN[i] = 1;
        planeN.normalize();

        if(i == 2 || i == 0){
            std::swap(a, b);
        }

        if(abs(intersectionPoint[i]-a) < 0.0001){
            collisionSide.setPlane(planeN, a);
            collisionSide.resolveCollision(p, kElastic, kFriction);
            return;
        }else if (abs(intersectionPoint[i]-b) < 0.0001){
            collisionSide.setPlane(-planeN, b);
            collisionSide.resolveCollision(p, kElastic, kFriction);
            return;
        }
    }
}




