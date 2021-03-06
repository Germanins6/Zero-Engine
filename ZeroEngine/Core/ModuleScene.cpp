#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "glew/include/glew.h"
#include "PrimitivesGL.h"
#include "ImGui/imgui.h"
#include "Textures.h"
#include "Serialization.h"


#include "Serialization.h"
#include "ZeroImporter.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Scene";
	draw = true;
}

ModuleScene::~ModuleScene()
{
	CleanUp();
}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	ModelImporter::Load("Assets/Scenes/StreetEnviroment.ZeroScene");

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{

	//Cleaning each gameObject parent , calling each gameObject destructor will clean also components.
	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		delete gameobjects[i];
		gameobjects[i] = nullptr;
	}

	gameobjects.clear();

	return true;
}

// Update: draw background
update_status ModuleScene::Update(float dt)
{
	if (draw) {

		//Scene Grid
		App->primitivesGL->AxisGL();

		for (size_t i = 0; i < spheres.size(); i++)
			App->renderer3D->RenderThrowSpheres(spheres[i]);

		if(App->timeManager->started){
			
			//Create spheres to shot if F1 pressed
			if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
				PxRigidBody* sphere = App->physX->CreateGeometry(GeometryType::SPHERE, App->camera->editor_camera_transform->position);
				PxVec3 velocity = { App->camera->editor_camera_info->frustum.front.x, App->camera->editor_camera_info->frustum.front.y, App->camera->editor_camera_info->frustum.front.z };
				sphere->setLinearVelocity(velocity * 50.0f);
				spheres.push_back(sphere);
			}
		}
			

		if (App->vehicle->gVehicle4W != nullptr) {

			//-- Vehicle Controls --//
			if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT) //Forward
				App->vehicle->incrementDrivingMode(dt, DriveMode::eDRIVE_MODE_ACCEL_FORWARDS);

			if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT) //Right
				App->vehicle->incrementDrivingMode(dt, DriveMode::eDRIVE_MODE_HARD_TURN_RIGHT);

			if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) //Backwards
				App->vehicle->incrementDrivingMode(dt, DriveMode::eDRIVE_MODE_ACCEL_REVERSE);

			if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT) //Left
				App->vehicle->incrementDrivingMode(dt, DriveMode::eDRIVE_MODE_HARD_TURN_LEFT);

			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) //Break
				App->vehicle->incrementDrivingMode(dt, DriveMode::eDRIVE_MODE_BRAKE);

		}

		//If car is not moving release Controls
		if (App->input->GetKey(SDL_SCANCODE_I) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_J) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_K) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_L) != KEY_REPEAT)
			App->vehicle->releaseAllControls();

		//Accesing all gameobjects in scene, then accesing each gameobject component and calling their updates
		for (size_t i = 0; i < gameobjects.size(); i++) {

			//Just will check components and update if gameObject enabled
			if (gameobjects[i]->active) {
				for (size_t j = 0; j < gameobjects[i]->components.size(); j++) {
					if (gameobjects[i]->components[j] != nullptr)
						gameobjects[i]->components[j]->Update(dt);
				}
			}

			if(gameobjects[i]->GetMesh()!=nullptr)
				gameobjects[i]->Update(dt);
		}
	}
	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent) {

	GameObject* temp = new GameObject();
	gameobjects.push_back(temp);
	return temp;
}

void ModuleScene::SaveScene() const {

	Serializer scene;

	if (gameobjects.size() == 0) {
		scene.AddString("-Scene Name", name);
		scene.AddUnsignedInt("-Num_Children", 0);
	}

	for (size_t i = 0; i < gameobjects.size(); i++)
	{
		scene.AddString("-Scene Name", name);
		scene.AddUnsignedInt("-Num_Children", gameobjects.size() - 1);

		scene.Object[to_string(i)];
		scene.AddStringObj("Name", gameobjects[i]->name, to_string(i));
		scene.AddUnsignedIntObj("ID", gameobjects[i]->uuid, to_string(i));
		scene.AddUnsignedIntObj("IDParent", gameobjects[i]->parentId, to_string(i));
		
		//Transform
		ComponentTransform* transform = gameobjects[i]->GetTransform();
		scene.AddFloat3Obj("Translation", transform->position, to_string(i));
		scene.AddQuaternionObj("Rotation", transform->rotation, to_string(i));
		scene.AddFloat3Obj("Scale", transform->scale, to_string(i));

		//Meshes
		if (gameobjects[i]->GetMesh() != nullptr) {
			UID meshUID =gameobjects[i]->GetMesh()->ourMesh->GetUID();
			scene.AddUnsignedIntObj("MeshUID", meshUID, to_string(i));
		}
		else {
			scene.AddUnsignedIntObj("MeshUID", 0, to_string(i));
		}

		//Materials
		if (gameobjects[i]->GetMaterial() != nullptr) {
			UID materialUID = gameobjects[i]->GetMaterial()->GetMaterial()->GetUID();
			scene.AddUnsignedIntObj("MaterialUID", materialUID, to_string(i));
		}
		else {
			scene.AddUnsignedIntObj("MaterialUID", 0, to_string(i));
		}

		//Cameras
		if (gameobjects[i]->GetCamera() != nullptr) {
			ComponentCamera* camera = gameobjects[i]->GetCamera();
			scene.AddBoolObj("HasCamera", true, to_string(i));
			scene.AddFloatObj("Near Distance", camera->GetNearDistance(), to_string(i));
			scene.AddFloatObj("Far Distance", camera->GetFarDistance(), to_string(i));
			scene.AddFloatObj("Field of View", camera->GetFOV(), to_string(i));
		}
		else {
			scene.AddBoolObj("HasCamera", false, to_string(i));
		}

		//RigidBody
		if (gameobjects[i]->GetRigidbody() != nullptr) {
			ComponentRigidDynamic* rigidbody = gameobjects[i]->GetRigidbody();
			scene.AddBoolObj("HasRigidbody", true, to_string(i));
			scene.AddBoolObj("EnableGravity", rigidbody->use_gravity, to_string(i));
			scene.AddBoolObj("isKinematic", rigidbody->use_kinematic, to_string(i));
			scene.AddFloatObj("Mass", rigidbody->GetMass(), to_string(i));
			scene.AddFloatObj("Density", rigidbody->GetDensity(), to_string(i));
			scene.AddFloatObj("Linear Damping", rigidbody->GetLinearDamping(), to_string(i));
			scene.AddFloatObj("Angular Damping", rigidbody->GetAngularDamping(), to_string(i));
			scene.AddFloat3Obj("Force", rigidbody->GetForce(), to_string(i));
			scene.AddFloat3Obj("Torque", rigidbody->GetTorque(), to_string(i));
			float3 linearVel = { rigidbody->GetLinearVelocity().x,rigidbody->GetLinearVelocity().y,rigidbody->GetLinearVelocity().z };
			scene.AddFloat3Obj("Linear Velocity", linearVel , to_string(i));
			float3 angularVel = { rigidbody->GetAngularVeloctity().x,rigidbody->GetAngularVeloctity().y,rigidbody->GetAngularVeloctity().z };
			scene.AddFloat3Obj("Angular Velocity", angularVel, to_string(i));
			scene.AddBoolObj("LockLinearX", rigidbody->lock_linearX, to_string(i));
			scene.AddBoolObj("LockLinearY", rigidbody->lock_linearY, to_string(i));
			scene.AddBoolObj("LockLinearZ", rigidbody->lock_linearZ, to_string(i));
			scene.AddBoolObj("LockAngularX", rigidbody->lock_angularX, to_string(i));
			scene.AddBoolObj("LockAngularY", rigidbody->lock_angularY, to_string(i));
			scene.AddBoolObj("LockAngularZ", rigidbody->lock_angularZ, to_string(i));
		}
		else {
			scene.AddBoolObj("HasRigidbody", false, to_string(i));
		}

		//Collider
		if (gameobjects[i]->GetCollider() != nullptr) {
			ComponentCollider* collider = gameobjects[i]->GetCollider();
			scene.AddBoolObj("HasCollider", true, to_string(i));
			scene.AddIntObj("ColliderType", (int)collider->type, to_string(i));

			scene.AddBoolObj("isTrigger", collider->isTrigger, to_string(i));
			
			scene.AddFloat3Obj("Center", collider->GetPosition(), to_string(i));
			scene.AddFloat3Obj("Euler", collider->GetEuler(), to_string(i));
			scene.AddFloat3Obj("Size", collider->GetScale(), to_string(i));

			if (collider->colliderMaterial != nullptr) {
				scene.AddBoolObj("HasMaterial", true, to_string(i));
				scene.AddFloatObj("StaticFriction", collider->colliderMaterial->getStaticFriction(), to_string(i));
				scene.AddFloatObj("DynamicFriction", collider->colliderMaterial->getDynamicFriction(), to_string(i));
				scene.AddFloatObj("Restitution", collider->colliderMaterial->getRestitution(), to_string(i));
			}
			else {
				scene.AddBoolObj("HasMaterial", false, to_string(i));
			}
		}
		else {
			scene.AddBoolObj("HasCollider", false, to_string(i));
		}

		//DistanceJoint
		if (gameobjects[i]->GetDistanceJoint() != nullptr) {
			ComponentDistanceJoint* distance = gameobjects[i]->GetDistanceJoint();
			scene.AddBoolObj("HasDistanceJoint", true, to_string(i));
			
			if (distance->actorExtern != nullptr)
				scene.AddUnsignedIntObj("ActorExternUID", distance->actorExternReference, to_string(i));

			scene.AddBoolObj("MinDistanceEnable", distance->min_enable, to_string(i));
			scene.AddBoolObj("MaxDistanceEnable", distance->max_enable, to_string(i));

			physx::PxVec3 pivotA = distance->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
			physx::PxVec3 pivotB = distance->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

			scene.AddFloat3Obj("PivotA", { pivotA.x, pivotA.y, pivotA.z }, to_string(i));
			scene.AddFloat3Obj("PivotB", { pivotB.x, pivotB.y, pivotB.z }, to_string(i));

		}
		else {
			scene.AddBoolObj("HasDistanceJoint", false, to_string(i));
		}

		//RevoluteJoint[Hinge]
		if (gameobjects[i]->GetRevoluteJoint() != nullptr) {
			ComponentRevoluteJoint* hinge = gameobjects[i]->GetRevoluteJoint();
			scene.AddBoolObj("HasRevoluteJoint", true, to_string(i));

			if (hinge->actorExtern != nullptr)
				scene.AddUnsignedIntObj("ActorExternUID", hinge->actorExternReference, to_string(i));

			physx::PxVec3 pivotA = hinge->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
			physx::PxVec3 pivotB = hinge->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

			scene.AddFloat3Obj("PivotA", { pivotA.x, pivotA.y, pivotA.z }, to_string(i));
			scene.AddFloat3Obj("PivotB", { pivotB.x, pivotB.y, pivotB.z }, to_string(i));

			if (hinge->use_limit) {
				scene.AddBoolObj("useLimit", true, to_string(i));
				scene.AddFloatObj("lowerLimit", hinge->lower_limit, to_string(i));
				scene.AddFloatObj("upperLimit", hinge->upper_limit, to_string(i));
			}
			else {
				scene.AddBoolObj("useLimit", false, to_string(i));
			}

			if (hinge->use_motor) {
				scene.AddBoolObj("useMotor", true, to_string(i));
				scene.AddFloatObj("driveVelocity", hinge->velocity);
			}
			else {
				scene.AddBoolObj("useMotor", false, to_string(i));
			}

			if (hinge->use_freespin)
				scene.AddBoolObj("useFreespin", true, to_string(i));
			else
				scene.AddBoolObj("useFreespin", false, to_string(i));

		}
		else {
			scene.AddBoolObj("HasRevoluteJoint", false, to_string(i));
		}

		//SliderJoint
		if (gameobjects[i]->GetSliderJoint() != nullptr) {
			ComponentSliderJoint* slider = gameobjects[i]->GetSliderJoint();
			scene.AddBoolObj("HasSliderJoint", true, to_string(i));

			if (slider->actorExtern != nullptr)
				scene.AddUnsignedIntObj("ActorExternUID", slider->actorExternReference, to_string(i));

			physx::PxVec3 pivotA = slider->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
			physx::PxVec3 pivotB = slider->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

			scene.AddFloat3Obj("PivotA", { pivotA.x, pivotA.y, pivotA.z }, to_string(i));
			scene.AddFloat3Obj("PivotB", { pivotB.x, pivotB.y, pivotB.z }, to_string(i));

		}
		else {
			scene.AddBoolObj("HasSliderJoint", false, to_string(i));
		}

		//SphericalJoint
		if (gameobjects[i]->GetSphericalJoint() != nullptr) {			
			ComponentSphericalJoint* spherical = gameobjects[i]->GetSphericalJoint();
			scene.AddBoolObj("HasSphericalJoint", true, to_string(i));

			if (spherical->actorExtern != nullptr)
				scene.AddUnsignedIntObj("ActorExternUID", spherical->actorExternReference, to_string(i));

			physx::PxVec3 pivotA = spherical->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
			physx::PxVec3 pivotB = spherical->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

			scene.AddFloat3Obj("PivotA", { pivotA.x, pivotA.y, pivotA.z }, to_string(i));
			scene.AddFloat3Obj("PivotB", { pivotB.x, pivotB.y, pivotB.z }, to_string(i));

		}
		else {
			scene.AddBoolObj("HasSphericalJoint", false, to_string(i));
		}
	}


	string path = "Assets/Scenes/" + name + ".ZeroScene";
	scene.Save(path.c_str());
}