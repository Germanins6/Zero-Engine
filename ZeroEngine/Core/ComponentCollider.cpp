#include "Application.h"
#include "ModuleCamera3D.h"
#include "ComponentCollider.h"
#include "ModulePhysics.h"

ComponentCollider::ComponentCollider(GameObject* parent, GeometryType geoType) : Component(parent, ComponentType::COLLIDER) {

	type = geoType;
	isTrigger = false;

	//Checks if component does have any owner and get additional data to be created
	if (owner != nullptr) {
		
		transform = owner->GetTransform();
		rigidbody = owner->GetRigidbody();

		//If gameObject does have mesh we apply measures directly to collider from OBB
		if (owner->GetMesh() != nullptr) {
			colliderSize = owner->GetAABB().Size();
			colliderSize = colliderSize.Mul(transform->scale);
			colliderShape = App->physX->CreateCollider(type, colliderSize / 2);
		}
		else {
			colliderSize = { 1.0f, 1.0f, 1.0f };
			colliderShape = App->physX->CreateCollider(type, colliderSize / 2);
		}

		colliderEuler = transform->euler;
		colliderMaterial = nullptr;

		//If we have a rigid body and doesnt have reference collider we attach the current one
		if (rigidbody != nullptr && rigidbody->collider_info == nullptr)
			rigidbody->collider_info = this;

		SetPosition(transform->position);
		UpdateValues();

		//We attach shape to a static or dynamic rigidbody to be collidable.
		if (rigidbody != nullptr) {
			rigidbody->rigid_dynamic->attachShape(*colliderShape);
			rigidStatic = nullptr;
		}
		else {
			rigidStatic = App->physX->CreateRigidStatic(colliderPos);
			rigidStatic->attachShape(*colliderShape);
		}
	
	}
	
	if (App->camera->editor_camera_transform != nullptr && App->camera->camera_created == false) {

		colliderRadius = 3.0f;
		colliderShape = App->physX->CreateCollider(type, { colliderRadius, colliderRadius, colliderRadius });
		colliderMaterial = nullptr;
		
		transform = App->camera->editor_camera_transform;

		colliderEuler = transform->euler;

		SetPosition(transform->position);
		rigidbody = App->camera->editor_camera_rigid;
		rigidbody->rigid_dynamic->attachShape(*colliderShape);
		rigidbody->EnableGravity(false);
	}
}

ComponentCollider::~ComponentCollider() {

	if (rigidStatic != nullptr)
		App->physX->ReleaseActor((PxRigidActor*)rigidStatic);

	if (colliderMaterial != nullptr)
		colliderMaterial->release();

	if(colliderShape != nullptr)
		colliderShape->release();
}

bool ComponentCollider::Update(float dt) {
	
	if (colliderShape != nullptr)
		App->physX->DrawCollider(this);
	
	return true;
}

void ComponentCollider::SetPosition(float3 position) {

	colliderPos = (position);

	PxTransform transformation = colliderShape->getLocalPose();
	float3 new_position = colliderPos.Mul(transform->scale);
	transformation.p = PxVec3(new_position.x, new_position.y, new_position.z);

	colliderShape->setLocalPose(transformation); //Set new Transformation Values

}

void ComponentCollider::SetRotation(float3 rotation) {

	colliderEuler = rotation;

	PxTransform transformation = colliderShape->getLocalPose();
	float3 rot = DEGTORAD * colliderEuler;
	Quat new_rotation = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);
	transformation.q = PxQuat(new_rotation.x, new_rotation.y, new_rotation.z, new_rotation.w);

	colliderShape->setLocalPose(transformation); //Set new Transformation Values

}

void ComponentCollider::SetScale(float3 scale, float radius) {
	
	PxTransform transformation = colliderShape->getLocalPose();
	float3 new_colliderDimensions = { 1.0f, 1.0f, 1.0f };
	float new_radius = 1.0f;

	transformation = SetLocalPose(transformation);
	colliderShape->setLocalPose(transformation);

	switch (type)
	{
	case GeometryType::BOX:
		colliderSize = scale;
		new_colliderDimensions = colliderSize.Mul(transform->scale) / 2;
		colliderShape->setGeometry(PxBoxGeometry(new_colliderDimensions.x, new_colliderDimensions.y, new_colliderDimensions.z));
		break;
	case GeometryType::SPHERE:
		colliderRadius = radius;

		new_radius = colliderRadius;
		colliderShape->setGeometry(PxSphereGeometry(new_radius));
		break;
	case GeometryType::CAPSULE:
		colliderSize = scale;
		colliderRadius = radius;
		new_radius = colliderRadius;
		new_colliderDimensions = colliderSize.Mul(transform->scale) / 2;
		colliderShape->setGeometry(PxCapsuleGeometry(new_radius, new_colliderDimensions.y));

		break;
	}

}

physx::PxTransform ComponentCollider::SetLocalPose(physx::PxTransform transformation) {

	float3 new_position = colliderPos.Mul(transform->scale);
	float3 rot = DEGTORAD * colliderEuler;
	Quat new_rotation = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);

	transformation.p = PxVec3(new_position.x, new_position.y, new_position.z);
	transformation.q = PxQuat(new_rotation.x, new_rotation.y, new_rotation.z, new_rotation.w);

	return transformation;

}

void ComponentCollider::SetTrigger(bool trigger) {

	if (trigger)
	{
		colliderShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		colliderShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else {
		colliderShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		colliderShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
	}
}

void ComponentCollider::UpdateValues() {

	SetPosition(owner->GetOBB().pos);
	SetRotation(owner->GetTransform()->euler);
	float3 scale = owner->GetOBB().Size();
	LOG("%f %f %f", scale.x, scale.y, scale.z);
	//SetScale();
}