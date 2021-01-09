#pragma once

#include "Component.h"
#include "Globals.h"
#include "Module.h"
#include "MathGeoLib/include/MathGeoLib.h"

#include "ModulePhysics.h"
#include "PhysX/include/PxPhysicsAPI.h"
#include "PhysX/include/vehicle/PxVehicleUtil.h"

#include "PxVehicleSceneQuery.h"
#include "PxVehicleFilterShader.h"
#include "PxVehicleTireFriction.h"
#include "PxVehicleCreate.h"

class GameObject;
class ComponentTransform;
struct VehicleDesc;

using namespace snippetvehicle;

enum class DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

class ModuleVehicle : public Module {

public:

	ModuleVehicle(Application* app, bool start_enabled = true);
	~ModuleVehicle();
	
	bool Init();
	update_status Update(float dt);

	void CreateVehicle(physx::PxF32 mass = 1500.0f, physx::PxVec3 dimensions = { 2.5f, 2.0f, 5.0f }, physx::PxF32 wmass = 20.0f, physx::PxF32 wradius = 0.5f, physx::PxF32 wwidth = 0.4f);
	
	//--Vehicle Functions
	void startAccelerateForwardsMode();
	void startAccelerateReverseMode();
	void startBrakeMode();
	void startTurnHardLeftMode();
	void startTurnHardRightMode();
	void startHandbrakeTurnLeftMode();
	void startHandbrakeTurnRightMode();
	void releaseAllControls();

	snippetvehicle::VehicleDesc initVehicleDesc(physx::PxF32 mass, physx::PxVec3 dimensions, physx::PxF32 wmass, physx::PxF32 wradius, physx::PxF32 wwidth);
	void incrementDrivingMode(const physx::PxF32 timestep, DriveMode type);

public:

	snippetvehicle::VehicleSceneQueryData* gVehicleSceneQueryData = nullptr;
	physx::PxBatchQuery* gBatchQuery = nullptr;

	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = nullptr;

	physx::PxRigidStatic* gGroundPlane = nullptr;
	physx::PxVehicleDrive4W* gVehicle4W = nullptr;

	bool gIsVehicleInAir = true;
	bool gVehicleOrderComplete = false;
	bool gMimicKeyInputs = false;

	physx::PxF32 gSteerVsForwardSpeedData[16] =
	{
		0.0f,		0.75f,
		5.0f,		0.75f,
		30.0f,		0.125f,
		120.0f,		0.1f,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32,
		PX_MAX_F32, PX_MAX_F32
	};

	physx::PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable = { gSteerVsForwardSpeedData, 4 };

	physx::PxVehicleKeySmoothingData gKeySmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	physx::PxVehiclePadSmoothingData gPadSmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	physx::PxVehicleDrive4WRawInputData gVehicleInputData;

	physx::PxF32 gVehicleModeLifetime = 4.0f;
	physx::PxF32 gVehicleModeTimer = 0.0f;
	physx::PxU32 gVehicleOrderProgress = 0;
	
	//-- Vehicle Config --//
	bool use_gravity = true;
	bool use_kinematic = false;

	bool lock_linearX = false, lock_linearY = false, lock_linearZ = false;
	bool lock_angularX = false, lock_angularY = false, lock_angularZ = false;

	float linear_damping = 0.0f;
	float angular_damping = 0.05f;

	//Chassis
	float mass = 1500.0f;
	float3 dimensions = { 2.5f, 2.0f, 5.0f };

	//Wheel
	float wmass = 20.0f;
	float wradius = 0.5f;
	float wwidth = 0.4f;

};


