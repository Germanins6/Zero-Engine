#include "Application.h"
#include "ComponentRigidDynamic.h"

ComponentRigidDynamic::ComponentRigidDynamic(GameObject* parent) : Component(parent, ComponentType::RIGIDBODY) {
	
	goTransform = owner->GetTransform();
	collider_info = owner->GetCollider();

	rigid_dynamic = App->physX->CreateRigidbody(goTransform->position);

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

	physx::PxShape* shapes[128];
	rigid_dynamic->getShapes(shapes, rigid_dynamic->getNbShapes());

	//TODO: DETACH ALL SHAPES ATTACHED TO RIGID BODY/ACTOR
	for (size_t i = 0; i < rigid_dynamic->getNbShapes(); i++)
		rigid_dynamic->detachShape(*shapes[i]);
}

bool ComponentRigidDynamic::Update(float dt) {

	
	//TODO : if(play) doit

	if (collider_info != nullptr) {
		
		//collider_info->UpdateMatrix();
		goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });

	}
	else {
		goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });
	}

	goTransform->UpdateNodeTransforms();


	return true;
}