#include "Application.h"
#include "p2Defs.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"


Application::Application()
{
	PERF_START(ptimer);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);

	//Control variable to close App
	closeEngine = false;
	vsync = false;
	fps = 0.0f;
	cap = 60;
	capped_ms = -1;


	PERF_PEEK(ptimer);
}

Application::~Application()
{

	for (uint i = 0; i < list_modules.size(); i++)
	{

		delete list_modules[i];

	}

	list_modules.clear();

}

bool Application::Init()
{
	PERF_START(ptimer);
	bool ret = true;

	// Call Init() in all modules
	for (size_t i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (size_t i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->Start();
	}

	
	ms_timer.Start();
	PERF_PEEK(ptimer);

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{

	frame_count++;
	last_sec_frame_count++;

	dt = (float)frame_time.ReadSec();
	frame_time.Start();

}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (cap > 0)
	{
		capped_ms = 1000 / cap;
	}

	// Framerate calculations --
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		fps = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	uint32 last_frame_ms = frame_time.Read();

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{

	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (size_t i = 0; i < list_modules.size() && ret== UPDATE_CONTINUE; i++)
	{
		list_modules[i]->PreUpdate(dt);
	}

	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		list_modules[i]->Update(dt);
	}


	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		list_modules[i]->PostUpdate(dt);

	}

	// If main menu bar exit button pressed changes closeEngine bool to true and closes App
	if (closeEngine) ret = UPDATE_STOP;

	FinishUpdate();


	return ret;
}



bool Application::CleanUp()
{
	bool ret = true;
	for (size_t i = 0; i < list_modules.size() && ret == true; i++)
	{
		list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}