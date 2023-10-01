#include "scenefountain.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>


SceneFountain::SceneFountain() {
    widget = new WidgetFountain();
    connect(widget, SIGNAL(updatedParameters()), this, SLOT(updateSimParams()));
}


SceneFountain::~SceneFountain() {
    if (widget)         delete widget;
    if (shader)         delete shader;
    if (vaoFloor)       delete vaoFloor;
    if (vaoSphereS)     delete vaoSphereS;
    if (vaoCube)        delete vaoCube;
    if (vaoSphereL)     delete vaoSphereL;
    if (vaoBlackHole)   delete vaoBlackHole;
    if (fGravity)       delete fGravity;
}


void SceneFountain::initialize() {
    // load shader
    shader = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");

    // create floor VAO
    Model quad = Model::createQuad();
    vaoFloor = glutils::createVAO(shader, &quad);
    glutils::checkGLError();

    // create cube VAO
    Model cube = Model::createCube();
    vaoCube = glutils::createVAO(shader, &cube);
    glutils::checkGLError();

    // create sphere VAOs
    Model sphere = Model::createIcosphere(1);
    vaoSphereS = glutils::createVAO(shader, &sphere);
    numFacesSphereS = sphere.numFaces();
    glutils::checkGLError();

    // create large sphere VAOs
    Model sphereL = Model::createIcosphere(3);
    vaoSphereL = glutils::createVAO(shader, &sphereL);
    numFacesSphereL = sphereL.numFaces();
    glutils::checkGLError();

    // create sphere VAOs
    Model blackHole = Model::createIcosphere(2);
    vaoBlackHole = glutils::createVAO(shader, &blackHole);
    numFacesBlackHole = blackHole.numFaces();
    glutils::checkGLError();

    // scene
    fountainPos = Vec3(0, 100, 0);
    cubePos = Vec3(-50, 30, 10);
    spherePos = Vec3(0, 0, 0);
    blackHolePos = Vec3(50, 50, 0);
    cubeSide = 10;
    colliderFloor.setPlane(Vec3(0, 1, 0), 0);
    colliderCube = ColliderCube(cubePos, cubeSide);
    colliderSphere = ColliderSphere(spherePos, 25);

    // create forces
    fGravity = new ForceConstAcceleration();
    fGravitationalAttraction = new ForceGravitationalAttraction(blackHolePos);
    system.addForce(fGravity);
    system.addForce(fGravitationalAttraction);    
}


void SceneFountain::reset()
{
    // update values from UI
    updateSimParams();

    // reset random seed
    Random::seed(1337);

    // erase all particles
    fGravity->clearInfluencedParticles();
    fGravitationalAttraction->clearInfluencedParticles();
    system.deleteParticles();
    deadParticles.clear();
}


void SceneFountain::updateSimParams()
{
    // get gravity from UI and update force
    double g = widget->getGravity();
    fGravity->setAcceleration(Vec3(0, -g, 0));

    // get other relevant UI values and update simulation params
    kBounce = 0.5;
    kFriction = 0.1;
    maxParticleLife = 10.0;
    emitRate = 100;
}


void SceneFountain::paint(const Camera& camera) {

    QOpenGLFunctions_3_3_Core* glFuncs = nullptr;
    glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    shader->bind();

    // camera matrices
    QMatrix4x4 camProj = camera.getPerspectiveMatrix();
    QMatrix4x4 camView = camera.getViewMatrix();
    shader->setUniformValue("ProjMatrix", camProj);
    shader->setUniformValue("ViewMatrix", camView);

    // lighting
    const int numLights = 1;
    const QVector3D lightPosWorld[numLights] = {QVector3D(100,500,100)};
    const QVector3D lightColor[numLights] = {QVector3D(1,1,1)};
    QVector3D lightPosCam[numLights];
    for (int i = 0; i < numLights; i++) {
        lightPosCam[i] = camView * lightPosWorld[i];
    }
    shader->setUniformValue("numLights", numLights);
    shader->setUniformValueArray("lightPos", lightPosCam, numLights);
    shader->setUniformValueArray("lightColor", lightColor, numLights);

    // draw floor
    vaoFloor->bind();
    QMatrix4x4 modelMat;
    modelMat.scale(100, 1, 100);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // draw cube

    vaoCube->bind();
    modelMat = QMatrix4x4();
    modelMat.translate(cubePos.x(), cubePos.y(), cubePos.z());
    modelMat.scale(cubeSide, cubeSide, cubeSide);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 100, GL_UNSIGNED_INT, 0);


    // draw the big sphere

    vaoSphereL->bind();
    modelMat = QMatrix4x4();
    modelMat.translate(spherePos.x(), spherePos.y(), spherePos.z());
    modelMat.scale(25.0);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);

    glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesSphereL, GL_UNSIGNED_INT, 0);

    // draw the black hole

    vaoBlackHole->bind();
    modelMat = QMatrix4x4();
    modelMat.translate(blackHolePos.x(), blackHolePos.y(), blackHolePos.z());
    modelMat.scale(5.0);
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);

    glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesBlackHole, GL_UNSIGNED_INT, 0);

    // draw the different spheres
    vaoSphereS->bind();
    for (const Particle* particle : system.getParticles()) {
        Vec3   p = particle->pos;
        Vec3   c = particle->color;
        double r = particle->radius;

        modelMat = QMatrix4x4();
        modelMat.translate(p[0], p[1], p[2]);
        modelMat.scale(r);
        shader->setUniformValue("ModelMatrix", modelMat);

        shader->setUniformValue("matdiff", GLfloat(c[0]), GLfloat(c[1]), GLfloat(c[2]));
        shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
        shader->setUniformValue("matshin", 100.f);

        glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesSphereS, GL_UNSIGNED_INT, 0);
    }

    shader->release();
}


void SceneFountain::update(double dt) {

    // emit new particles, reuse dead ones if possible
    int emitParticles = std::max(1, int(std::round(emitRate * dt)));
    for (int i = 0; i < emitParticles; i++) {
        Particle* p;
        if (!deadParticles.empty()) {
            // reuse one dead particle
            p = deadParticles.front();
            deadParticles.pop_front();
        }
        else {
            // create new particle
            p = new Particle();
            system.addParticle(p);

            // don't forget to add particle to forces that affect it
            fGravity->addInfluencedParticle(p);
            fGravitationalAttraction->addInfluencedParticle(p);
        }

        if(widget->isBlackHoleActive()){
            fGravitationalAttraction->enableBlackHole();
        }else{
            fGravitationalAttraction->disableBlackHole();
        }

        p->color = Vec3(153/255.0, 217/255.0, 234/255.0);
        p->radius = 1.0;
        p->life = maxParticleLife;

        double x = Random::get(-10.0, 10.0);
        double y = 0;
        double z = Random::get(-10.0, 10.0);
        p->pos = Vec3(x, y, z) + fountainPos;
        p->vel = Vec3((float) rand()/RAND_MAX - 0.5, 0, (float) rand()/RAND_MAX - 0.5);
    }

    // integration step
    Vecd ppos = system.getPositions();
    integrator.step(system, dt);
    system.setPreviousPositions(ppos);

    // collisions
    for (Particle* p : system.getParticles()) {
        if (colliderFloor.testCollision(p)) {
            colliderFloor.resolveCollision(p, kBounce, kFriction);
        }
        /*if(colliderCube.testCollision(p)){
            std::cout << "collision" << std::endl;
        }*/
        if(colliderSphere.testCollision(p)){
            colliderSphere.resolveCollision(p, kBounce, kFriction);
        }
    }

    // check dead particles
    for (Particle* p : system.getParticles()) {
        if (p->life > 0) {
            p->life -= dt;
            if (p->life < 0) {
                deadParticles.push_back(p);
            }
        }
    }
}

void SceneFountain::mousePressed(const QMouseEvent* e, const Camera&)
{
    mouseX = e->pos().x();
    mouseY = e->pos().y();
}

void SceneFountain::mouseMoved(const QMouseEvent* e, const Camera& cam)
{
    int dx = e->pos().x() - mouseX;
    int dy = e->pos().y() - mouseY;
    mouseX = e->pos().x();
    mouseY = e->pos().y();

    Vec3 disp = cam.worldSpaceDisplacement(dx, -dy, cam.getEyeDistance());

    // example
    if (e->modifiers() & Qt::ControlModifier) {
        // move fountain
        fountainPos += disp;
    }
    else {
        // do something else: e.g. move colliders
        spherePos += disp;
        colliderSphere.updateCenter(spherePos);
    }
}
