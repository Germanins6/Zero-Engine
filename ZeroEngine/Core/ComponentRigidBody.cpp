#include "ComponentRigidBody.h"
#include "Application.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent) : Component(parent, ComponentType::RIGIDBODY) {

	//rigidbody = App->physX->CreateDynamic({1.0f, 1.0f, 1.0f}, 10.0f);
}

ComponentRigidBody::~ComponentRigidBody() {

}

