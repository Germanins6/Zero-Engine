#ifndef _APPLICATION_
#define _APPLICATION_

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
#include "ModuleEditor.h"
#include "ViewportBuffer.h"
#include "PrimitivesGL.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "ModuleGeometry.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	Primitives* primitivesGL;
	ViewportBuffer* viewport_buffer;
	ModuleGeometry* geometry;

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
	JSON_Object* test;

private: 
	std::vector<Module*> list_modules;

	JSON_Value* config_file;
	const char* config_path;

};

extern Application* App;

#endif //_APPLICATION_