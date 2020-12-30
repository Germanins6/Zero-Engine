#include "ModulePhysics.h"
#include "Application.h"
#include "PrimitivesGL.h"
#include "p2Defs.h"

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleSDK.h"

#ifdef _DEBUG
#pragma comment(lib, "Core/physx/libx86/PhysXCommon_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysX_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXExtensions_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXFoundation_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXCharacterKinematic_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/SceneQuery_static_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXCooking_32.lib")
#pragma comment(lib, "Core/physx/libx86/PhysXVehicle_static_32.lib")
#endif //_DEBUG

using namespace physx;

#define MAX_NUM_MESH_VEC3S  1024
#define MAX_NUM_ACTOR_SHAPES 128

static PxVec3 gVertexBuffer[MAX_NUM_MESH_VEC3S];

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled) {
	
	mFoundation = nullptr;
	mPhysics = nullptr;
	mPvd = nullptr;
	mCooking = nullptr;
	mMaterial = nullptr;
	mScene = nullptr;
	mDispatcher = nullptr;

	//dynamic = nullptr;
	gravity = -9.81f;
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

	//PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
	//mScene->addActor(*groundPlane);

	//Init vehicle after foundation and physics
	PxInitVehicleSDK(*mPhysics);

	return true;
}

update_status ModulePhysics::Update(float gameTimestep) {


	if (App->timeManager->started)
		SceneSimulation(gameTimestep);

	//TODO: REMOVE OR REPLACE
	mScene->setGravity(PxVec3(0.0f, gravity, 0.0f));
	
	RenderGeometry();
	
	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::SceneSimulation(float gameTimestep, bool fetchResults) {

	mScene->simulate(gameTimestep);
	mScene->fetchResults(fetchResults);

}

bool ModulePhysics::CleanUp() {

	PxCloseVehicleSDK(); //->Close vehicle sdk before close physics and foundation

	PX_RELEASE(mScene);
	PX_RELEASE(mMaterial);
	PX_RELEASE(mPhysics);

	if (mPvd) 
	{
		PxPvdTransport* transport = mPvd->getTransport();
		mPvd->release(); mPvd = NULL;
		PX_RELEASE(transport);
	}

	PX_RELEASE(mCooking);
	PxCloseExtensions(); // Needed to close extensions we inited before
	PX_RELEASE(mDispatcher);

	//Remember to release the last
	PX_RELEASE(mFoundation);

	return true;
}

void ModulePhysics::RenderGeometry() {

	PxGetPhysics().getScenes(&mScene, 1);
	PxU32 nbActors = mScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if (nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		mScene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
		renderActors(&actors[0], static_cast<PxU32>(actors.size()), true);
	}

}

void ModulePhysics::renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows)
{
	const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
	const PxReal shadowMat[] = { 1,0,0,0, -shadowDir.x / shadowDir.y,0,-shadowDir.z / shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
	const PxVec3 color = PxVec3(0.0f, 0.75f, 0.0f);

	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	for (PxU32 i = 0; i < numActors; i++)
	{
		const PxU32 nbShapes = actors[i]->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actors[i]->getShapes(shapes, nbShapes);
		const bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

		for (PxU32 j = 0; j < nbShapes; j++)
		{
			const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
			const PxGeometryHolder h = shapes[j]->getGeometry();

			if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// render object
			glPushMatrix();
			glMultMatrixf(&shapePose.column0.x);
			if (sleeping)
			{
				const PxVec3 darkColor = color * 0.25f;
				glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
			}
			else
				glColor4f(color.x, color.y, color.z, 1.0f);
			renderGeometryHolder(h);
			glPopMatrix();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (shadows)
			{
				glPushMatrix();
				glMultMatrixf(shadowMat);
				glMultMatrixf(&shapePose.column0.x);
				glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
				renderGeometryHolder(h);
				glPopMatrix();
			}
		}
	}
}

void ModulePhysics::renderGeometryHolder(const PxGeometryHolder& h) { 
	renderGeometry(h.any()); 
};

void ModulePhysics::renderGeometry(const PxGeometry& geom)
{

	switch (geom.getType())
	{
		case PxGeometryType::eBOX:
		{
			const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
			DrawGeometry(GeometryType::BOX, { NULL, NULL, NULL }, NULL, { boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z });
		}
		break;

		case PxGeometryType::eSPHERE:
		{
			const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
			const PxF32 radius = sphereGeom.radius;

			DrawGeometry(GeometryType::SPHERE, { NULL, NULL, NULL }, radius);

		}
		break;

		case PxGeometryType::eCAPSULE:
		{
			const PxCapsuleGeometry& capsuleGeom = static_cast<const PxCapsuleGeometry&>(geom);
			const PxF32 radius = capsuleGeom.radius;
			const PxF32 halfHeight = capsuleGeom.halfHeight;

			//Sphere
			DrawGeometry(GeometryType::SPHERE, { NULL, NULL, NULL }, radius);

			//Sphere
			DrawGeometry(GeometryType::SPHERE, { NULL, NULL, NULL }, radius);

			//Cylinder
			DrawGeometry(GeometryType::CAPSULE, { 0.0f, halfHeight, 0.0f }, radius);

		}
		break;

		case PxGeometryType::ePLANE:
			break;
	}

}

//--------------CREATE A NEW GEOMETRY--------------//
physx::PxRigidDynamic* ModulePhysics::CreateGeometry(GeometryType type, float3 pos, float mass, float radius, float3 size){

	physx::PxRigidDynamic* geometry = nullptr;
	PxTransform position = PxTransform(pos.x, pos.y, pos.z);

	switch (type)
	{
	case GeometryType::BOX:
	{
		PxShape* shape = mPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *mMaterial);

		geometry = mPhysics->createRigidDynamic(position);
		geometry->attachShape(*shape);

		geometry->setAngularDamping(0.5f);
		geometry->setLinearVelocity(PxVec3(0));

		LOG("CREATED BOX");
	}
		break;
	case GeometryType::SPHERE:
	{
		PxShape* shape = mPhysics->createShape(PxSphereGeometry(radius), *mMaterial);

		geometry = PxCreateDynamic(*mPhysics, position, PxSphereGeometry(radius), *mMaterial, mass);
		geometry->attachShape(*shape);

		geometry->setAngularDamping(0.05f);

		LOG("CREATED SPHERE");
	
	}
		break;
	case GeometryType::CAPSULE:
	{
		PxReal halfHeight = size.y / 2;
		
		geometry = mPhysics->createRigidDynamic(PxTransform(position));
		
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*geometry, PxCapsuleGeometry(radius, halfHeight), *mMaterial);
		geometry->attachShape(*shape);

		geometry->setAngularDamping(0.05f);

		LOG("CREATED CAPSULE");
	}
		break;
	case GeometryType::NONE:
		break;
	}
	
	geometry->setMass(mass);
	mScene->addActor(*geometry);

	return geometry;

}

//--------------DRAW THE GEOMETRY--------------//
void ModulePhysics::DrawGeometry(GeometryType type, float3 pos, float radius, float3 size){

	switch (type)
	{
	case GeometryType::BOX:
	{
		glBegin(GL_QUADS);
		glPushMatrix();

		glNormal3f(-1.0f, 0.0f, 0.0f); //FRONT
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);

		glNormal3f(0.0f, 0.0f, -1.0f); //BOTTOM
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glNormal3f(1.0f, 0.0f, 0.0f); //BACK
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);

		glNormal3f(0.0f, 0.0f, 1.0f); //TOP
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 1.0f, 1.0f);

		glNormal3f(0.0f, 1.0f, 0.0f); //LEFT
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 1.0f);

		glNormal3f(0.0f, -1.0f, 0.0f); //RIGHT
		glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glScalef(size.x, size.y, size.z);
		glTranslatef(pos.x, pos.y, pos.z);

		glPopMatrix();
		glEnd();

	}
		break;
	case GeometryType::SPHERE:
	{
		GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        
		int gradation = 10;

		for (alpha = 0.0; alpha < PI; alpha += PI / gradation)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glPushMatrix();

			for (beta = 0.0; beta < 2.01 * PI; beta += PI / gradation)
			{
				x = cos(beta) * sin(alpha);
				y = sin(beta) * sin(alpha);
				z = cos(alpha);
				glVertex3f(x, y, z);
				x = cos(beta) * sin(alpha + PI / gradation);
				y = sin(beta) * sin(alpha + PI / gradation);
				z = cos(alpha + PI / gradation);
				glVertex3f(x, y, z);
			}

			glTranslatef(pos.x, 0.0f, 0.0f);
			glScalef(radius, radius, radius);

			glPopMatrix();
			glEnd();
		}
	}
		break;
	case GeometryType::CAPSULE:
	{
		glBegin(GL_QUAD_STRIP);
		glPushMatrix();
		glTranslatef(-pos.x, 0.0f, 0.0f);
		//glScalef(2.0f * pos.x, radius, radius);

		for (int i = 0; i < 480; i += (360 / 16)) {
			float a = i * M_PI / 180; // degrees to radians
			glVertex3f(2 * cos(a), 2 * sin(a), 0.0);
			glVertex3f(2 * cos(a), 2 * sin(a), 4);
		}
		glEnd();

		glPopMatrix();
		glEnd();
	}
		break;
	case GeometryType::NONE:
		break;
	}

}


physx::PxRigidDynamic* ModulePhysics::CreateRigidbody(float3 pos) {

	PxTransform position = PxTransform(pos.x, pos.y, pos.z);
	/*
	PxVec3 collider_size = { size.x, size.y, size.z };
	float collider_radius = radius;
	PxReal halfHeight = size.y / 2;*/

	PxRigidDynamic* rigidbody = nullptr;
	rigidbody = mPhysics->createRigidDynamic(position);

	/*
	switch (type)
	{
		case GeometryType::BOX:
			rigidbody = PxCreateDynamic(*mPhysics, position, PxBoxGeometry(collider_size), *mMaterial, 1.0f);
			break;

		case GeometryType::SPHERE:
			rigidbody = PxCreateDynamic(*mPhysics, position, PxSphereGeometry(collider_radius), *mMaterial, 1.0f);
			break;

		case GeometryType::CAPSULE:
			rigidbody = PxCreateDynamic(*mPhysics, position, PxCapsuleGeometry(collider_radius, halfHeight), *mMaterial, 1.0f);
			break;
	}
	*/

	mScene->addActor(*rigidbody);
	return rigidbody;
}

physx::PxShape* ModulePhysics::CreateCollider(GeometryType colliderType, float3 size) {
	
	PxShape* colliderShape = nullptr;

	switch (colliderType) {
	case GeometryType::BOX:
		colliderShape = mPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *mMaterial, true);
		break;
	case GeometryType::SPHERE:
		colliderShape = mPhysics->createShape(PxSphereGeometry(), *mMaterial, true);
		break; 
	case GeometryType::CAPSULE:
		colliderShape = mPhysics->createShape(PxCapsuleGeometry(), *mMaterial, true);
		break;
	}

	return colliderShape;
}

physx::PxJoint* ModulePhysics::CreateJoint(JointType jointType) {

	PxJoint* joint = nullptr;

	switch (jointType)
	{
	case JointType::FIXED:
		break;
	case JointType::DISTANCE:
		//joint = PxDistanceJointCreate(*mPhysics);
		break;
	case JointType::SPHERICAL:
		break;
	case JointType::REVOLUTE:
		break;
	case JointType::PRISMATIC:
		break;
	case JointType::D6:
		break;
	}


	return joint;
}


void ModulePhysics::DrawCollider(GeometryType type) {

	switch (type)
	{
	case GeometryType::BOX:
	{
		glBegin(GL_LINES);
		glLineWidth(3.0f);
		glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

		/*for (uint i = 0; i < bbox.NumEdges(); i++)
		{
			glVertex3f(bbox.Edge(i).a.x, bbox.Edge(i).a.y, bbox.Edge(i).a.z);
			glVertex3f(bbox.Edge(i).b.x, bbox.Edge(i).b.y, bbox.Edge(i).b.z);
		}*/
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	}
		break;
	case GeometryType::SPHERE:
		break;
	case GeometryType::CAPSULE:
		break;
	case GeometryType::NONE:
		break;
	}
		
}

/*
void ModulePhysics::createVehicle4WSimulationData
(const PxF32 chassisMass, PxConvexMesh* chassisConvexMesh,
	const PxF32 wheelMass, PxConvexMesh** wheelConvexMeshes, const PxVec3* wheelCentreOffsets,
	PxVehicleWheelsSimData& wheelsData, PxVehicleDriveSimData4W& driveData, PxVehicleChassisData& chassisData)
{
	//Extract the chassis AABB dimensions from the chassis convex mesh.
	//-->const PxVec3 chassisDims = computeChassisAABBDimensions(chassisConvexMesh);

	//The origin is at the center of the chassis mesh.
	//Set the center of mass to be below this point and a little towards the front.
	const PxVec3 chassisCMOffset = PxVec3(0.0f, -chassisDims.y * 0.5f + 0.65f, 0.25f);

	//Now compute the chassis mass and moment of inertia.
	//Use the moment of inertia of a cuboid as an approximate value for the chassis moi.
	PxVec3 chassisMOI
	((chassisDims.y * chassisDims.y + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.z * chassisDims.z) * chassisMass / 12.0f,
		(chassisDims.x * chassisDims.x + chassisDims.y * chassisDims.y) * chassisMass / 12.0f);
	//A bit of tweaking here.  The car will have more responsive turning if we reduce the
	//y-component of the chassis moment of inertia.
	chassisMOI.y *= 0.8f;

	//Let's set up the chassis data structure now.
	chassisData.mMass = chassisMass;
	chassisData.mMOI = chassisMOI;
	chassisData.mCMOffset = chassisCMOffset;

	//Work out the front/rear mass split from the cm offset.
	//This is a very approximate calculation with lots of assumptions.
	//massRear*zRear + massFront*zFront = mass*cm           (1)
	//massRear       + massFront        = mass                      (2)
	//Rearrange (2)
	//massFront = mass - massRear                                           (3)
	//Substitute (3) into (1)
	//massRear(zRear - zFront) + mass*zFront = mass*cm      (4)
	//Solve (4) for massRear
	//massRear = mass(cm - zFront)/(zRear-zFront)           (5)
	//Now we also have
	//zFront = (z-cm)/2                                                                     (6a)
	//zRear = (-z-cm)/2                                                                     (6b)
	//Substituting (6a-b) into (5) gives
	//massRear = 0.5*mass*(z-3cm)/z                                         (7)
	const PxF32 massRear = 0.5f * chassisMass * (chassisDims.z - 3 * chassisCMOffset.z) / chassisDims.z;
	const PxF32 massFront = chassisMass - massRear;

	//Extract the wheel radius and width from the wheel convex meshes.
	PxF32 wheelWidths[4];
	PxF32 wheelRadii[4];
	computeWheelWidthsAndRadii(wheelConvexMeshes, wheelWidths, wheelRadii);

	//Now compute the wheel masses and inertias components around the axle's axis.
	//http://en.wikipedia.org/wiki/List_of_moments_of_inertia
	PxF32 wheelMOIs[4];
	for (PxU32 i = 0; i < 4; i++)
	{
		wheelMOIs[i] = 0.5f * wheelMass * wheelRadii[i] * wheelRadii[i];
	}
	//Let's set up the wheel data structures now with radius, mass, and moi.
	PxVehicleWheelData wheels[4];
	for (PxU32 i = 0; i < 4; i++)
	{
		wheels[i].mRadius = wheelRadii[i];
		wheels[i].mMass = wheelMass;
		wheels[i].mMOI = wheelMOIs[i];
		wheels[i].mWidth = wheelWidths[i];
	}
	//Disable the handbrake from the front wheels and enable for the rear wheels
	wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxHandBrakeTorque = 0.0f;
	wheels[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mMaxHandBrakeTorque = 0.0f;
	wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxHandBrakeTorque = 4000.0f;
	wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxHandBrakeTorque = 4000.0f;
	//Enable steering for the front wheels and disable for the front wheels.
	wheels[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mMaxSteer = PxPi * 0.3333f;
	wheels[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mMaxSteer = PxPi * 0.3333f;
	wheels[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mMaxSteer = 0.0f;
	wheels[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mMaxSteer = 0.0f;

	//Let's set up the tire data structures now.
	//Put slicks on the front tires and wets on the rear tires.
	PxVehicleTireData tires[4];
	tires[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mType = TIRE_TYPE_SLICKS;
	tires[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mType = TIRE_TYPE_SLICKS;
	tires[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mType = TIRE_TYPE_WETS;
	tires[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mType = TIRE_TYPE_WETS;

	//Let's set up the suspension data structures now.
	PxVehicleSuspensionData susps[4];
	for (PxU32 i = 0; i < 4; i++)
	{
		susps[i].mMaxCompression = 0.3f;
		susps[i].mMaxDroop = 0.1f;
		susps[i].mSpringStrength = 35000.0f;
		susps[i].mSpringDamperRate = 4500.0f;
	}
	susps[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].mSprungMass = massFront * 0.5f;
	susps[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].mSprungMass = massFront * 0.5f;
	susps[PxVehicleDrive4W::eREAR_LEFT_WHEEL].mSprungMass = massRear * 0.5f;
	susps[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].mSprungMass = massRear * 0.5f;

	//We need to set up geometry data for the suspension, wheels, and tires.
	//We already know the wheel centers described as offsets from the rigid body centre of mass.
	//From here we can approximate application points for the tire and suspension forces.
	//Lets assume that the suspension travel directions are absolutely vertical.
	//Also assume that we apply the tire and suspension forces 30cm below the centre of mass.
	PxVec3 suspTravelDirections[4] = { PxVec3(0,-1,0),PxVec3(0,-1,0),PxVec3(0,-1,0),PxVec3(0,-1,0) };
	PxVec3 wheelCentreCMOffsets[4];
	PxVec3 suspForceAppCMOffsets[4];
	PxVec3 tireForceAppCMOffsets[4];
	for (PxU32 i = 0; i < 4; i++)
	{
		wheelCentreCMOffsets[i] = wheelCentreOffsets[i] - chassisCMOffset;
		suspForceAppCMOffsets[i] = PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
		tireForceAppCMOffsets[i] = PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
	}

	//Now add the wheel, tire and suspension data.
	for (PxU32 i = 0; i < 4; i++)
	{
		wheelsData.setWheelData(i, wheels[i]);
		wheelsData.setTireData(i, tires[i]);
		wheelsData.setSuspensionData(i, susps[i]);
		wheelsData.setSuspTravelDirection(i, suspTravelDirections[i]);
		wheelsData.setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsData.setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
		wheelsData.setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
	}

	//Now set up the differential, engine, gears, clutch, and ackermann steering.

	//Diff
	PxVehicleDifferential4WData diff;
	diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	driveData.setDiffData(diff);

	//Engine
	PxVehicleEngineData engine;
	engine.mPeakTorque = 500.0f;
	engine.mMaxOmega = 600.0f;//approx 6000 rpm
	driveData.setEngineData(engine);

	//Gears
	PxVehicleGearsData gears;
	gears.mSwitchTime = 0.5f;
	driveData.setGearsData(gears);

	//Clutch
	PxVehicleClutchData clutch;
	clutch.mStrength = 10.0f;
	driveData.setClutchData(clutch);

	//Ackermann steer accuracy
	PxVehicleAckermannGeometryData ackermann;
	ackermann.mAccuracy = 1.0f;
	ackermann.mAxleSeparation = wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].z - wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].z;
	ackermann.mFrontWidth = wheelCentreOffsets[PxVehicleDrive4W::eFRONT_RIGHT_WHEEL].x - wheelCentreOffsets[PxVehicleDrive4W::eFRONT_LEFT_WHEEL].x;
	ackermann.mRearWidth = wheelCentreOffsets[PxVehicleDrive4W::eREAR_RIGHT_WHEEL].x - wheelCentreOffsets[PxVehicleDrive4W::eREAR_LEFT_WHEEL].x;
	driveData.setAckermannGeometryData(ackermann);
}
*/