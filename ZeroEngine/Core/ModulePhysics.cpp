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
		renderActors(&actors[0], static_cast<PxU32>(actors.size()), false);
	}

}

void ModulePhysics::renderGeometry(const PxGeometry& geom)
{
	int gradation = 5;
	GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        

	switch (geom.getType())
	{
	case PxGeometryType::eBOX:
	{
		const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
		DrawBox({ boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z });

	}
	break;

	case PxGeometryType::eSPHERE:
	{
		const PxSphereGeometry& sphereGeom = static_cast<const PxSphereGeometry&>(geom);
		const PxF32 radius = sphereGeom.radius;

		DrawSphere(radius);

	}
	break;

	case PxGeometryType::eCAPSULE:
	{
		const PxCapsuleGeometry& capsuleGeom = static_cast<const PxCapsuleGeometry&>(geom);
		const PxF32 radius = capsuleGeom.radius;
		const PxF32 halfHeight = capsuleGeom.halfHeight;

		//Sphere
		DrawSphere(radius, { 0.0f, halfHeight, 0.0f });

		//Sphere
		DrawSphere(radius, { 0.0f, -halfHeight, 0.0f });

		//Cylinder
		DrawCylinder(radius, { 0.0f, halfHeight, 0.0f });

	}
	break;

	case PxGeometryType::eCONVEXMESH:
	{
		const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);

		//Compute triangles for each polygon.
		const PxVec3& scale = convexGeom.scale.scale;
		PxConvexMesh* mesh = convexGeom.convexMesh;
		const PxU32 nbPolys = mesh->getNbPolygons();
		const PxU8* polygons = mesh->getIndexBuffer();
		const PxVec3* verts = mesh->getVertices();
		PxU32 nbVerts = mesh->getNbVertices();
		PX_UNUSED(nbVerts);

		PxU32 numTotalTriangles = 0;
		for (PxU32 i = 0; i < nbPolys; i++)
		{
			PxHullPolygon data;
			mesh->getPolygonData(i, data);

			const PxU32 nbTris = PxU32(data.mNbVerts - 2);
			const PxU8 vref0 = polygons[data.mIndexBase + 0];
			PX_ASSERT(vref0 < nbVerts);
			for (PxU32 j = 0; j < nbTris; j++)
			{
				const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
				const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

				//generate face normal:
				PxVec3 e0 = verts[vref1] - verts[vref0];
				PxVec3 e1 = verts[vref2] - verts[vref0];

				PX_ASSERT(vref1 < nbVerts);
				PX_ASSERT(vref2 < nbVerts);

				PxVec3 fnormal = e0.cross(e1);
				fnormal.normalize();

				if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
				{
					gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
					gVertexBuffer[numTotalTriangles * 6 + 1] = verts[vref0];
					gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
					gVertexBuffer[numTotalTriangles * 6 + 3] = verts[vref1];
					gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
					gVertexBuffer[numTotalTriangles * 6 + 5] = verts[vref2];
					numTotalTriangles++;
				}
			}
		}
		glPushMatrix();
		glScalef(scale.x, scale.y, scale.z);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
		glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
		glPopMatrix();
	}
	break;

	case PxGeometryType::eTRIANGLEMESH:
	{
		const PxTriangleMeshGeometry& triGeom = static_cast<const PxTriangleMeshGeometry&>(geom);

		const PxTriangleMesh& mesh = *triGeom.triangleMesh;
		const PxVec3 scale = triGeom.scale.scale;

		const PxU32 triangleCount = mesh.getNbTriangles();
		const PxU32 has16BitIndices = mesh.getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;
		const void* indexBuffer = mesh.getTriangles();

		const PxVec3* vertexBuffer = mesh.getVertices();

		const PxU32* intIndices = reinterpret_cast<const PxU32*>(indexBuffer);
		const PxU16* shortIndices = reinterpret_cast<const PxU16*>(indexBuffer);
		PxU32 numTotalTriangles = 0;
		for (PxU32 i = 0; i < triangleCount; ++i)
		{
			PxVec3 triVert[3];

			if (has16BitIndices)
			{
				triVert[0] = vertexBuffer[*shortIndices++];
				triVert[1] = vertexBuffer[*shortIndices++];
				triVert[2] = vertexBuffer[*shortIndices++];
			}
			else
			{
				triVert[0] = vertexBuffer[*intIndices++];
				triVert[1] = vertexBuffer[*intIndices++];
				triVert[2] = vertexBuffer[*intIndices++];
			}

			PxVec3 fnormal = (triVert[1] - triVert[0]).cross(triVert[2] - triVert[0]);
			fnormal.normalize();

			if (numTotalTriangles * 6 < MAX_NUM_MESH_VEC3S)
			{
				gVertexBuffer[numTotalTriangles * 6 + 0] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 1] = triVert[0];
				gVertexBuffer[numTotalTriangles * 6 + 2] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 3] = triVert[1];
				gVertexBuffer[numTotalTriangles * 6 + 4] = fnormal;
				gVertexBuffer[numTotalTriangles * 6 + 5] = triVert[2];
				numTotalTriangles++;
			}
		}
		glPushMatrix();
		glScalef(scale.x, scale.y, scale.z);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer);
		glVertexPointer(3, GL_FLOAT, 2 * 3 * sizeof(float), gVertexBuffer + 1);
		glDrawArrays(GL_TRIANGLES, 0, int(numTotalTriangles * 3));
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glPopMatrix();
	}
	break;

	case PxGeometryType::eINVALID:
	case PxGeometryType::eHEIGHTFIELD:
	case PxGeometryType::eGEOMETRY_COUNT:
	case PxGeometryType::ePLANE:
		break;
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

//-----------------BOX-----------------//
physx::PxRigidDynamic* ModulePhysics::CreateBox(float3 pos, float3 size, float mass) {
	
	physx::PxRigidDynamic* box = nullptr;
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *mMaterial);

	box = mPhysics->createRigidDynamic({ pos.x, pos.y, pos.z });
	box->attachShape(*shape);
	box->setAngularDamping(0.5f);
	box->setLinearVelocity(PxVec3(0));
	PxRigidBodyExt::updateMassAndInertia(*box, mass);

	mScene->addActor(*box);

	LOG("CREATED BOX");

	return box;
}

void ModulePhysics::DrawBox(float3 size) {

	glBegin(GL_QUADS);

	glScalef(size.x, size.y, size.z);

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

	glEnd();

}

//-----------------SPHERE-----------------//
physx::PxRigidDynamic* ModulePhysics::CreateSphere(float3 pos, float radius, float mass) {

	physx::PxRigidDynamic* sphere = nullptr;
	PxTransform position = PxTransform(pos.x, pos.y, pos.z);

	sphere = PxCreateDynamic(*mPhysics, position, PxSphereGeometry(radius), *mMaterial, mass);
	sphere->setAngularDamping(0.05f);
	sphere->setMass(mass);

	mScene->addActor(*sphere);

	LOG("CREATED SPHERE");
	return sphere;
}

void ModulePhysics::DrawSphere(float radius, float3 pos) {

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

//-----------------CAPSULE-----------------//
physx::PxRigidDynamic* ModulePhysics::CreateCapsule(float3 pos, float radius, float3 size, float mass) {

	PxReal halfHeight = size.y / 2;
	
	PxRigidDynamic* capsule = nullptr;
	capsule = mPhysics->createRigidDynamic(PxTransform({ pos.x, pos.y, pos.z }));
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*capsule, PxCapsuleGeometry(radius, halfHeight), *mMaterial);

	//PxTransform relativePose(PxQuat(1.57f, PxVec3(0, 0, 1)));
	capsule->attachShape(*shape);

	//shape->setLocalPose(relativePose);
	capsule->setAngularDamping(0.05f);
	capsule->setMass(mass);

	mScene->addActor(*capsule);

	LOG("CREATED CAPSULE");

	return capsule;
}

void ModulePhysics::DrawCylinder(float radius, float3 pos) {

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

