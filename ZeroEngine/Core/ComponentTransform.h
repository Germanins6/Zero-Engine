#pragma once

#include "Component.h"

class ComponentTransform : public Component {

public:

	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	void SetPosition();
	void SetRotation();
	void SetScale();

public:
	
	vec3 position;
	vec3 rotation;
	vec3 scale;
};