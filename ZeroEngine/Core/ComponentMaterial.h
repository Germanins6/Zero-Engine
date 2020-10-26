#pragma once

#include "Component.h"

// -- ModuleTextures
#include "ModuleTextures.h"

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

public:

	Texture* data;
	const char* texture_path;
};