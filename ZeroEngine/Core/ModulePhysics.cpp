#include "ModulePhysics.h"
#include "Application.h"
#include "PrimitivesGL.h"
#include "p2Defs.h"

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
	gravity = 0.0f;
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

	SceneSimulation();

	//if(dynamic != nullptr)
		//LOG("%f %f %f", dynamic->getGlobalPose().p.x, dynamic->getGlobalPose().p.y, dynamic->getGlobalPose().p.z);

	mScene->setGravity(PxVec3(0.0f, gravity, 0.0f));
	
	RenderGeometry();
	
	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::SceneSimulation(bool fetchResults) {

	mScene->simulate(1.0f / 60.0f);
	mScene->fetchResults(fetchResults);

}

bool ModulePhysics::CleanUp() {

	//TODO: Remember not just delete , pref RELEASE delete->nullptr later
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

//--------------COLLIDERS--------------//
void ModulePhysics::CreateCollider(GeometryType type, float3 pos, float3 size, float radius) {

	PxTransform position = PxTransform(pos.x, pos.y, pos.z);
	PxVec3 collider_size = { size.x, size.y, size.z };
	float collider_radius = radius;
	PxReal halfHeight = size.y / 2;

	switch (type)
	{
		case GeometryType::PLANE:
		{
			PxRigidStatic* plane_C = nullptr;

			plane_C = mPhysics->createRigidStatic(position);
			plane_C = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);

			mScene->addActor(*plane_C);

			LOG("Created Plane Collider");
		}
			break;
		case GeometryType::BOX:
		{
			PxRigidDynamic* box_C = nullptr;
		
			box_C = mPhysics->createRigidDynamic(position);
			box_C = PxCreateDynamic(*mPhysics, position, PxBoxGeometry(collider_size), *mMaterial, 1.0f);

			mScene->addActor(*box_C);		

			LOG("Created Box Collider");
		}
			break;
		case GeometryType::SPHERE:
		{
			PxRigidDynamic* sphere_C = nullptr;

			sphere_C = mPhysics->createRigidDynamic(position);
			sphere_C = PxCreateDynamic(*mPhysics, position, PxSphereGeometry(collider_radius), *mMaterial, 1.0f);

			mScene->addActor(*sphere_C);

			LOG("Created Sphere Collider");
		}
			break;
		case GeometryType::CAPSULE:
		{
			PxRigidDynamic* capsule_C = nullptr;

			capsule_C = mPhysics->createRigidDynamic(position);
			capsule_C = PxCreateDynamic(*mPhysics, position, PxCapsuleGeometry(collider_radius, halfHeight), *mMaterial, 1.0f);

			mScene->addActor(*capsule_C);

			LOG("Created Capsule Collider");
		}
			break;
		case GeometryType::NONE:
			break;

	}

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