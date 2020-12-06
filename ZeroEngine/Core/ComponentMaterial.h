#pragma once

#include "Component.h"

//Forward declaration
class ResourceMaterial;

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent, UID ourMaterial);
	~ComponentMaterial();

	bool Update(float dt);


	void GenerateTextureInfo(); //-> this should be maybe at import options and should store gpu_id to read later with uids
	void GenerateCheckers();

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