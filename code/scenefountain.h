#ifndef SCENEFOUNTAIN_H
#define SCENEFOUNTAIN_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <list>
#include "scene.h"
#include "widgetfountain.h"
#include "particlesystem.h"
#include "integrators.h"
#include "colliders.h"

class SceneFountain : public Scene
{
    Q_OBJECT

public:
    SceneFountain();
    virtual ~SceneFountain();

    virtual void initialize();
    virtual void reset();
    virtual void update(double dt);
    virtual void paint(const Camera& cam);

    virtual void mousePressed(const QMouseEvent* e, const Camera& cam);
    virtual void mouseMoved(const QMouseEvent* e, const Camera& cam);

    virtual void getSceneBounds(Vec3& bmin, Vec3& bmax) {
        bmin = Vec3(-50, -10, -50);
        bmax = Vec3( 50, 100, 50);
    }
    virtual unsigned int getNumParticles() { return system.getNumParticles(); }

    virtual QWidget* sceneUI() { return widget; }

public slots:
    void updateSimParams();

protected:
    WidgetFountain* widget = nullptr;

    QOpenGLShaderProgram* shader = nullptr;
    QOpenGLVertexArrayObject* vaoSphereS = nullptr;
    QOpenGLVertexArrayObject* vaoSphereL = nullptr;
    QOpenGLVertexArrayObject* vaoFloor   = nullptr;
    QOpenGLVertexArrayObject* vaoCube = nullptr;
    QOpenGLVertexArrayObject* vaoBlackHole = nullptr;
    unsigned int numFacesSphereS = 0;
    unsigned int numFacesSphereL = 0;
    unsigned int numFacesBlackHole = 0;
    unsigned int cubeSide = 0;
    unsigned int sphereSize = 0;

    IntegratorEuler integrator;
    ParticleSystem system;
    std::list<Particle*> deadParticles;
    ForceConstAcceleration* fGravity;
    ForceGravitationalAttraction* fGravitationalAttraction;
    ColliderPlane colliderFloor;
    ColliderAABB colliderCube;
    ColliderSphere colliderSphere;

    double kBounce, kFriction;
    double emitRate;
    double maxParticleLife;

    Vec3 fountainPos;
    Vec3 cubePos;
    Vec3 spherePos;
    Vec3 blackHolePos;
    int mouseX, mouseY;
};

#endif // SCENEFOUNTAIN_H
