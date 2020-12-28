#include "Application.h"
#include "ComponentCollider.h"

ComponentCollider::ComponentCollider(GameObject* parent) : Component(parent, ComponentType::COLLIDER){
	
	colliderShape = App->physX->CreateCollider(GeometryType::BOX, owner->GetOBB().Size()); //-->Should have a function to create depending shape App->physx->CreateCollider

	if (owner->GetRigidbody() != nullptr)
		owner->GetRigidbody()->rigid_dynamic->attachShape(*colliderShape);

	
	drawCollider = false;
}

ComponentCollider::~ComponentCollider() {

}

bool ComponentCollider::Update(float dt) {
	

	
	return true;
}