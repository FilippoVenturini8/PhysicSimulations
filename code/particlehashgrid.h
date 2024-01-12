#ifndef PARTICLEHASHGRID_H
#define PARTICLEHASHGRID_H

#include "stdlib.h"

#include <Particle.h>
#include <vector>

class ParticleHashGrid {
private:
    double spacing;
    int tableSize;
    std::vector<int> cellStart;
    std::vector<int> cellEntries;
    std::vector<int> queryIds;
    int querySize;

    int hashCoords(int xi, int yi, int zi);
    int intCoord(double coord);
    int hashPos(std::vector<double>& pos, int nr);

public:
    ParticleHashGrid(double spacing, int maxNumObjects);

    void create(std::vector<Particle *> particles);
    void query(const std::vector<Particle *>& particles, int i, double maxDist);
    std::vector<int> getNeighbors(){return queryIds;};
    int getQuerySize(){return querySize;};
};

#endif // PARTICLEHASHGRID_H
