#include "particlehashgrid.h"
#include <cmath>
#include <iostream>

int ParticleHashGrid::hashCoords(double xi, double yi, double zi)
{
    /*int hashCoord = (intCoord(xi) * 20 + intCoord(yi)) * 20 + intCoord(zi);
    return hashCoord;*/
    int h = (int(xi) * 92837111)^(int(yi) * 689287499)^(int(zi) * 283923481);
    return abs(h) % this->tableSize;
}

int ParticleHashGrid::intCoord(double coord)
{
    return floor(coord / this->h);
}

void ParticleHashGrid::create(std::vector<Particle*> particles)
{
    int numParticles = particles.size();

    std::vector<int> zerosCellStart (this->tableSize + 1, 0);
    std::vector<int> zerosEntries (numParticles, 0);
    std::vector<int> defaultQueryIds (numParticles, -1);

    this->cellStart = zerosCellStart;
    this->cellEntries = zerosEntries;
    this->queryIds = defaultQueryIds;

    for(Particle* p: particles)
    {
        int hash = this->hashCoords(this->intCoord(p->pos.x()), this->intCoord(p->pos.y()), this->intCoord(p->pos.z()));
        this->cellStart[hash]++;
    }

    int start = 0;

    for(int i = 0; i < this->tableSize; i++)
    {
        start += this->cellStart[i];
        this->cellStart[i] = start;
    }

    this->cellStart[this->tableSize] = start;

    for(int i = 0; i < numParticles; i++)
    {
        int hash = this->hashCoords(particles[i]->pos.x(), particles[i]->pos.y(), particles[i]->pos.z());
        this->cellStart[hash]--;
        this->cellEntries[this->cellStart[hash]] = i;
    }
}

void ParticleHashGrid:: query(Vec3 pos, double maxDist)
{
    int x0 = this->intCoord(pos.x() - maxDist);
    int y0 = this->intCoord(pos.y() - maxDist);
    int z0 = this->intCoord(pos.z() - maxDist);
    //std::cout << "pos:" <<pos[i]->pos << std::endl;
    int x1 = this->intCoord(pos.x() + maxDist);
    int y1 = this->intCoord(pos.y() + maxDist);
    int z1 = this->intCoord(pos.z() + maxDist);

    this->querySize = 0;

    /*std::cout << "x0" << x0 << std::endl;
    std::cout << "y0" << y0 << std::endl;
    std::cout << "z0" << z0 << std::endl;

    std::cout << "x1" << x1 << std::endl;
    std::cout << "y1" << y1 << std::endl;
    std::cout << "z1" << z1 << std::endl;*/

    /*for(int t = 0; t < this->cellStart.size(); t++){
        std::cout << "Cell Start: " << this->cellStart[t] << std::endl;
    }*/

    for(int xi = x0; xi <= x1; xi++)
    {
       for(int yi = y0; yi <= y1; yi++)
        {
            for(int zi = z0; zi <= z1; zi++)
            {
               /*std::cout << "xi" << xi << std::endl;
               std::cout << "yi" << yi << std::endl;
               std::cout << "zi" << zi << std::endl;*/

               int hash = this->hashCoords(xi, yi, zi);

               //std::cout << "hash" << hash << std::endl;

               int start = this->cellStart[hash];
               int end = this->cellStart[hash + 1];

               //std::cout << "start: " << this->cellStart[hash] << std::endl;
               //std::cout << "end: " << this->cellStart[hash + 1] << std::endl;

               for(int j = start; j < end; j++) //It enters here only if the partial sums of 2 cells are different
               {
                   //std::cout << "enter" << std::endl;

                   //If the neighbor is not already found due to hash collisions
                   if(std::find(queryIds.begin(), queryIds.end(), this->cellEntries[j]) == queryIds.end()){
                       this->queryIds[this->querySize] = this->cellEntries[j];

                       this->querySize++;

                       //TODO CLEAR BASING ON THE DISTANCES
                   }

               }
            }
        }
    }

}
