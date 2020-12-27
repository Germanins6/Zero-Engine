#include "Application.h"
#include "ComponentRigidDynamic.h"

ComponentRigidDynamic::ComponentRigidDynamic(GameObject* parent) : Component(parent, ComponentType::RIGIDBODY) {
	
}

ComponentRigidDynamic::~ComponentRigidDynamic() {

}

bool ComponentRigidDynamic::Update(float dt) {


	return true;
}