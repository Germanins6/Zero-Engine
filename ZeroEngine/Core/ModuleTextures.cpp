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

	//Cleaning texture buffers and vector
	for (size_t i = 0; i < textures.size(); i++)
		RELEASE(textures[i])

		textures.clear();
}


bool ModuleTextures::Init() {
	bool ret = true;

	//Devil Ilu and Il initialization
	ilInit();
	iluInit();
	ilutInit();

	if (ilutRenderer(ILUT_OPENGL)) {
		LOG("DevIL: Renderer set as OpenGL");
		LOG("DevIL succesfully loaded | Libs initialized");
		ret = true;
	}
	else {
		LOG("Error trying load DevIL renderer");
		ret = false;
	}

	return ret;
}

Texture* ModuleTextures::Load(const char* path) {

	ILuint temp = 0;
	ilGenImages(1, &temp);
	ilBindImage(temp);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	
	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = 0;
	string norm_path_short;
	if (buffer == nullptr) {
		norm_path_short = "Assets/Textures/" + App->file_system->SetNormalName(path);
		bytesFile = App->file_system->Load(norm_path_short.c_str(), &buffer);
	}
	std::string extension(path);
	ILenum type = IL_TYPE_UNKNOWN;
	extension = extension.substr(extension.find_last_of("."));
	if (extension == ".png"){
		type=IL_PNG;
	}
	else if (extension == ".jpg") {
		type = IL_JPG;
	}
	else if (extension == ".tga") {
		type = IL_TGA;
	}

	if (type != IL_TYPE_UNKNOWN && buffer != nullptr) {
		if (ilLoadL(type, buffer, bytesFile) == IL_FALSE) {
			if (ilLoadImage(norm_path_short.c_str()) == IL_FALSE) 
				LOG("Source image from %s path Loaded Succesfully", norm_path_short.c_str())
			else 
				LOG("Unable to load texture");
		}
	}

	LOG("Source image from %s path Loaded Succesfully", norm_path_short.c_str())
	//Initialitizing texture values and buff
	Texture* image = nullptr;
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	image = new Texture(temp, ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_FORMAT), ilGetData());
	textures.push_back(image);

	ilBindImage(0);
	
	LOG("Succesfully image loaded with: ID %u SIZE %u X %u", image->id, image->width, image->height);
	RELEASE_ARRAY(buffer);
	return image;
}	