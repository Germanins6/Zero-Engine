#include "ComponentDistanceJoint.h"
#include "Application.h"

#include "physx/include/extensions/PxDistanceJoint.h"

ComponentDistanceJoint::ComponentDistanceJoint(GameObject* parent) : Component(parent, ComponentType::DISTANCE_JOINT) {

	//First we check if we have any actor to attach the constrain
	if (owner->GetRigidbody() == nullptr)
		owner->CreateComponent(ComponentType::RIGIDBODY);

	actorOwner = owner->GetRigidbody()->rigid_dynamic;
	actorExtern = nullptr;
	joint = nullptr;

	/*
	Getrigidbody 
	if rigidbody == nullptr -> createrigidbody
	create distancejoint and attack actor1 rigidbody this->rigidbody
	actor2 nullptr waiting for attach with drag and drop in engine
	*/
}

ComponentDistanceJoint::~ComponentDistanceJoint() {
	actorOwner = nullptr;
	actorExtern = nullptr;
	joint = nullptr;
}

void ComponentDistanceJoint::CreateJoint(GameObject* draggedGameobject) {

	if (draggedGameobject->GetRigidbody() != nullptr)
		actorExtern = draggedGameobject->GetRigidbody()->rigid_dynamic;

	if(actorExtern != nullptr)
		joint = physx::PxDistanceJointCreate(*App->physX->mPhysics, actorOwner, actorOwner->getGlobalPose(), actorExtern, actorExtern->getGlobalPose());

}

void ComponentDistanceJoint::SetPosition(int actor, physx::PxVec3 position) {

	physx::PxTransform transform(position);

	if (actor == 0)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, transform);
	else if (actor == 1)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, transform);

}