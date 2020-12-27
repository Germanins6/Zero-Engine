#include "Application.h"
#include "ComponentRigidDynamic.h"

ComponentRigidDynamic::ComponentRigidDynamic(GameObject* parent) : Component(parent, ComponentType::RIGIDBODY) {
	
	//GeometryType set as box just for testing purpose
	rigid_dynamic = App->physX->CreateGeometry(GeometryType::BOX);

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

}

bool ComponentRigidDynamic::Update(float dt) {

	
	return true;
}