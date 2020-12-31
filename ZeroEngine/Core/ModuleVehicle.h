#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ModulePhysics.h"
#include "physx/include/PxPhysicsAPI.h"

class GameObject;
class ComponentTransform;

enum
{
	TIRE_TYPE_WETS = 0,
	TIRE_TYPE_SLICKS,
	TIRE_TYPE_ICE,
	TIRE_TYPE_MUD,
	MAX_NUM_TIRE_TYPES
};

enum
{
	MAX_NUM_4W_VEHICLES = 8,
	MAX_NUM_6W_VEHICLES = 2,
	MAX_NUM_4W_TANKS = 2
};

//Collision types and flags describing collision interactions of each collision type.
enum
{
	COLLISION_FLAG_GROUND = 1 << 0,
	COLLISION_FLAG_WHEEL = 1 << 1,
	COLLISION_FLAG_CHASSIS = 1 << 2,
	COLLISION_FLAG_OBSTACLE = 1 << 3,
	COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

	COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
};

class ModuleVehicle : public Module {

public:

	ModuleVehicle(Application* app, bool start_enabled = true);
	~ModuleVehicle();
	
	update_status Update(float dt);

	//Vehicle creation test
	void create4WVehicle(physx::PxScene& scene, physx::PxPhysics& physics, physx::PxCooking& cooking, const physx::PxMaterial& material, const physx::PxF32 chassisMass, 
		const physx::PxVec3* wheelCentreOffsets4, physx::PxConvexMesh* chassisConvexMesh, physx::PxConvexMesh** wheelConvexMeshes4, const physx::PxTransform& startTransform, const bool useAutoGearFlag);
	
	physx::PxRigidDynamic* createVehicleActor4W(const physx::PxVehicleChassisData& chassisData, physx::PxConvexMesh** wheelConvexMeshes, physx::PxConvexMesh* chassisConvexMesh, 
		physx::PxScene& scene, physx::PxPhysics& physics, const physx::PxMaterial& material);
	
	void resetNWCar(const physx::PxTransform& startTransform, physx::PxVehicleWheels* vehWheels);

	void createVehicle4WSimulationData(const physx::PxF32 chassisMass, physx::PxConvexMesh* chassisConvexMesh, const physx::PxF32 wheelMass, physx::PxConvexMesh** wheelConvexMeshes, 
		const physx::PxVec3* wheelCentreOffsets, physx::PxVehicleWheelsSimData& wheelsData, physx::PxVehicleDriveSimData4W& driveData, physx::PxVehicleChassisData& chassisData);
	
	physx::PxVec3 computeChassisAABBDimensions(const physx::PxConvexMesh* chassisConvexMesh);
	
	void computeWheelWidthsAndRadii(physx::PxConvexMesh** wheelConvexMeshes, physx::PxF32* wheelWidths, physx::PxF32* wheelRadii);
	
	void setupActor(physx::PxRigidDynamic* vehActor, const physx::PxFilterData& vehQryFilterData, const physx::PxGeometry** wheelGeometries, const physx::PxTransform* wheelLocalPoses, 
		const physx::PxU32 numWheelGeometries, const physx::PxMaterial* wheelMaterial, const physx::PxFilterData& wheelCollFilterData, const physx::PxGeometry** chassisGeometries, 
		const physx::PxTransform* chassisLocalPoses, const physx::PxU32 numChassisGeometries, const physx::PxMaterial* chassisMaterial, const physx::PxFilterData& chassisCollFilterData, 
		const physx::PxVehicleChassisData& chassisData, physx::PxPhysics* physics);

public:

	physx::PxVehicleWheels* mVehicles[MAX_NUM_4W_VEHICLES + MAX_NUM_6W_VEHICLES];
	physx::PxVehicleWheelQueryResult mVehicleWheelQueryResults[MAX_NUM_4W_VEHICLES + MAX_NUM_6W_VEHICLES];
	physx::PxU32 mNumVehicles;

	
};