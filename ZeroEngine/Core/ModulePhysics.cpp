#include "ModulePhysics.h"
#include "Application.h"
#include "PrimitivesGL.h"
#include "p2Defs.h"


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
	static PxDefaultErrorCallback	mErrorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
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
		renderActors(&actors[0], static_cast<PxU32>(actors.size()), false);
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
		colliderShape = mPhysics->createShape(PxSphereGeometry(size.MaxElement()), *mMaterial, true);
		break; 
	case GeometryType::CAPSULE:
		colliderShape = mPhysics->createShape(PxCapsuleGeometry(size.z, size.y), *mMaterial, true);
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


void ModulePhysics::DrawCollider(ComponentCollider* collider)
{
	PxTransform new_transform = collider->rigidbody->rigid_dynamic->getGlobalPose() * collider->colliderShape->getLocalPose();
	float3 pos = { new_transform.p.x, new_transform.p.y, new_transform.p.z };
	Quat rot = { new_transform.q.x, new_transform.q.y, new_transform.q.z, new_transform.q.w};
	float4x4 transformation = float4x4(rot, pos);
	
	switch (GeometryType::BOX)
	{
	case GeometryType::BOX:
	{
		PxBoxGeometry boxCollider;
		collider->colliderShape->getBoxGeometry(boxCollider);
		float3 size = { boxCollider.halfExtents.x, boxCollider.halfExtents.y, boxCollider.halfExtents.z };
		DrawBoxCollider(transformation, size);
	}
		break;
	case GeometryType::SPHERE:
	{
		PxSphereGeometry sphereCollider;
		collider->colliderShape->getSphereGeometry(sphereCollider);
		DrawSphereCollider(transformation, sphereCollider.radius);
	}
		break;
	case GeometryType::CAPSULE:
	{
		PxCapsuleGeometry capsuleCollider;
		collider->colliderShape->getCapsuleGeometry(capsuleCollider);
		DrawCapsuleCollider(transformation, capsuleCollider.halfHeight, capsuleCollider.radius);
	}
	break;

	}
}

void ModulePhysics::DrawBoxCollider(const float4x4& transform, const float3& half_size) 
{
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, half_size.z);
	glVertex3f(-half_size.x, half_size.y, half_size.z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(-half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, half_size.z);
	glVertex3f(-half_size.x, -half_size.y, half_size.z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-half_size.x, half_size.y, -half_size.z);
	glVertex3f(-half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, -half_size.z);
	glVertex3f(half_size.x, -half_size.y, -half_size.z);
	glVertex3f(half_size.x, half_size.y, half_size.z);
	glVertex3f(half_size.x, -half_size.y, half_size.z);
	glVertex3f(-half_size.x, half_size.y, half_size.z);
	glVertex3f(-half_size.x, -half_size.y, half_size.z);
	glEnd();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void ModulePhysics::DrawSphereCollider(const float4x4& transform, float radius) {
	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	glColor3f(1.0f, 0.0f, 0.0f);

	float delta_amgle = 360.0f / 50.0f;  // Sphere sides
	float curr_angle = 0.f;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), 0.0f, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle), 0.0f);
	}
	glEnd();

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle), radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void ModulePhysics::DrawCapsuleCollider(const float4x4& transform, const float half_height, const float radius) {
	glPushMatrix();

	float4x4 final_trams = transform * Quat::RotateZ(DEGTORAD * -90);
	glMultMatrixf(final_trams.Transposed().ptr());
	glColor3f(1.0f, 0.0f, 0.0f);

	float delta_amgle = 360.0f / 50.0f;
	float half_delta_angle = 180.f / 25.0f;
	float curr_angle = 0.f;


	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), half_height, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = half_delta_angle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle) + half_height, 0.0f);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = half_delta_angle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle) + half_height, radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();


	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < (int)50.0f; ++i) {
		curr_angle = delta_amgle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), -half_height, radius * sinf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = 180.F + half_delta_angle * i;
		glVertex3f(radius * cosf(DEGTORAD * curr_angle), radius * sinf(DEGTORAD * curr_angle) - half_height, 0.0f);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= (int)25.0f; ++i) {
		curr_angle = 180.F + half_delta_angle * i;
		glVertex3f(0.0f, radius * sinf(DEGTORAD * curr_angle) - half_height, radius * cosf(DEGTORAD * curr_angle));
	}
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(0.f, half_height, -radius);
	glVertex3f(0.f, -half_height, -radius);
	glVertex3f(0.f, half_height, radius);
	glVertex3f(0.f, -half_height, radius);
	glVertex3f(-radius, half_height, 0.f);
	glVertex3f(-radius, -half_height, 0.f);
	glVertex3f(radius, half_height, 0.f);
	glVertex3f(radius, -half_height, 0.f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void ModulePhysics::WakeUpGeometry(GameObject* gameObject) {
	gameObject->GetRigidbody()->rigid_dynamic->wakeUp();
}