#include "ModulePhysics.h"
#include "Application.h"

#include "PxPhysicsAPI.h"

#pragma comment(lib, "Core/physx/libx86/PhysXCommon_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysX_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXExtensions_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXFoundation_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXCharacterKinematic_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/SceneQuery_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXCooking_32.lib")

using namespace physx;

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled) {
	
	mFoundation = nullptr;
	mPhysics = nullptr;
	mPvd = nullptr;
	mCooking = nullptr;
	mMaterial = nullptr;
	mScene = nullptr;
	mDispatcher = nullptr;

}

ModulePhysics::~ModulePhysics() {

}

bool ModulePhysics::Init() {

	//Initialize PhysX mFoundation
	#pragma region Foundation_Initialize
	static PxDefaultErrorCallback gDefaultErrorCallback;
    static PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation) 
		LOG("PxCreateFoundation failed!")
	else 
		LOG("PxCreateFoundation Created: Succesfully inited PhysX");
	#pragma endregion Foundation_Initialize

	//Initialize physics
	#pragma region Physics_Initialize
	bool recordMemoryAllocations = true;

	mPvd = PxCreatePvd(*mFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("hello", 5425, 10);
	mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocations, mPvd);
	if (!mPhysics) 
		LOG("PxCreatePhysics failed!")
	else 
		LOG("PxCreatePhysics Sucessfull");

	#pragma endregion Physics_Initialize

	//Initialize Cooking
	#pragma region Cooking_Initialize
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
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

	//Initialize Scene
	#pragma region Scene_Initialize
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	mDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	mScene = mPhysics->createScene(sceneDesc);
	#pragma endregion Scene_Initialize

	//Initialize SceneClient
	#pragma region SceneClient_Initialize
	PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	#pragma endregion SceneClient_Initialize

	//Initialize Material
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
	mScene->addActor(*groundPlane);

	return true;
}

update_status ModulePhysics::Update(float dt) {

	mScene->simulate(1.0f/60.0f);
	mScene->fetchResults(true);

	return update_status::UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp() {

	//TODO: Remember not just delete , pref RELEASE delete->nullptr later
	mScene->release();
	mMaterial->release();
	mPhysics->release();
	mPvd->release();
	mCooking->release();
	PxCloseExtensions(); // Needed to close extensions we inited before
	mDispatcher->release();

	//Remember to release the last
	mFoundation->release();

	return true;
}

void ModulePhysics::CreateGeometry() {
	
	PxTransform pos = PxTransform(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
	PxRigidDynamic* dynamic = PxCreateDynamic(*mPhysics, pos , PxSphereGeometry(3.0f), *mMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(PxVec3(0));
	mScene->addActor(*dynamic); 
	LOG("CREATED BALL IN CAMERA");
}