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
    Vec3 planeN = -(p->pos - this->center)/pow((p->pos - this->center).dot(p->pos - this->center), 0.5);
    double planeD = -(planeN.dot(p->pos));
    ColliderPlane tangentPlane = ColliderPlane(planeN, planeD);
    /*std::cout << "prev pos: "<< p->pos << std::endl;
    std::cout << "planeD: "<< planeD << std::endl;*/
    tangentPlane.resolveCollision(p, kElastic, kFriction);
    if(this->testCollision(p)){
        p->prevPos = p->pos;
        p->pos += 0.2 * p->vel;
    }
    //std::cout << "after pos: "<< p->pos << std::endl;
}

/*
 * Cube
 */

bool ColliderAABB::isRayIntersecting(const Particle* p, Vec3 &intersectionPoint)const
{
    Vec3 dir = p->vel;
    Vec3 pos = p->pos;
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    double EPSILON = 0.000000001;

    for (int i = 0; i < 3; i++) {
        float a = this->position[i] - (this->dimension[i] / 2);
        float b = this->position[i] + (this->dimension[i] / 2);
        if (abs(dir[i]) < EPSILON) {
            if (pos[i] < a || pos[i] > b) return false;
        } else {
            dir =  p->vel.normalized();

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
    Vec3 intersectionPoint = Vec3(0.0f,0.0f,0.0f);
    this->isRayIntersecting(p, intersectionPoint);
    p->prevPos = p->pos;

    for(int i = 0; i < 3; i++){
        float a = position[i] - (this->dimension[i] / 2);
        float b = position[i] + (this->dimension[i] / 2);
        ColliderPlane* collisionSide = new ColliderPlane();
        Vec3 planeN = Vec3(0,0,0);
        planeN[i] = 1;

        if(i == 2 || i == 0){
            std::swap(a, b);
        }

        if(abs(intersectionPoint[i]-a) < 0.0001){
            p->pos[i] = a;
            collisionSide->setPlane(planeN, a);
            collisionSide->resolveCollision(p, kElastic, kFriction);
            return;
        }else if (abs(intersectionPoint[i]-b) < 0.0001){
            p->pos[i] = b;
            collisionSide->setPlane(-planeN, b);
            collisionSide->resolveCollision(p, kElastic, kFriction);
            return;
        }
    }
}

