#ifndef FORCES_H
#define FORCES_H

#include <vector>
#include "particle.h"

class Force
{
public:
    Force(void) {}
    virtual ~Force(void) {}

    virtual void apply() = 0;

    void addInfluencedParticle(Particle* p) {
        particles.push_back(p);
    }

    void setInfluencedParticles(const std::vector<Particle*>& vparticles) {
        particles = vparticles;
    }

    void clearInfluencedParticles() {
        particles.clear();
    }

    std::vector<Particle*> getInfluencedParticles() const {
        return particles;
    }

protected:
    std::vector<Particle*>	particles;
};


class ForceConstAcceleration : public Force
{
public:
    ForceConstAcceleration() { acceleration = Vec3(0,0,0); }
    ForceConstAcceleration(const Vec3& a) { acceleration = a; }
    virtual ~ForceConstAcceleration() {}

    virtual void apply();

    void setAcceleration(const Vec3& a) { acceleration = a; }
    Vec3 getAcceleration() const { return acceleration; }

protected:
    Vec3 acceleration;
};

class ForceAirDrag : public Force
{
public:
    ForceAirDrag() { k = 0; }
    ForceAirDrag(double k) { this->k = k; }
    virtual ~ForceAirDrag() {}

    virtual void apply();

    void setK(double newK) { k = newK; }
    double getK() const { return k; }

protected:
    double k;
};

class ForceGravitationalAttraction : public Force
{
public:
    ForceGravitationalAttraction() {}
    ForceGravitationalAttraction(Vec3 blackHolePostition) {this->blackHolePos = blackHolePostition;}
    virtual ~ForceGravitationalAttraction() {}

    virtual void apply();

    void enableBlackHole() { blackHoleMass = 2.e15; }
    void disableBlackHole() { blackHoleMass = 0.0; }

protected:
    float G = 6.67 / 1e11;
    Vec3 blackHolePos;
    double blackHoleMass;
};

class ForceSpring : public Force
{
public:
    ForceSpring() { ks = 0; kd = 0; }
    ForceSpring(double ks, double kd) {this->ks = ks; this->kd = kd;}

    virtual void apply();

    void updateKs(double newKs) { this->ks = newKs; }
    void updateKd(double newKd) { this->kd = newKd; }
    void setL(double l) { this->l = l; }
    double getL(){ return this->l; }

protected:
    double l;
    double ks;
    double kd;
};

class ForceNavierStockes: public Force {
public:
    double h;
    ForceNavierStockes(double h){this->h = h;};

    virtual void apply();

protected:
    double densityCalculation(Particle *p);
    double pressureCalculation(double density);
    void pressureAccelerationCalculation();
    void viscosityAccelerationCalculation();

    double viscosity = 0.05;
    double waterDensity = 1.0;
    double soundSpeed = 10.0; // For dt = 0.0001 s,  1 to 10 for dt = 0.01 to 0.03
};


#endif // FORCES_H
