#pragma once
#include "Module.h"

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

    typedef uint32_t PxU32;
};

class ModulePhysics : public Module {

public:

	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Init();
	bool CleanUp();


public:
	physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxPvd* mPvd;
};