#include "ModuleTextures.h"

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

	Texture* image = nullptr;

	if (ilLoadImage(path)) {
		LOG("Source image from %s path Loaded Succesfully", path);
		

		Texture* image = new Texture(0, ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
		ilGenImages(1, &image->id);
		ilBindImage(image->id);

		LOG("%u", image->height);
		return image;
	}
	else {
		LOG("Unable to load texture");
	}

}