#include "Application.h"
#include "ZeroImporter.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, UID ourMaterial) : Component(parent, ComponentType::MATERIAL){

	//Save path info for showing purpose and stores information about texture in TextureData structure
	materialReference = dynamic_cast<ResourceMaterial*>(App->resources->RequestResource(ourMaterial));
	
	//Check this variables and clean , some look like useless
	CheckersID = 0;

	draw_texture = false;
	draw_checkers = false;
	GenerateTextureInfo();
	GenerateCheckers();
}

ComponentMaterial::~ComponentMaterial() {
}

bool ComponentMaterial::Update(float dt) {

	if (draw_texture) {
		glBindTexture(GL_TEXTURE_2D, materialReference->diffuse->gpu_id);
	}

	if (draw_checkers) {
		glBindTexture(GL_TEXTURE_2D, CheckersID);
	}

	return true;
}

void ComponentMaterial::GenerateTextureInfo() {

	//-- Generate Texture
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//if (draw_texture && this->tex_info != nullptr) {
	//	glGenTextures(1, (GLuint*)&(this->tex_info->id));
	//	glBindTexture(GL_TEXTURE_2D, this->tex_info->id);

	//}

	//if (draw_checkers && this->tex_info != nullptr) {
	//	glGenTextures(1, (GLuint*)&(this->textureID));
	//	glBindTexture(GL_TEXTURE_2D, this->textureID);
	//}

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//if (draw_texture && this->tex_info != nullptr)
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)this->tex_info->GetWidth(), (int)this->tex_info->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)this->tex_info->data);

	//if (draw_checkers)
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->checkerImage);

	////UnBind last
	//glBindTexture(GL_TEXTURE_2D, 0);

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