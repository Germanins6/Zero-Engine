#pragma once

#include <vector>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleEditor.h"
#include "Timer.h"
#include "PerfTimer.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleEditor* editor;

private:

	Timer	ms_timer;
	
	std::vector<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void DrawFPSDiagram();
	void DrawHardwareConsole();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	PerfTimer				ptimer;

public:

	bool closeEngine;
	bool vsync;
	float fps;
	float	dt;
	int cap;
	int	 capped_ms;
	Timer				frame_time;
	uint64				frame_count = 0;
	Timer				startup_time;
	Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

};