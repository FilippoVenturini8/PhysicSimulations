#ifndef SCENEFLUID_H
#define SCENEFLUID_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <Particle.h>
#include <ParticleHashGrid.h>
#include "colliders.h"
#include "forces.h"
#include "integrators.h"
#include "scene.h"
#include "widgetfluid.h"

class SceneFluid : public Scene
{
    Q_OBJECT

public:
    SceneFluid();
    virtual ~SceneFluid();

    virtual void initialize();
    virtual void reset();
    virtual void update(double dt);
    virtual void paint(const Camera& cam);

    virtual void updateForces();
    virtual void deleteParticles();

    virtual void getSceneBounds(Vec3& bmin, Vec3& bmax) {
        bmin = Vec3(-100, -100, -100);
        bmax = Vec3( 100,  100,  100);
    }

    virtual QWidget* sceneUI() { return widget; }

protected:
    // ui
    WidgetFluid* widget = nullptr;

    // opengl & render
    QOpenGLShaderProgram* shaderPhong = nullptr;
    QOpenGLVertexArrayObject* vaoSphereS = nullptr;
    QOpenGLVertexArrayObject* vaoFloor   = nullptr;
    QOpenGLVertexArrayObject* vaoWall    = nullptr;
    QOpenGLVertexArrayObject* vaoCube    = nullptr;
    unsigned int numFacesSphereS = 0, numFacesSphereL = 0;
    unsigned int numMeshIndices = 0;

    ParticleHashGrid* particleHashGrid;

    std::vector<Particle*> particles;
    std::vector<Force*> forces;
    int numPartX;
    int numPartY;
    int numPartZ;
    int numParticles;
    bool showParticles = true;
    double particleRadius = 1;
    double particleSpacing = 2.0f * particleRadius;

    ForceConstAcceleration* fGravity = nullptr;
    ForceNavierStockes* fNavierStockes = nullptr;

    IntegratorSymplecticEuler* integrator = nullptr;

    ColliderPlane colliderFloor;
    ColliderPlane colliderCeiling;
    ColliderPlane colliderWallLeft;
    ColliderPlane colliderWallRight;
    ColliderPlane colliderWallUp;
    ColliderPlane colliderWallDown;

    double colBounce = 0.01;
    double colFriction = 0.05;

    float MASS = 1.0f;
    float boundDimensions = 40.0f;
};

#endif // SCENEFLUID_H
