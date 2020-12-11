#include "Application.h"
#include "ZeroImporter.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, Resource* ourMaterial) : Component(parent, ComponentType::MATERIAL){

	//Save path info for showing purpose and stores information about texture in TextureData structure
	materialReference = dynamic_cast<ResourceMaterial*>(ourMaterial);
	
	//Check this variables and clean , some look like useless
	CheckersID = 0;

	draw_texture = true;
	draw_checkers = false;
	
	GenerateCheckers();
}

ComponentMaterial::~ComponentMaterial() {
}

bool ComponentMaterial::Update(float dt) {

	glColor4f(materialReference->materialColor.r, materialReference->materialColor.g, materialReference->materialColor.b, materialReference->materialColor.a);

	if (draw_texture && materialReference->diffuse != nullptr)
		glBindTexture(GL_TEXTURE_2D, materialReference->diffuse->gpu_id);

	if (draw_checkers)
		glBindTexture(GL_TEXTURE_2D, CheckersID);

	return true;
}

void ComponentMaterial::GenerateCheckers() {

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glGenTextures(1, (GLuint*)&(CheckersID));
	glBindTexture(GL_TEXTURE_2D, CheckersID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}