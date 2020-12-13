#include "Globals.h"
#include "Application.h"
#include "TimeManager.h"
#include "ModuleEditor.h"
#include "Timer.h"
#include "PerfTimer.h"

TimeManager::TimeManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

TimeManager::~TimeManager()
{}

// -----------------------------------------------------------------
bool TimeManager::Init()
{
	LOG("Setting up the camera");
	bool ret = true;

	gameTime = 0.0f;
	realTime = 0.0f;
	frames = 0;
	speed = 1.0f;
	tick = false;
	Pause();

	return ret;
}

// -----------------------------------------------------------------
bool TimeManager::CleanUp()
{
	gameTime = 0.0f;
	realTime = 0.0f;
	frames = 0;
	speed = 1.0f;

	return true;
}

void TimeManager::StartGame() {
	isPaused = false;
	gameTime = 0;
}

void TimeManager::Finish(float dt){
	
	frames++;
	realTime += App->ms_timer.ReadSec() * dt;

	if (!isPaused) {

		gameTime += App->ms_timer.ReadSec() * dt * speed;
		if (tick) {
			isPaused = false;
			tick = false;
		}
	}


}