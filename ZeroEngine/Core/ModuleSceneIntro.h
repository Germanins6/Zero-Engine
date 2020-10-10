#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	vec3 cube_pos;
	vec3 pyramid_pos;
public:

	bool draw;
};
