#pragma once

#include "Component.h"

struct Texture;
class ResourceMaterial;
typedef unsigned int UID;

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, UID ourMaterial);
	~ComponentMaterial();
	

public:

	UID materialReference;
};