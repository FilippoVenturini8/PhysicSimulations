#include "scenefluid.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <cmath>


SceneFluid::SceneFluid() {
    widget = new WidgetFluid();
}

SceneFluid::~SceneFluid() {
    if (widget)      delete widget;
    if (shaderPhong) delete shaderPhong;
    if (vaoFloor)  delete vaoFloor;
    if (vaoWall)  delete vaoWall;
    if (vaoSphereS)  delete vaoSphereS;
    if (vaoCube)  delete vaoCube;
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
    glutils::checkGLError();

    // create cube VAO
    Model cube = Model::createCube();
    vaoCube = glutils::createVAO(shaderPhong, &cube);
    glutils::checkGLError();

    numPartX = boundDimensions/4.f;
    numPartY = boundDimensions/4.f;
    numPartZ = boundDimensions/4.f;
    if(widget->getComboBoxIndex() == 0){
        numPartY = boundDimensions/3.f;
        numPartZ = boundDimensions/2.f;
    }
    numParticles = numPartX * numPartY * numPartZ;

//    particleHashGridGrid = new particleHashGridSystem(2.0f*particleRadius, numParticles);
    particleHashGrid = new ParticleHashGrid(2.0f * particleRadius, numParticles);
    integrator = new IntegratorSymplecticEuler();

    fGravity = new ForceConstAcceleration();
    fGravity->setAcceleration(Vec3(0, -9.81, 0));
    fNavierStockes = new ForceNavierStockes(2.0f*particleRadius);

    colliderFloor.setPlane(Vec3(0, 1, 0), 0);
    colliderCeiling.setPlane(Vec3(0, 1, 0), -boundDimensions);
    colliderWallLeft.setPlane(Vec3(0, 0, 1), 0);
    colliderWallRight.setPlane(Vec3(0, 0, 1), -boundDimensions);
    colliderWallDown.setPlane(Vec3(1, 0, 0), 0);
    colliderWallUp.setPlane(Vec3(1, 0, 0), -boundDimensions);

    for (int i = 0; i < numPartX/2; i++) {
        for (int j = 0; j < numPartY; j++) {
            for (int k = 0; k < numPartZ; k++) {
                Vec3 pos = Vec3(i*particleSpacing, j*particleSpacing, k*particleSpacing);
                Particle* p = new Particle();
                p->id = (i * numPartY + j) * numPartZ + k;
                p->pos = pos;
                p->prevPos = pos;
                p->vel = Vec3(0,0,0);
                p->mass = MASS;
                p->radius = particleRadius;
                p->color = Vec3(45.0, 114.0, 178.0).normalized();
                p->isFixed = false;

                particles.push_back(p);
                fGravity->addInfluencedParticle(p);
                fNavierStockes->addInfluencedParticle(p);
            }
        }
    }
    for (int i = 0; i < numPartX/2; i++) {
        for (int j = 0; j < numPartY; j++) {
            for (int k = 0; k < numPartZ; k++) {
                Vec3 pos = Vec3(i*particleSpacing + 32.f, j*particleSpacing, k*particleSpacing);
                Particle* p = new Particle();
                p->id = (i * numPartY + j) * numPartZ + k;
                p->pos = pos;
                p->prevPos = pos;
                p->vel = Vec3(0,0,0);
                p->mass = MASS;
                p->radius = particleRadius;
                p->color = Vec3(45.0, 114.0, 178.0).normalized();
                p->isFixed = false;

                particles.push_back(p);
                fGravity->addInfluencedParticle(p);
                fNavierStockes->addInfluencedParticle(p);
            }
        }
    }
    forces.push_back(fGravity);
    forces.push_back(fNavierStockes);
}

void SceneFluid::reset()
{
    glutils::checkGLError();

    deleteParticles();

    numPartX = boundDimensions/4.f;
    numPartY = boundDimensions/4.f;
    numPartZ = boundDimensions/4.f;
    if(widget->getComboBoxIndex() == 0){
        numPartY = boundDimensions/3.f;
        numPartZ = boundDimensions/2.f;
    }
    numParticles = numPartX * numPartY * numPartZ;

    // reset forces
    forces.clear();
    fGravity->clearInfluencedParticles();
    fNavierStockes->clearInfluencedParticles();

    // create particles

    if(widget->getComboBoxIndex() == 1){
        for (int i = 0; i < numPartX; i++) {
            for (int j = 0; j < numPartY; j++) {
                for (int k = 0; k < numPartZ; k++) {
                    Vec3 pos = Vec3(i*particleSpacing + 1.5f, j*particleSpacing + 1.5f, k*particleSpacing + 1.5f);
                    Particle* p = new Particle();
                    p->id = (i * numPartY + j) * numPartZ + k;
                    p->pos = pos;
                    p->prevPos = pos;
                    p->vel = Vec3(0,0,0);
                    p->mass = MASS;
                    p->radius = particleRadius;
                    p->color = Vec3(45.0, 114.0, 178.0).normalized();
                    p->isFixed = false;

                    particles.push_back(p);
                    fGravity->addInfluencedParticle(p);
                    fNavierStockes->addInfluencedParticle(p);
                }
            }
        }
    }
    else if(widget->getComboBoxIndex() == 0){
        for (int i = 0; i < numPartX/2; i++) {
            for (int j = 0; j < numPartY; j++) {
                for (int k = 0; k < numPartZ; k++) {
                    Vec3 pos = Vec3(i*particleSpacing, j*particleSpacing, k*particleSpacing);
                    Particle* p = new Particle();
                    p->id = (i * numPartY + j) * numPartZ + k;
                    p->pos = pos;
                    p->prevPos = pos;
                    p->vel = Vec3(0,0,0);
                    p->mass = MASS;
                    p->radius = particleRadius;
                    p->color = Vec3(45.0, 114.0, 178.0).normalized();
                    p->isFixed = false;

                    particles.push_back(p);
                    fGravity->addInfluencedParticle(p);
                    fNavierStockes->addInfluencedParticle(p);
                }
            }
        }
        for (int i = 0; i < numPartX/2; i++) {
            for (int j = 0; j < numPartY; j++) {
                for (int k = 0; k < numPartZ; k++) {
                    Vec3 pos = Vec3(i*particleSpacing + 32.f, j*particleSpacing, k*particleSpacing);
                    Particle* p = new Particle();
                    p->id = (i * numPartY + j) * numPartZ + k;
                    p->pos = pos;
                    p->prevPos = pos;
                    p->vel = Vec3(0,0,0);
                    p->mass = MASS;
                    p->radius = particleRadius;
                    p->color = Vec3(45.0, 114.0, 178.0).normalized();
                    p->isFixed = false;

                    particles.push_back(p);
                    fGravity->addInfluencedParticle(p);
                    fNavierStockes->addInfluencedParticle(p);
                }
            }
        }
    }

    forces.push_back(fGravity);
    forces.push_back(fNavierStockes);
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
            const Particle* particle = particles.at(i);
            Vec3   p = particle->pos;
            Vec3   c = particle->color;

            modelMat = QMatrix4x4();
            modelMat.translate(p[0], p[1], p[2]);
            modelMat.scale(particle->radius);
            shaderPhong->setUniformValue("ModelMatrix", modelMat);
            shaderPhong->setUniformValue("matdiff", GLfloat(c[0]), GLfloat(c[1]), GLfloat(c[2]));
            shaderPhong->setUniformValue("alpha", 0.6f);
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
    shaderPhong->setUniformValue("alpha", 1.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    vaoWall->bind();
    modelMat = QMatrix4x4();
    modelMat.scale(100, 1, 100);
    modelMat.rotate(0,0,90);

    shaderPhong->setUniformValue("ModelMatrix", modelMat);
    shaderPhong->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shaderPhong->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shaderPhong->setUniformValue("matshin", 0.0f);
    shaderPhong->setUniformValue("alpha", 1.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    vaoCube->bind();
    modelMat = QMatrix4x4();
    modelMat.translate(boundDimensions/2.f,boundDimensions/2.f,boundDimensions/2.f);
    modelMat.scale(boundDimensions/2.f, boundDimensions/2.f, boundDimensions/2.f);
    shaderPhong->setUniformValue("ModelMatrix", modelMat);
    shaderPhong->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shaderPhong->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shaderPhong->setUniformValue("matshin", 0.0f);
    shaderPhong->setUniformValue("alpha", 0.3f);
    glFuncs->glDrawElements(GL_TRIANGLES, 100, GL_UNSIGNED_INT, 0);

    shaderPhong->release();

    glutils::checkGLError();
}

void SceneFluid::update(double dt)
{
    particleHashGrid->create(particles);

    for(int i = 0; i < particles.size(); i++) {
        particleHashGrid->query(particles, i, 2*particleRadius);
        particles[i]->neighbors.clear();
        for(int j= 0; j < particleHashGrid->getQuerySize(); j++) {
            Particle* p = particles[particleHashGrid->getNeighbors().at(j)];
            particles[i]->neighbors.insert(p);
        }
    }

    updateForces();
    integrator->stepWithoutPS(particles, dt);

    for (Particle* p : particles) {
        if (colliderFloor.testCollision(p)) {
            colliderFloor.resolveCollision(p, colBounce, colFriction);
        }
        if (colliderCeiling.testCollision(p)) {
            colliderCeiling.resolveCollision(p, colBounce, colFriction);
        }
        if (colliderWallLeft.testCollision(p)) {
            colliderWallLeft.resolveCollision(p, colBounce, colFriction);
        }
        if (colliderWallRight.testCollision(p)) {
            colliderWallRight.resolveCollision(p, colBounce, colFriction);
        }
        if (colliderWallUp.testCollision(p)) {
            colliderWallUp.resolveCollision(p, colBounce, colFriction);
        }
        if (colliderWallDown.testCollision(p)) {
            colliderWallDown.resolveCollision(p, colBounce, colFriction);
        }
    }

    for(int i = 0; i < particles.size(); i++) {
        particleCollisions(particles.at(i));
    }
}

void SceneFluid::updateForces(){
    for (unsigned int i = 0; i < particles.size(); i++) {
        particles[i]->force = Vec3(0.0, 0.0, 0.0);
    }
    // apply forces
    for (unsigned int i = 0; i < forces.size(); i++) {
        forces[i]->apply();
    }
}

void SceneFluid::deleteParticles(){
    for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); it++)
        delete (*it);
    particles.clear();
}
