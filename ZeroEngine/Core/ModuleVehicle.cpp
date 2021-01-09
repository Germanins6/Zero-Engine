#include "Application.h"
#include "ModuleVehicle.h"

using namespace physx;

ModuleVehicle::ModuleVehicle(Application* app, bool start_enabled) : Module(app, start_enabled) {
	
	gVehicle4W = nullptr;
}

ModuleVehicle::~ModuleVehicle() {

}

update_status ModuleVehicle::Update(float dt) {

	if(gVehicle4W != nullptr){

		//Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
		//incrementDrivingMode(dt);

		//Update the control inputs for the vehicle.
		if (gMimicKeyInputs)
		{
			PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, dt, gIsVehicleInAir, *gVehicle4W);
		}
		else
		{
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, gVehicleInputData, dt, gIsVehicleInAir, *gVehicle4W);
		}

		//Raycasts.
		PxVehicleWheels* vehicles[1] = { gVehicle4W };
		PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(gBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const PxVec3 grav = App->physX->mScene->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = { {wheelQueryResults, gVehicle4W->mWheelsSimData.getNbWheels()} };
		PxVehicleUpdates(dt, grav, *gFrictionPairs, 1, vehicles, vehicleQueryResults);

		//Work out if the vehicle is in the air.
		gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
	}
	
	return UPDATE_CONTINUE;
}

bool ModuleVehicle::Init() {

	PxInitVehicleSDK(*App->physX->mPhysics);
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, WheelSceneQueryPreFilterBlocking, NULL, App->physX->mAllocator);
	gBatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, App->physX->mScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = snippetvehicle::createFrictionPairs(App->physX->mMaterial);

	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = snippetvehicle::createDrivablePlane(groundPlaneSimFilterData, App->physX->mMaterial, App->physX->mPhysics);
	App->physX->mScene->addActor(*gGroundPlane);

	return true;

}

void ModuleVehicle::CreateVehicle(PxF32 mass, PxVec3 dimensions, PxF32 wmass, PxF32 wradius, PxF32 wwidth) {
	
	//Create a vehicle that will drive on the plane.
	snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc(mass, dimensions, wmass, wradius, wwidth);

	gVehicle4W = snippetvehicle::createVehicle4W(vehicleDesc, App->physX->mPhysics, App->physX->mCooking);

	GameObject* vehicle = App->scene->CreateGameObject();
	vehicle->name = "vehicle";

	float3 pos, scale;
	Quat rot;

	vehicle->GetTransform()->GetGlobalMatrix().Transposed().Decompose(pos, rot, scale);
	
	PxTransform startTransform({ pos.x,pos.y,pos.z }, { rot.x, rot.y, rot.z , rot.w });

	//PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y * 0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	App->physX->mScene->addActor(*gVehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	gVehicle4W->setToRestState();
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	gVehicle4W->mDriveDynData.setUseAutoGears(true);

	gVehicleModeTimer = 0.0f;
	gVehicleOrderProgress = 0;
	
}

snippetvehicle::VehicleDesc ModuleVehicle::initVehicleDesc(PxF32 mass, PxVec3 dimensions, PxF32 wmass, PxF32 wradius, PxF32 wwidth)
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = mass;
	const PxVec3 chassisDims(dimensions);
	const PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * 0.8f * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = wmass;
	const PxF32 wheelRadius = wradius;
	const PxF32 wheelWidth = wwidth;
	const PxF32 wheelMOI = 0.5f * wheelMass * wheelRadius * wheelRadius;
	const PxU32 nbWheels = 4;

	snippetvehicle::VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = App->physX->mMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = App->physX->mMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

void ModuleVehicle::incrementDrivingMode(const PxF32 timestep, DriveMode type)
{
	LOG("%f %f %f", gVehicle4W->getRigidDynamicActor()->getLinearVelocity().x, gVehicle4W->getRigidDynamicActor()->getLinearVelocity().y, gVehicle4W->getRigidDynamicActor()->getLinearVelocity().z);
	gVehicleModeTimer += timestep;
	if (gVehicleModeTimer > gVehicleModeLifetime)
	{
		//If we move Forwards we change Gear to First
		if (DriveMode::eDRIVE_MODE_ACCEL_FORWARDS == type)
		{
			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		}

		//If we move Backwars we change Gear to Reverse
		if (DriveMode::eDRIVE_MODE_ACCEL_REVERSE == type)
		{
			gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		}

		//Start driving in the selected mode.
		DriveMode eDriveMode = type;
		switch (eDriveMode)
		{
		case DriveMode::eDRIVE_MODE_ACCEL_FORWARDS:
			startAccelerateForwardsMode();
			break;
		case DriveMode::eDRIVE_MODE_ACCEL_REVERSE:
			startAccelerateReverseMode();
			break;
		case DriveMode::eDRIVE_MODE_HARD_TURN_LEFT:
			startTurnHardLeftMode();
			break;
		case DriveMode::eDRIVE_MODE_HANDBRAKE_TURN_LEFT:
			startHandbrakeTurnLeftMode();
			break;
		case DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT:
			startTurnHardRightMode();
			break;
		case DriveMode::eDRIVE_MODE_HANDBRAKE_TURN_RIGHT:
			startHandbrakeTurnRightMode();
			break;
		case DriveMode::eDRIVE_MODE_BRAKE:
			startBrakeMode();
			break;
		case DriveMode::eDRIVE_MODE_NONE:
			break;
		};

	}
}

void  ModuleVehicle::startAccelerateForwardsMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void  ModuleVehicle::startAccelerateReverseMode()
{
	gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(1.0f);
	}
}

void  ModuleVehicle::startBrakeMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalBrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogBrake(1.0f);
	}
}

void  ModuleVehicle::startTurnHardLeftMode()
{
	if (gMimicKeyInputs)
	{
		//gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerLeft(true);
	}
	else
	{
		//gVehicleInputData.setAnalogAccel(true);
		gVehicleInputData.setAnalogSteer(-1.0f);
	}
}

void  ModuleVehicle::startTurnHardRightMode()
{
	if (gMimicKeyInputs)
	{
		//gVehicleInputData.setDigitalAccel(true);
		gVehicleInputData.setDigitalSteerRight(true);
	}
	else
	{
		//gVehicleInputData.setAnalogAccel(1.0f);
		gVehicleInputData.setAnalogSteer(1.0f);
	}
}

void  ModuleVehicle::startHandbrakeTurnLeftMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerLeft(true);
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(-1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void  ModuleVehicle::startHandbrakeTurnRightMode()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalSteerRight(true);
		gVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		gVehicleInputData.setAnalogSteer(1.0f);
		gVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void  ModuleVehicle::releaseAllControls()
{
	if (gMimicKeyInputs)
	{
		gVehicleInputData.setDigitalAccel(false);
		gVehicleInputData.setDigitalSteerLeft(false);
		gVehicleInputData.setDigitalSteerRight(false);
		gVehicleInputData.setDigitalBrake(false);
		gVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		gVehicleInputData.setAnalogAccel(0.0f);
		gVehicleInputData.setAnalogSteer(0.0f);
		gVehicleInputData.setAnalogBrake(0.0f);
		gVehicleInputData.setAnalogHandbrake(0.0f);
	}
}