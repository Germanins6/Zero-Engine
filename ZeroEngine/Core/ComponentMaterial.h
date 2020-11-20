#pragma once

#include "Component.h"

struct Texture;

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, const char* path, Texture* ourTexture);
	~ComponentMaterial();
	
	void UpdateTextureInfo(const char* path);

public:
	Texture* diffuse;
	const char* texture_path;
};