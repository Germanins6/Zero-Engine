#include "ModuleTextures.h"

//Devil Libs loading
#pragma comment(lib, "Core/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILU.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILUT.lib")

ModuleTextures::ModuleTextures(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleTextures::~ModuleTextures() {

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

bool  ModuleTextures::CleanUp() {
	return true;
}

bool  ModuleTextures::Load(const char* path) {

	bool ret = false;

	if (ilLoadImage(path)) {
		LOG("Source image from %s path Loaded Succesfully");
		ret = true;
	}


	return ret;
}