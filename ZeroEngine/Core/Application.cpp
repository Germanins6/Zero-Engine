#include "Application.h"
#include "p2Defs.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"

#include "ModuleWindow.h"

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

	//Control when the Diagram is working

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
	bool ret = true;
	PERF_START(ptimer);
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
	dt = (float)ms_timer.ReadMs() / 1000.0f;
	ms_timer.Start();

	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.Read();
	frame_time.Start();

}

// ---------------------------------------------
void Application::FinishUpdate()
{

	// Framerate calculations --
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.Read();
	float seconds_since_startup = startup_time.Read();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];

	sprintf_s(title, 256, " Zero Engine v0.0 - ZeroEMP | FPS: %d",
		prev_last_sec_frame_count);

	window->SetTitle(title);

	/*if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		Timer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}*/
	
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

void Application::DrawFPSDiagram() {

	ImGui::InputText("App Name", TITLE, 20);

	/*char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotLines("##framerate", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));*/

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