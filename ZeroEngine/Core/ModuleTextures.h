#pragma once

#include "Module.h"
#include <vector>
#include <string>

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "glew/include/glew.h"

using namespace std;

struct Texture {

	Texture(ILuint my_id, ILint h, ILint w, ILint format, ILubyte* info) : id{ my_id }, height { h }, width{ w }, type{ format }, data{ info }{}
	~Texture() {
		glDeleteTextures(1, &this->id);
		ilDeleteImages(1, &this->id);
		data = nullptr;
	}
	ILuint id;
	ILint height, width, type;
	ILubyte* data;

	//Get size and type
	ILint GetWidth(){ return width; }
	ILint GetHeight() { return height; }
	ILubyte GetType() { return type; }

};

class ModuleTextures : public Module {

public:
	ModuleTextures(Application* app, bool start_enabled = true);
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	Texture* Load(const char* path);


public:

	vector<Texture*> textures;
};