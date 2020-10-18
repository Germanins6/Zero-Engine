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

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	

	//Docking Helper functions
	bool DockingRootItem(char* id, ImGuiWindowFlags winFlags);
	void BeginDock(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size = { .0f, .0f });

	//Core Update functions to show and manage windows
	void MenuBar();
	void UpdateWindowStatus();

	//Console Text Pushback
	void UpdateText(std::string consoleText);

	void About_Window();	//Can be done better

public:

	//Window status control
	bool show_demo_window;
	bool show_another_window;
	bool show_about_window;
	bool show_conf_window;
	bool show_console_window;
	bool show_inspector_window;
	bool show_hierarchy_window;
	bool show_scene_window;
	bool show_game_window;

	ImGuiTextBuffer console_text;
	bool draw;
	bool is_cap;

	ImVec4 current_color;

	ImGuiWindowFlags scene_window;

};