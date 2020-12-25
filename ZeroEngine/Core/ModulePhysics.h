#pragma once
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"

namespace physx
{
    class PxPvd;
    class PxPvdSceneClient;
    class PxFoundation;
    class PxPhysics;
    class PxScene;
    class PxMaterial;
    class PxRigidStatic;
    class PxControllerManager;
    class PxRigidActor;
    class PxVolumeCache;
    class PxSimulationEventCallback;
    class PxActorShape;
    class PxQueryFilterCallback;
    class RaycastCCDManager;
    class PxCooking;
    class PxConvexMesh;
    class PxBase;
    class PxDefaultCpuDispatcher;
    class PxRigidDynamic;
    class PxRigidBody;
    class PxGeometry;
    class PxGeometryHolder;

    typedef uint32_t PxU32;
};

class ModulePhysics : public Module {

public:

	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Init();
    update_status Update(float dt);
	bool CleanUp();

    void SceneSimulation(bool fetchResults = true);
    void RenderGeometry();
    
    void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows);
    void renderGeometry(const physx::PxGeometry& geom);
    void renderGeometryHolder(const physx::PxGeometryHolder& h);
    
    physx::PxRigidDynamic* CreateSphere(float3 pos = { 0.0f, 15.0f, 0.0f }, float radius = 3.0f, float mass = 10.0f);
    void DrawSphere(float radius = 3.0f, float3 pos = { 0.0f, 0.0f, 0.0f });
    
    physx::PxRigidDynamic* CreateCapsule(float3 pos = { 0.0f, 15.0f, 0.0f }, float radius = 3.0f, float3 size = { 1.0f, 5.0f, 1.0f }, float mass = 10.0f);
    void DrawCylinder(float radius = 3.0f, float3 pos = { 0.0f, 0.0f, 0.0f });

    physx::PxRigidDynamic* CreateBox(float3 pos = { 0.0f, 15.0f, 0.0f }, float3 size = { 2.0f, 2.0f, 2.0f }, float mass = 10.0f);
    void DrawBox(float3 size);

public:
    physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxPvd* mPvd;
    physx::PxCooking* mCooking;
    physx::PxMaterial* mMaterial;
    physx::PxScene* mScene;
    physx::PxDefaultCpuDispatcher* mDispatcher;

    float gravity;
};