#include "p2Defs.h"
#include "ComponentSphericalJoint.h"
#include "Application.h"

#include "physx/include/extensions/PxSphericalJoint.h"

ComponentSphericalJoint::ComponentSphericalJoint(GameObject* parent) : Component(parent, ComponentType::SPHERICAL_JOINT) {

	//First we check if we have any actor to attach the constrain
	if (owner->GetRigidbody() == nullptr)
		owner->CreateComponent(ComponentType::RIGIDBODY);

	actorOwner = owner->GetRigidbody()->rigid_dynamic;
	actorExtern = nullptr;
	actorExternReference = 0;
	joint = nullptr;

}

ComponentSphericalJoint::~ComponentSphericalJoint() {
	actorOwner = nullptr;
	actorExtern = nullptr;
	joint = nullptr;
}

void ComponentSphericalJoint::CreateJoint(GameObject* draggedGameobject) {

	if (draggedGameobject->GetRigidbody() != nullptr) {
		actorExtern = draggedGameobject->GetRigidbody()->rigid_dynamic;
		actorExternReference = draggedGameobject->Getuid();
	}

	if(actorExtern != nullptr)
		joint = physx::PxSphericalJointCreate(*App->physX->mPhysics, actorOwner, actorOwner->getGlobalPose(), actorExtern, actorExtern->getGlobalPose());

}

void ComponentSphericalJoint::SetPosition(int actor, physx::PxVec3 position) {

	physx::PxTransform transform(position);

	if (actor == 0)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, transform);
	else if (actor == 1)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, transform);

}