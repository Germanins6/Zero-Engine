#pragma once

#include "Component.h"

//Forward declaration
class ResourceMaterial;

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, Resource* ourMaterial);
	~ComponentMaterial();

	bool Update(float dt);

	void GenerateCheckers(); // this could be a performance improve if we create just one instead generating one per each material

	inline ResourceMaterial* GetMaterial() { return materialReference; };
	inline void SetMaterial(ResourceMaterial* newMaterial) { materialReference = newMaterial; };
	
private:

	ResourceMaterial* materialReference;
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

public:
	bool draw_texture;
	bool draw_checkers;
	uint CheckersID;
};