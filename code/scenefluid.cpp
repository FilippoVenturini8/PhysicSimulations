#include "scenefluid.h"
#include "colliders.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <cmath>
#include <Particle.h>

SceneFluid::SceneFluid() {
    widget = new WidgetFluid();
}

SceneFluid::~SceneFluid() {
    if (widget)      delete widget;
    if (shaderPhong) delete shaderPhong;
    if (vaoFloor)  delete vaoFloor;
    if (vaoWall)  delete vaoWall;
    if (vaoSphereS)  delete vaoSphereS;
    if (fGravity) delete fGravity;
    if (fNavierStockes) delete fNavierStockes;
}

void SceneFluid::initialize() {

    // load shaders
    shaderPhong = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");

    // create floor VAO
    Model quad = Model::createQuad();
    vaoFloor = glutils::createVAO(shaderPhong, &quad);
    glutils::checkGLError();

    quad = Model::createQuad();
    vaoWall = glutils::createVAO(shaderPhong, &quad);
    glutils::checkGLError();

    Model sphere = Model::createIcosphere(1);
    vaoSphereS = glutils::createVAO(shaderPhong, &sphere);
    numFacesSphereS = sphere.numFaces();

    numParticlesX = 3;
    numParticlesY = 3;
    numParticlesZ = 3;
    numParticles = numParticlesX * numParticlesY * numParticlesZ;

    this->integrator = new IntegratorSymplecticEuler();
    this->particleHashGrid = new ParticleHashGrid(2*particleRadius, numParticles);
    colliderFloor.setPlane(Vec3(0, 1, 0), 0);

    this->fGravity = new ForceConstAcceleration();
    this->fGravity->setAcceleration(Vec3(0, -9.81, 0));

    this->fNavierStockes = new ForceNavierStockes(2*particleRadius);

    this->forces.push_back(this->fGravity);
    this->forces.push_back(this->fNavierStockes);

    glutils::checkGLError();
}

void SceneFluid::reset()
{
    this->deleteParticles();

    // reset forces
    this->forces.clear();
    fGravity->clearInfluencedParticles();
    fNavierStockes->clearInfluencedParticles();

    for (int i = 0; i < numParticlesX; i++) {
        for (int j = 0; j < numParticlesY; j++) {
            for (int k = 0; k < numParticlesY; k++) {
                //Vec3 pos = Vec3(i + i * 1.5f, j + j * 1.5f, k + k * 1.5f);
                Vec3 pos = Vec3(particleSpacing + i * particleSpacing, particleSpacing + j * particleSpacing, particleSpacing + k * particleSpacing);
                int idx = (i * numParticlesY + j) * numParticlesZ + k;

                Particle* p = new Particle();
                p->id = idx;
                p->pos = pos;
                p->prevPos = pos;
                p->vel = Vec3(0,0,0);
                p->mass = 0.01;
                p->radius = particleRadius;
                p->color = Vec3(235/255.0, 51/255.0, 36/255.0);
                p->isFixed = false;

                this->fGravity->addInfluencedParticle(p);
                this->fNavierStockes->addInfluencedParticle(p);

                this->particles.push_back(p);
            }
        }
    }

    this->forces.push_back(this->fGravity);
    this->forces.push_back(this->fNavierStockes);

    glutils::checkGLError();
}

void SceneFluid::paint(const Camera& camera)
{
    QOpenGLFunctions_3_3_Core* glFuncs = nullptr;
    glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    shaderPhong->bind();
    shaderPhong->setUniformValue("normalSign", 1.0f);

    // camera matrices
    QMatrix4x4 camProj = camera.getPerspectiveMatrix();
    QMatrix4x4 camView = camera.getViewMatrix();
    shaderPhong->setUniformValue("ProjMatrix", camProj);
    shaderPhong->setUniformValue("ViewMatrix", camView);

    // lighting
    const int numLights = 1;
    const QVector3D lightPosWorld[numLights] = {QVector3D(80,80,80)};
    const QVector3D lightColor[numLights] = {QVector3D(1,1,1)};
    QVector3D lightPosCam[numLights];
    for (int i = 0; i < numLights; i++) {
        lightPosCam[i] = camView * lightPosWorld[i];
    }
    shaderPhong->setUniformValue("numLights", numLights);
    shaderPhong->setUniformValueArray("lightPos", lightPosCam, numLights);
    shaderPhong->setUniformValueArray("lightColor", lightColor, numLights);

    // draw the particle spheres
    QMatrix4x4 modelMat;

    if (showParticles) {
        vaoSphereS->bind();
        shaderPhong->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
        shaderPhong->setUniformValue("matshin", 100.f);
        for (int i = 0; i < numParticles; i++) {
            const Particle* particle = this->particles[i];
            Vec3   p = particle->pos;
            Vec3   c = particle->color;

            modelMat = QMatrix4x4();
            modelMat.translate(p[0], p[1], p[2]);
            modelMat.scale(particle->radius);
            shaderPhong->setUniformValue("ModelMatrix", modelMat);
            shaderPhong->setUniformValue("matdiff", GLfloat(c[0]), GLfloat(c[1]), GLfloat(c[2]));
            glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesSphereS, GL_UNSIGNED_INT, 0);
        }
    }

    // TODO: draw colliders and walls
    vaoFloor->bind();
    modelMat = QMatrix4x4();
    modelMat.scale(100, 1, 100);
    shaderPhong->setUniformValue("ModelMatrix", modelMat);
    shaderPhong->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shaderPhong->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shaderPhong->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    vaoWall->bind();
    modelMat = QMatrix4x4();
    modelMat.scale(1, 100, 100);
    shaderPhong->setUniformValue("ModelMatrix", modelMat);
    shaderPhong->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shaderPhong->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shaderPhong->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    shaderPhong->release();
    glutils::checkGLError();
}

void SceneFluid::updateForces() {
    // clear force accumulators
    for (unsigned int i = 0; i < particles.size(); i++) {
        particles[i]->force = Vec3(0.0, 0.0, 0.0);
    }
    // apply forces
    for (unsigned int i = 0; i < forces.size(); i++) {
        forces[i]->apply();
    }
}

void SceneFluid::deleteParticles() {
    for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); it++)
        delete (*it);
    particles.clear();
}

void SceneFluid::update(double dt)
{
    this->particleHashGrid->create(this->particles); //Create the hash grid each step

    for(int i = 0; i < this->particles.size(); i++){ //Iterate all over the particles
        this->particleHashGrid->query(this->particles.at(i)->pos, 2.0f * this->particleRadius); //Find the neighboor of one particle

        for(int j = 0; j < this->particleHashGrid->getQuerySize(); j++){
            Particle* neighbor = particles[this->particleHashGrid->getQueryIds()[j]];
            this->particles[i]->neighbors->push_back(neighbor);
        }
    }

    this->integrator->stepWithoutPS(this->particles, dt);
    //std::cout << "Particle 0 pos: " << this->particles[0]->pos << std::endl;
    for(Particle* p : this->particles){
        if(this->colliderFloor.testCollision(p)){
            this->colliderFloor.resolveCollision(p, colBounce, colFriction);
        }
    }

    this->updateForces();
}


