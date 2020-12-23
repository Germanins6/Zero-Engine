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
	mMaterial = nullptr;
	mScene = nullptr;
	mDispatcher = nullptr;

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

	//Initialize Material
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	//Initialize Scene
	#pragma region Scene_Initialize
	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	mScene = mPhysics->createScene(sceneDesc);
	#pragma endregion Scene_Initialize

	return true;
}

update_status ModulePhysics::Update(float dt) {

	mScene->simulate(1.0f / 60.0f);
	mScene->fetchResults(true);

	return update_status::UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp() {

	mPhysics->release();
	mPvd->release();
	mCooking->release();
	PxCloseExtensions(); // Needed to close extensions we inited before

	//Remember to release the last
	mFoundation->release();

	return true;
}

void ModulePhysics::CreateSphereCollider() {
	/*
	physx::PxRigidDynamic* aCapsuleActor = mPhysics->createRigidDynamic(physx::PxTransform({ 0,0,0 }));
	physx::PxTransform relativePose(physx::PxQuat(0.4f, { 0, 0, 1 }));
	physx::PxMaterial* capsuleMaterial = mPhysics->createMaterial(0.0f,0.0f,0.0f);
	physx::PxShape* aCapsuleShape = physx::PxRigidActorExt::createExclusiveShape(*aCapsuleActor, physx::PxCapsuleGeometry(2.0f, 1.0f), capsuleMaterial);
	physx::PxRigidActorExt::createExclusiveShape()
	aCapsuleShape->setLocalPose(relativePose);
	physx::PxRigidBodyExt::updateMassAndInertia(*aCapsuleActor, 2.0f);
	*/

	physx::PxShape* shape = mPhysics->createShape(physx::PxSphereGeometry(2.0f), *mMaterial);
	physx::PxRigidDynamic* body = mPhysics->createRigidDynamic({ 0.0f,0.0f,0.0f });
	body->attachShape(*shape);
	physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	mScene->addActor(*body);

	shape->release();
}