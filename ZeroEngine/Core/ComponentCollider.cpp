#include "Application.h"
#include "ComponentCollider.h"
#include "ModulePhysics.h"

ComponentCollider::ComponentCollider(GameObject* parent) : Component(parent, ComponentType::COLLIDER){
	
	isTrigger = false;

	rigidbody = owner->GetRigidbody();
	transform = owner->GetTransform();

	if (owner->GetMesh() != nullptr)
		colliderSize = owner->GetOBB().Size();

	if (rigidbody != nullptr && rigidbody->collider_info == nullptr)
		rigidbody->collider_info = this;

	colliderDim = colliderSize.Mul(transform->scale);
	colliderShape = App->physX->CreateCollider(GeometryType::BOX, colliderDim / 2); //-->Should have a function to create depending shape App->physx->CreateCollider
	colliderMaterial = nullptr;

	SetPosition(owner->GetOBB().pos);

	if (rigidbody != nullptr)
		rigidbody->rigid_dynamic->attachShape(*colliderShape);

}

ComponentCollider::~ComponentCollider() {
	colliderShape->release();
}

bool ComponentCollider::Update(float dt) {
	
	if (colliderShape != nullptr)
		App->physX->DrawCollider(this);
	
	return true;
}

void ComponentCollider::SetPosition(float3 position) {

	colliderPos = position;

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

void ComponentCollider::SetScale(float3 scale) {

	colliderSize = scale;
	
	PxTransform transformation = colliderShape->getLocalPose();
	float3 new_colliderDimensions = colliderSize.Mul(transform->scale) / 2;
	
	float3 new_position = colliderPos.Mul(transform->scale);
	float3 rot = DEGTORAD * colliderEuler;
	Quat new_rotation = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);

	transformation.p = PxVec3(new_position.x, new_position.y, new_position.z);
	transformation.q = PxQuat(new_rotation.x, new_rotation.y, new_rotation.z, new_rotation.w);

	colliderShape->setLocalPose(transformation);
	colliderShape->setGeometry(PxBoxGeometry(new_colliderDimensions.x, new_colliderDimensions.y, new_colliderDimensions.z));

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