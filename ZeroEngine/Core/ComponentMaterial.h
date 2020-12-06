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
	
private:

	ResourceMaterial* materialReference;
	uint CheckersID;

	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

public:
	bool renderTextures;
	bool draw_texture;
	bool draw_checkers;

};