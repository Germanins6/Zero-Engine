#include "Application.h"
#include "ModuleTextures.h"

#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent, const char* path) : Component(parent, ComponentType::MATERIAL){
	
	draw_texture = true;
	texture_path = path;
	data = App->textures->Load(texture_path);

	data->GenerateBufferTexture();
}

ComponentMaterial::~ComponentMaterial() {

}

bool ComponentMaterial::Update(float dt) {

	if (draw_texture && dynamic_cast<ComponentMesh*>(owner->GetMesh())->mesh->tex_info != nullptr) {
		data->RenderTexture();
	}
	else {

	}
	return true;
}

void Texture::GenerateBufferTexture(){

	ComponentMesh* mesh_info = dynamic_cast<ComponentMesh*>(App->editor->gameobject_selected->GetMesh());
	//-- Generate Texture_Buffers
	this->my_texture = 0;
	glGenBuffers(1, (GLuint*)&(this->my_texture));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh_info->mesh->num_vertex, mesh_info->mesh->uv_coords, GL_STATIC_DRAW);

	//-- Generate Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&(this->id));
	glBindTexture(GL_TEXTURE_2D, this->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)this->GetWidth(), (int)this->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)this->data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, App->geometry->checkerImage);

}
void Texture::RenderTexture(){
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glBindTexture(GL_TEXTURE_2D, this->id);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}