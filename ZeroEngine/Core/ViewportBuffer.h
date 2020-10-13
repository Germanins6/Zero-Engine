#pragma once
#include "Module.h"
#include "Globals.h"

#include "glew/include/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <string>

class ViewportBuffer : public Module
{
public:
	ViewportBuffer(Application* app, bool start_enabled = true);
	~ViewportBuffer();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	

public:

	GLuint frameBuffer;
	uint renderBufferoutput;
	uint texture;
	bool show_viewport_window;

};