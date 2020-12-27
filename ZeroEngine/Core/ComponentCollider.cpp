#include "Application.h"
#include "ComponentCollider.h"

ComponentCollider::ComponentCollider(GameObject* parent) : Component(parent, ComponentType::COLLIDER){
	draw = false;
}

ComponentCollider::~ComponentCollider() {

}

bool ComponentCollider::Update(float dt) {
	
	
	return true;
}