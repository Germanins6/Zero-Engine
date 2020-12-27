#pragma once
#include "PhysX/include/PxPhysicsAPI.h"

#include "Component.h"
#include "Globals.h"
#include "Module.h"

class GameObject;

class ComponentRigidDynamic : public Component {

public:

	ComponentRigidDynamic(GameObject* parent);
	~ComponentRigidDynamic();

	bool Update(float dt);

	inline void EnableGravity(bool enable) { if (rigid_dynamic != nullptr) rigid_dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !enable); };
	inline void EnableKinematic(bool enable) { if (rigid_dynamic != nullptr) rigid_dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, !enable); };

	inline void SetMass(float mass) { if (rigid_dynamic != nullptr) rigid_dynamic->setMass(mass); };
	inline float GetMass() { if (rigid_dynamic != nullptr) return rigid_dynamic->getMass(); };

	inline void SetDensity(float density) { if (rigid_dynamic != nullptr) physx::PxRigidBodyExt::updateMassAndInertia(*rigid_dynamic, density); };

	inline void AddForce(float3 force) { if (rigid_dynamic != nullptr) rigid_dynamic->addForce({ force.x, force.y, force.z }); };
	inline void AddTorque(float3 torque) { if (rigid_dynamic != nullptr) rigid_dynamic->addTorque({ torque.x, torque.y, torque.z }); };

	//------------Velocity------------//
	inline void SetLinearVelocity(float3 linear_velocity) { if (rigid_dynamic != nullptr) rigid_dynamic->setLinearVelocity({ linear_velocity.x, linear_velocity.y, linear_velocity.z }); };
	inline physx::PxVec3 GetLinearVelocity() { if (rigid_dynamic != nullptr) return rigid_dynamic->getLinearVelocity(); };

	inline void SetAngularVelocity(float3 angular_velocity) { if (rigid_dynamic != nullptr) rigid_dynamic->setAngularVelocity({ angular_velocity.x, angular_velocity.y, angular_velocity.z }); };
	inline physx::PxVec3 GetAngularVeloctity() { if (rigid_dynamic != nullptr) return rigid_dynamic->getAngularVelocity(); };

	//------------Damping------------//
	inline void SetAngularDamping(float angular_Damping) { if (rigid_dynamic != nullptr) rigid_dynamic->setAngularDamping(angular_Damping); };
	inline float GetAngularDamping() { if (rigid_dynamic != nullptr) return rigid_dynamic->getAngularDamping(); };

	inline void SetLinearDamping(float linear_Damping) { if (rigid_dynamic != nullptr) rigid_dynamic->setLinearDamping(linear_Damping); };
	inline float GetLinearDamping() { if (rigid_dynamic != nullptr) return rigid_dynamic->getLinearDamping(); };

public:

	physx::PxRigidDynamic* rigid_dynamic; 

	//------------Initial Values of Rigid Body------------//
	bool use_gravity = true;
	bool use_kinematic = false;

	float mass = 10.0f;
	float density = 1.0f;

	float3 force = float3::zero;
	float3 torque = float3::zero;

	float3 linear_vel = float3::zero;
	float3 angular_vel = float3::zero;

	float linear_damping = 0.0f;
	float angular_damping = 0.0f;

};