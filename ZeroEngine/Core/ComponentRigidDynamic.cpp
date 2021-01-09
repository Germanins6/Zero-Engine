#include "Application.h"
#include "ComponentRigidDynamic.h"

ComponentRigidDynamic::ComponentRigidDynamic(GameObject* parent) : Component(parent, ComponentType::RIGIDBODY) {
	
	if (App->camera->editor_camera_transform != nullptr && App->camera->camera_created == false) {
		goTransform = App->camera->editor_camera_transform;
		collider_info = nullptr;
	}
	else {
		goTransform = owner->GetTransform();
		collider_info = owner->GetCollider();
	}

	rigid_dynamic = App->physX->CreateRigidDynamic(goTransform->position);

	//Attach collider(shape) to actor if exist
	if (collider_info != nullptr)
		rigid_dynamic->attachShape(*collider_info->colliderShape);

	EnableGravity(use_gravity);
	EnableKinematic(use_kinematic);

	SetMass(mass);
	SetDensity(density);

	AddForce(force);
	AddTorque(torque);

	SetLinearVelocity(linear_vel);
	SetAngularVelocity(angular_vel);

	SetLinearDamping(linear_damping);
	SetAngularDamping(angular_damping);

}

ComponentRigidDynamic::~ComponentRigidDynamic() {

	App->physX->ReleaseActor((PxRigidActor*)rigid_dynamic);
}

bool ComponentRigidDynamic::Update(float dt) {

	//Just update transform if we have rigidbody simulation
	if (App->timeManager->started) {

		if (collider_info != nullptr) {

			goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });
			Quat rot = { rigid_dynamic->getGlobalPose().q.x, rigid_dynamic->getGlobalPose().q.y, rigid_dynamic->getGlobalPose().q.z,  rigid_dynamic->getGlobalPose().q.w };
			float3 new_rot = rot.ToEulerXYZ() * RADTODEG;

			goTransform->SetRotation({ new_rot.x,new_rot.y, new_rot.z, });
		}
		else
			goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });

		goTransform->UpdateNodeTransforms();
	}
	else {
		if (collider_info != nullptr)
			rigid_dynamic->setGlobalPose(PxTransform({ collider_info->transform->position.x, collider_info->transform->position.y, collider_info->transform->position.z }));
		else
			goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });
	}

	//TODO: MOVE RIGID BODY IF GLOBAL POSITION CHANGED

	return true;
}