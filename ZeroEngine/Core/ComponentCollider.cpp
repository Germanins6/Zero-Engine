#include "Application.h"
#include "ComponentCollider.h"

ComponentCollider::ComponentCollider(GameObject* parent) : Component(parent, ComponentType::COLLIDER){
	
	//collider = App->physX->CreateGeometry(type); //-->Should have a function to create depending shape App->physx->CreateCollider

	/*Pseudo code
	* 
	* Call shape creation
	* 
	* Detect if our gameobject does have rigid body -> GetRigidBody ->
	* 
	* in Case != nullptr we attach our shape into rigidbody
	* 
	* getrigidbody == nullptr means our object doesnt have rigid so que just create shape with properly geometry
	*/
	drawCollider = false;
}

ComponentCollider::~ComponentCollider() {

}

bool ComponentCollider::Update(float dt) {
	
	
	return true;
}