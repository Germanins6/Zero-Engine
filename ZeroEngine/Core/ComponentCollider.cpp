#include "Application.h"
#include "ComponentCollider.h"

ComponentCollider::ComponentCollider(GameObject* parent) : Component(parent, ComponentType::COLLIDER){
	

	rigidbody = owner->GetRigidbody();
	transform = owner->GetTransform();

	ownerMatrix = transform->GetGlobalMatrix().Transposed();
	
	localMatrix = transform->GetLocalMatrix();
	globalMatrix = transform->GetGlobalMatrix().Transposed();

	localMatrix.Decompose(colliderPos, colliderRot, colliderSize);
	colliderEuler = colliderRot.ToEulerXYZ() * RADTODEG;

	colliderShape = App->physX->CreateCollider(GeometryType::BOX, colliderSize / 2); //-->Should have a function to create depending shape App->physx->CreateCollider

	if (rigidbody != nullptr)
		rigidbody->rigid_dynamic->attachShape(*colliderShape);

	drawCollider = false;
}

ComponentCollider::~ComponentCollider() {

}

bool ComponentCollider::Update(float dt) {
	

	
	return true;
}

void ComponentCollider::UpdateMatrix() {

	float3 positionGlobal, positionLocal, scaleGlobal, scaleLocal;
	Quat rotationGlobal, rotationLocal;

	if(rigidbody != nullptr){
		colliderPos = { rigidbody->rigid_dynamic->getGlobalPose().p.x, rigidbody->rigid_dynamic->getGlobalPose().p.y, rigidbody->rigid_dynamic->getGlobalPose().p.z };
		colliderRot = { rigidbody->rigid_dynamic->getGlobalPose().q.x, rigidbody->rigid_dynamic->getGlobalPose().q.y, rigidbody->rigid_dynamic->getGlobalPose().q.z, rigidbody->rigid_dynamic->getGlobalPose().q.w };
		colliderEuler = colliderRot.ToEulerXYZ() * RADTODEG;
	}
	SetPosition(colliderPos);
	SetRotation(colliderEuler);

	ownerMatrix = transform->GetGlobalMatrix().Transposed();

	globalMatrix.Decompose(positionLocal, rotationLocal, scaleLocal);

	colliderPos = positionLocal; //--Change Position of Collider
	colliderSize = scaleLocal; //--Change Rotation of Collider
	colliderRot = rotationLocal;
	colliderEuler = rotationLocal.ToEulerXYZ() * RADTODEG; //--Change Size of Collider
	
	UpdateLocalMatrix();
	UpdateGlobalMatrix();

	globalMatrix.Decompose(positionGlobal, rotationGlobal, scaleGlobal);

	physx::PxVec3 pos(positionGlobal.x, positionGlobal.y, positionGlobal.z);
	physx::PxQuat rot(rotationGlobal.x, rotationGlobal.y, rotationGlobal.z, rotationGlobal.w);
	physx::PxTransform transform(pos, rot);

	if(rigidbody != nullptr)
		owner->GetRigidbody()->rigid_dynamic->setGlobalPose(transform);

}

void ComponentCollider::SetPosition(float3 position) {

	colliderPos = position;

	UpdateLocalMatrix();
}

void ComponentCollider::SetRotation(float3 rotation) {

	colliderEuler = float3(rotation.x, rotation.y, rotation.z);
	colliderRot = Quat::FromEulerXYZ(rotation.x * DEGTORAD, rotation.y * DEGTORAD, rotation.z * DEGTORAD);

	UpdateLocalMatrix();
}

void ComponentCollider::SetScale(float3 scale) {

	colliderSize = scale;

	UpdateLocalMatrix();
}