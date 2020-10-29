#include "Component.h"

Component::Component(GameObject* parent, ComponentType ty) {
	owner = parent;
	type = ty;
	active = true;
}

Component::~Component() {
	owner = nullptr;
}