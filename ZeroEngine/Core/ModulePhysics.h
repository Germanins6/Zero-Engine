#pragma once
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "PhysX/include/common/PxCoreUtilityTypes.h"
#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleSDK.h"
#include "PhysX/include/extensions/PxDefaultAllocator.h"
#include "GameObject.h"
#include "Globals.h"
class ComponentCollider;

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
    class PxActor;
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
    class PxShape;
    class PxVec3;
    class PxJoint;
    class PxFilterData;
    class PxQueryHitType;
    class PxDefaultAllocator;
   
    //class PxFixedSizeLookupTable;

    //Vehicles
    class PxVehicleDrive4W;
    class PxVehicleChassisData;
    class PxVehicleWheelsSimData;
    class PxVehicleDriveSimData4W;
    class PxVehicleDrivableSurfaceToTireFrictionPairs;
    class PxVehicleWheels;
    class PxVehicleKeySmoothingData;
    class PxVehiclePadSmoothingData;
    class PxVehicleDrive4WRawInputData;

    typedef uint32_t PxU32;
    typedef float PxF32;

};



enum class JointType {
    FIXED,
    DISTANCE,
    SPHERICAL,
    REVOLUTE,
    PRISMATIC,
    D6,
};

class ModulePhysics : public Module {

public:

	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Init();
    update_status Update(float dt);
	bool CleanUp();

    void SceneSimulation(float gameTimesetp, bool fetchResults = true);
    void RenderGeometry();
    
    void renderActors(physx::PxRigidActor** actors, const physx::PxU32 numActors, bool shadows);
    void renderGeometry(const physx::PxGeometry& geom);
    void renderGeometryHolder(const physx::PxGeometryHolder& h);

    void DrawGeometry(GeometryType type, float3 pos = { 0.0f, 0.0f, 0.0f }, float radius = 3.0f, float3 size = { 1.0f, 1.0f, 1.0f });
    void DrawCollider(ComponentCollider* collider);
    void DrawBoxCollider(const float4x4& transform, const float3& half_size) ;
    void DrawSphereCollider(const float4x4& transform, float radius);
    void DrawCapsuleCollider(const float4x4& transform, const float half_height, const float radius);
    
    void WakeUpGeometry(GameObject* gameObject);
    // SPACE - M - N testing geometries
    physx::PxRigidDynamic* CreateGeometry(GeometryType type = GeometryType::NONE, float3 pos = { 0.0f, 0.0f, 0.0f }, float mass = 10.0f, float radius = 3.0f, float3 size = { 1.0f, 1.0f, 1.0f });
    
    physx::PxRigidDynamic* CreateRigidbody(float3 pos);
    physx::PxShape* CreateCollider(GeometryType colliderType, float3 size);
    physx::PxJoint* CreateJoint(JointType jointType);

    float4x4 PhysXTransformToF4F(physx::PxTransform transform);
public:
    physx::PxFoundation* mFoundation;
    physx::PxPhysics* mPhysics;
    physx::PxPvd* mPvd;
    physx::PxCooking* mCooking;
    physx::PxMaterial* mMaterial;
    physx::PxScene* mScene;
    physx::PxDefaultCpuDispatcher* mDispatcher;
    physx::PxDefaultAllocator		mAllocator;

    float gravity;
};