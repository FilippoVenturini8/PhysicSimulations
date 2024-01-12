#include "colliders.h"
#include <cmath>
#include <iostream>
#include <limits>  // For FLT_MAX

/*
 * Plane
 */

bool ColliderPlane::testCollision(const Particle* p) const
{
    if((planeN.dot(p->pos) + planeD) * (planeN.dot(p->prevPos) + planeD) <= 0){
        return true;
    }
    return false;
}

void ColliderPlane::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    double scalar1 = planeN.dot(p->pos) + planeD;
    Vec3 newpos = p->pos - (1 + kElastic) * scalar1 * planeN;
    Vec3 newvel = p->vel - (1 + kElastic) * (planeN.dot(p->vel)) * planeN;

    Vec3 velN = (planeN.dot(p->vel)) * planeN;
    Vec3 velT = p->vel - velN;

    Vec3 finalvel = newvel - kFriction * velT;

    p->vel = finalvel;
    p->pos = newpos;
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

bool ColliderAABB::collisionDetection(const Particle*p, bool &intersection, Vec3 &interPoint, float &tmin) const{
    Vec3 vel = p->vel;
    Vec3 initial = p->pos;
    tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    double EPSILON = 0.01;

    for (int i = 0; i < 3; ++i) {
        float a = position[i] - dimension[i]/2;
        float b = position[i] + dimension[i]/2;
        if(abs(vel[i]) < EPSILON){
            if(initial[i] < a || initial[i] > b){
                intersection = false;
                return intersection;
            }
        }
        else{
            vel = p->vel.normalized();

            float t1 = (a - initial[i]) /vel[i];
            float t2 = (b - initial[i])/ vel[i];

            if(t1 > t2){
                std::swap(t1,t2);
            }
            if(t1 > tmin){
                tmin = t1;
            }
            if(t2 < tmax){
                tmax = t2;
            }
            if(tmin > tmax){
                intersection = false;
                return intersection;
            }
        }
    }

    if(p->radius < tmin){
        intersection = false;
        return intersection;
    }

    interPoint = initial + vel*tmin;
    intersection = true;
    return intersection;
}


bool ColliderAABB::testCollision(const Particle* p)const
{
    Vec3 intersectionPoint = Vec3(0.0f,0.0f,0.0f);
    bool inter = false;
    float tmin;

    return this->collisionDetection(p, inter, intersectionPoint, tmin);
}

void ColliderAABB::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    p->prevPos = p->pos;
    bool inter;
    Vec3 interPoint = Vec3(0.0f,0.0f,0.0f);
    float tmin;
    collisionDetection(p,inter, interPoint, tmin);

    for(int i = 0; i < 3; i++){
        float a = position[i] - dimension[i]/2.0f;
        float b = position[i] + dimension[i]/2.0f;
        Vec3 n = Vec3(0,0,0);
        n[i] = 1;

        if(abs(interPoint[i] - a) < 1.0f){
            ColliderPlane plane = ColliderPlane(n, -a);
            plane.resolveCollision(p, kElastic, kFriction);
        }
        if(abs(interPoint[i] - b) < 1.0f){
            ColliderPlane plane2 = ColliderPlane(n, -b);
            plane2.resolveCollision(p, kElastic, kFriction);
        }
    }
}

void particleCollisions(Particle* pi){
    for (Particle *pj : pi->neighbors) {
        double minDist = 2.0 * pi->radius;

        Vec3 tempNormal = pi->pos - pj->pos;
        double d2 = tempNormal.dot(tempNormal);

        if (d2 > 0.0 && d2 < minDist * minDist) {
            double d = std::sqrt(d2);
            tempNormal *= 1.0 / d;
            double corr = (minDist - d) * 0.5;

            pi->pos += tempNormal*corr;
            pj->pos += tempNormal*-corr;

            double vi = pi->vel.dot(tempNormal);
            double vj = pj->vel.dot(tempNormal);

            pi->vel += tempNormal*(vj - vi);
            pj->vel += tempNormal*(vi - vj);
        }
    }
}




