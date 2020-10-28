#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"

//Devil Libs loading
#pragma comment(lib, "Core/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILU.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILUT.lib")

ModuleTextures::ModuleTextures(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleTextures::~ModuleTextures() {
	CleanUp();
}


bool ModuleTextures::Init() {
	bool ret = true;

	//Devil Ilu and Il initialization
	ilInit();
	iluInit();
	ilutInit();

	if (ilutRenderer(ILUT_OPENGL)) {
		LOG("DevIL Renderer Set as OpenGL");
		ret = true;
	}
	else {
		LOG("Error trying load DevIL renderer");
		ret = false;
	}

	return ret;
}

bool ModuleTextures::CleanUp() {

	//Cleaning texture buffers and vector
	for (size_t i = 0; i < textures.size(); i++)
	{
		glDeleteTextures(1, &textures[i]->id);
		textures[i] = nullptr;
	}

	textures.clear();

	return true;
}

Texture* ModuleTextures::Load(const char* path) {

	ILuint temp = 0;
	ilGenImages(1, &temp);
	ilBindImage(temp);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	//Create path buffer and import to scene
	/*char* buffer = nullptr;
	uint bytesFile = 0;

	if (buffer == nullptr) {
		string normalized_path(path);
		string new_path = normalized_path.substr(App->editor->ReturnNameObject(normalized_path, 0x5c) - 14);
		string norm_path_short = App->file_system->NormalizePath(new_path.c_str());

		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}

	if (buffer != nullptr) {
		
	}
	else {
		
	}*/

	if (ilLoadImage(path))
		LOG("Source image from %s path Loaded Succesfully", path)
	else
		LOG("Unable to load texture");

	//Initialitizing texture values and buff
	Texture* image = nullptr;
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	image = new Texture(temp, ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
	textures.push_back(image);
	
	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", image->id, image->width, image->height);
	return image;
}	