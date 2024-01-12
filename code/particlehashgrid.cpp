#include "particlehashgrid.h"
#include <iostream>

int ParticleHashGrid::hashCoords(int xi, int yi, int zi) {
    int h = (xi * 92837111) ^ (yi * 689287499) ^ (zi * 283923481); // Fantasy function
    return std::abs(h) % tableSize;
}

int ParticleHashGrid::intCoord(double coord) {
    return static_cast<int>(std::floor(coord / spacing));
}

int ParticleHashGrid::hashPos(std::vector<double>& pos, int nr) {
    return hashCoords(
        intCoord(pos[3 * nr]),
        intCoord(pos[3 * nr + 1]),
        intCoord(pos[3 * nr + 2])
        );
}

ParticleHashGrid::ParticleHashGrid(double spacing, int maxNumObjects) : spacing(spacing) {
    tableSize = 2 * maxNumObjects;
    cellStart.resize(tableSize + 1);
    cellEntries.resize(maxNumObjects);
    queryIds.resize(maxNumObjects);
    querySize = 0;
}

void ParticleHashGrid::create(std::vector<Particle *> particles) {
    int numObjects = std::min(static_cast<int>(particles.size()), static_cast<int>(cellEntries.size()));

    // Determine cell sizes
    std::fill(cellStart.begin(), cellStart.end(), 0);
    std::fill(cellEntries.begin(), cellEntries.end(), 0);

    for(int i = 0; i < numObjects; i++){
        Vec3 position = particles.at(i)->pos;
        int hash = this->hashCoords(intCoord(position.x()), intCoord(position.y()), intCoord(position.z()));
        this->cellStart[hash]++;
    }

    // Determine cell starts
    int start = 0;
    for (int i = 0; i < tableSize; i++) {
        start += cellStart[i];
        cellStart[i] = start;
    }
    cellStart[tableSize] = start;

    // Fill in objects ids
    for (int i = 0; i < numObjects; i++) {
        Vec3 position = particles.at(i)->pos;
        int h = this->hashCoords(intCoord(position.x()), intCoord(position.y()), intCoord(position.z()));
        cellStart[h]--;
        cellEntries[cellStart[h]] = i;
    }
}

void ParticleHashGrid::query(const std::vector<Particle *>& particles, int i, double maxDist) {
    int x0 = intCoord(particles.at(i)->pos.x() - maxDist);
    int y0 = intCoord(particles.at(i)->pos.y() - maxDist);
    int z0 = intCoord(particles.at(i)->pos.z() - maxDist);

    int x1 = intCoord(particles.at(i)->pos.x() + maxDist);
    int y1 = intCoord(particles.at(i)->pos.y() + maxDist);
    int z1 = intCoord(particles.at(i)->pos.z() + maxDist);

    querySize = 0;

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int h = hashCoords(xi, yi, zi);
                int start = cellStart[h];
                int end = cellStart[h + 1];

                for (int i = start; i < end; i++) {
                    queryIds[querySize] = cellEntries[i];
                    querySize++;
                }
            }
        }
    }
}
