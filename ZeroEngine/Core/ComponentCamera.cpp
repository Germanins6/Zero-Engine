#include "Application.h"
#include "ModuleTextures.h"

#include "ComponentCamera.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent, ComponentType::MATERIAL){


}

ComponentCamera::~ComponentCamera() {

}
