#ifndef PARTICLEHASHGRID_H
#define PARTICLEHASHGRID_H

#include "stdlib.h"

#include <Particle.h>
#include <vector>

class ParticleHashGrid
{
public:
    ParticleHashGrid(double h, int numberOfParticles){
        this->h = h;
        this->tableSize = 2 * numberOfParticles;
    };

    int hashCoords(double xi, double yi, double zi);
    int intCoord(double coord);
    void create(std::vector<Particle*> particles);
    void query(Vec3 pos, double maxDist);
    std::vector<int> getQueryIds(){return this->queryIds;};
    int getQuerySize(){return this->querySize;};


protected:
    double h;
    int tableSize;
    std::vector<int> cellStart;
    std::vector<int> cellEntries;
    std::vector<int> queryIds;
    int querySize = 0;
};

#endif // PARTICLEHASHGRID_H
