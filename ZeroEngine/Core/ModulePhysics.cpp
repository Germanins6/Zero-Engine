#include "ModulePhysics.h"

#include "PxPhysicsAPI.h"

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled) {

}

ModulePhysics::~ModulePhysics() {

}

bool ModulePhysics::Init() {

	LOG("Init PhysX");

  /*  static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;*/

	return true;
}

bool ModulePhysics::CleanUp() {

	return true;
}