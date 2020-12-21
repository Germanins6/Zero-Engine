#include "ModulePhysics.h"

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled) {

}

ModulePhysics::~ModulePhysics() {

}

bool ModulePhysics::Init() {

	LOG("Init PhysX");
	//Init NVIDIA - PhysX
	return true;
}

bool ModulePhysics::CleanUp() {

	return true;
}