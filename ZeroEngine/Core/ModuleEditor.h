#pragma once
#include "Module.h"
#include "Globals.h"

#include "glew/include/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <string>

class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	
	void About_Window();
	void UpdateText(std::string consoleText);

public:

	bool show_demo_window;
	bool show_another_window;
	bool show_about_window;
	bool show_conf_window;
	bool show_console_window;


	ImGuiTextBuffer console_text;

	bool is_cap;

};