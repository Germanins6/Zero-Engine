#pragma once

#include "Module.h"
#include <vector>

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

struct Texture {

	uint id = 0;
	const char* path = nullptr;
	//type? bmp, jpg, png, jpeg etc
};

class ModuleTextures : public Module {

public:
	ModuleTextures(Application* app, bool start_enabled = true);
	~ModuleTextures();

	bool Init();
	bool CleanUp();

	bool Load(const char* path);


public:

	std::vector<Texture*> textures;
};