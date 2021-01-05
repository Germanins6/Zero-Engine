#include "ComponentRevoluteJoint.h"
#include "Application.h"

#include "physx/include/extensions/PxRevoluteJoint.h"

ComponentRevoluteJoint::ComponentRevoluteJoint(GameObject * parent) : Component(parent, ComponentType::REVOLUTE_JOINT) {

	//First we check if we have any actor to attach the constrain
	if (owner->GetRigidbody() == nullptr)
		owner->CreateComponent(ComponentType::RIGIDBODY);

	actorOwner = owner->GetRigidbody()->rigid_dynamic;
	actorExtern = nullptr;
	joint = nullptr;

}

ComponentRevoluteJoint::~ComponentRevoluteJoint() {
	actorOwner = nullptr;
	actorExtern = nullptr;
	joint = nullptr;
}

void ComponentRevoluteJoint::CreateJoint(GameObject* draggedGameobject) {

	if (draggedGameobject->GetRigidbody() != nullptr)
		actorExtern = draggedGameobject->GetRigidbody()->rigid_dynamic;

	if (actorExtern != nullptr)
		joint = physx::PxRevoluteJointCreate(*App->physX->mPhysics, actorOwner, actorOwner->getGlobalPose(), actorExtern, actorExtern->getGlobalPose());

}

void ComponentRevoluteJoint::SetPosition(int actor, physx::PxVec3 position) {

	physx::PxTransform transform(position);

	if (actor == 0)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, transform);
	else if (actor == 1)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, transform);

}