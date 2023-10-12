#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "defines.h"
#include "particle.h"


class Collider  // Abstract interface
{
public:
    Collider() {}
    virtual ~Collider() {}

    virtual bool testCollision(const Particle* p) const = 0;
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const = 0;
};


class ColliderPlane : public Collider
{
public:
    ColliderPlane() { planeN = Vec3(0,0,0); planeD = 0; }
    ColliderPlane(const Vec3& n, double d) : planeN(n), planeD(d) {}
    virtual ~ColliderPlane() {}

    void setPlane(const Vec3& n, double d) { this->planeN = n; this->planeD = d; }

    virtual bool testCollision(const Particle* p) const;
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const;

protected:
    Vec3 planeN;
    double planeD;
};

class ColliderSphere : public Collider
{
public:
    ColliderSphere() { center = Vec3(0,0,0); radius = 0; }
    ColliderSphere(const Vec3& center, double radius) : center(center), radius(radius) {}
    virtual ~ColliderSphere() {}

    void updateCenter(const Vec3& newCenter) { center = newCenter; }
    void setRadius(const double newRadius){ radius = newRadius; }
    Vec3 getCenter(){ return center; }
    double getRadius(){ return radius; }

    virtual bool testCollision(const Particle* p) const;
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const;

protected:
    Vec3 center;
    double radius;
};

class ColliderAABB : public Collider
{
public:
    ColliderAABB() { position = Vec3(0,0,0); dimension = Vec3(0,0,0); }
    ColliderAABB(const Vec3& position, Vec3 dimension) : position(position), dimension(dimension){}
    virtual ~ColliderAABB() {}

    void updatePosition(const Vec3& newPosition) { position = newPosition; }

    virtual bool isRayIntersecting(const Particle* p, Vec3 &intersectionPoint) const;
    virtual bool testCollision(const Particle* p) const;
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const;

protected:
    Vec3 position;
    Vec3 dimension;
};


#endif // COLLIDERS_H
