#pragma once

#include "Component.h"

// -- ModuleTextures
#include "ModuleTextures.h"

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, const char* path);
	~ComponentMaterial();

public:

	Texture* TextureData;
	const char* texture_path;
};