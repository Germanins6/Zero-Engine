#include "ModulePhysics.h"

#include "PxPhysicsAPI.h"

#pragma comment(lib, "Core/physx/libx86/PhysXCommon_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysX_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXExtensions_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXFoundation_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXCharacterKinematic_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/SceneQuery_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXCooking_32.lib")

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled) {
	mFoundation = nullptr;
	mPhysics = nullptr;
	mPvd = nullptr;
	mCooking = nullptr;
}

ModulePhysics::~ModulePhysics() {

}



bool ModulePhysics::Init() {

	//Initialize PhysX mFoundation
	#pragma region Foundation_Initialize
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation) 
		LOG("PxCreateFoundation failed!")
	else 
		LOG("PxCreateFoundation Created: Succesfully inited PhysX");
	#pragma endregion Foundation_Initialize

	//Initialize physics
	#pragma region Physics_Initialize
	bool recordMemoryAllocations = true;

	mPvd = physx::PxCreatePvd(*mFoundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("hello", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, mPvd);
	if (!mPhysics) 
		LOG("PxCreatePhysics failed!")
	else 
		LOG("PxCreatePhysics Sucessfull");

	#pragma endregion Physics_Initialize

	//Initialize Cooking
	#pragma region Cooking_Initialize
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!mCooking)
		LOG("PxCreateCooking failed!")
	else
		LOG("PxCooking created Succesfully");
	#pragma endregion Cooking_Initialize

	//Initialize Extensions
	#pragma region Extensions_Initialize
	if (!PxInitExtensions(*mPhysics, mPvd))
		LOG("PxInitExtensions failed!")
	else
		LOG("PxInitextension Succesfull");
	#pragma endregion Extensions_Initialize

	return true;
}

bool ModulePhysics::CleanUp() {

	return true;
}