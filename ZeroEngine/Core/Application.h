#ifndef _APPLICATION_
#define _APPLICATION_

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ViewportBuffer.h"
#include "PrimitivesGL.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "ModuleFileSystem.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "ModuleVehicle.h"

#include <vector>

#include <iostream>
#include <fstream>
using namespace std;

#include "JSON/json.hpp"
using json = nlohmann::json;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModulePhysics* physX;
	ModuleAudio* audio;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	Primitives* primitivesGL;
	ViewportBuffer* viewport_buffer;
	ModuleFileSystem* file_system;
	ResourceManager* resources;
	TimeManager* timeManager;
	ModuleVehicle* vehicle;

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

	void LoadJSON(const char* path);
	void SaveJSON(const char* path);

public:

	//Fps core
	float fps;
	float	dt;
	int cap;
	int	 capped_ms;
	PerfTimer				ptimer;
	Timer	ms_timer;
	Timer				frame_time;
	uint64				frame_count = 0;
	Timer				startup_time;
	Timer				last_sec_frame_time;
	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	//Engine configuration
	bool closeEngine;
	bool vsync;

	//JSON Stuff
	json config_file;
	string config_path;

private: 

	std::vector<Module*> list_modules;

};

extern Application* App;

#endif //_APPLICATION_