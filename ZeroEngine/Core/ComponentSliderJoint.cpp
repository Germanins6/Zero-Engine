#include "p2Defs.h"
#include "ComponentSliderJoint.h"
#include "Application.h"

#include "physx/include/extensions/PxPrismaticJoint.h"

ComponentSliderJoint::ComponentSliderJoint(GameObject* parent) : Component(parent, ComponentType::PRISMATIC_JOINT) {

	//First we check if we have any actor to attach the constrain
	if (owner->GetRigidbody() == nullptr)
		owner->CreateComponent(ComponentType::RIGIDBODY);

	actorOwner = owner->GetRigidbody()->rigid_dynamic;
	actorExtern = nullptr;
	actorExternReference = 0;
	joint = nullptr;

}

ComponentSliderJoint::~ComponentSliderJoint() {
	actorOwner = nullptr;
	actorExtern = nullptr;
	joint = nullptr;
}

void ComponentSliderJoint::CreateJoint(GameObject* draggedGameobject) {

	if (draggedGameobject->GetRigidbody() != nullptr) {
		actorExtern = draggedGameobject->GetRigidbody()->rigid_dynamic;
		actorExternReference = draggedGameobject->Getuid();
	}

	if(actorExtern != nullptr)
		joint = physx::PxPrismaticJointCreate(*App->physX->mPhysics, actorOwner, actorOwner->getGlobalPose(), actorExtern, actorExtern->getGlobalPose());

}

void ComponentSliderJoint::SetPosition(int actor, physx::PxVec3 position) {

	physx::PxTransform transform(position);

	if (actor == 0)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR0, transform);
	else if (actor == 1)
		joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, transform);

}


bool ComponentSliderJoint::Update(float dt) {

	//Search once the reference of extern actor
	if (actorExtern == nullptr && actorExternReference != 0) {
		GameObject* gameobjectReference = App->resources->SearchGameObjectByUID(actorExternReference);
		CreateJoint(gameobjectReference);
	}

	return true;
}