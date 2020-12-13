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
	LOG("Init Time Manager");

	engineTime.Start();
	speed = 1.0f;
	started = false;

	Pause();

	return true;
}


void TimeManager::Finish() {

	gameTime.stopped_at = gameTime.started_at = SDL_GetTicks();
	gameTime.running = false;
	App->timeManager->started = false;

}

update_status TimeManager::Update(float dt) {


	return update_status::UPDATE_CONTINUE;
}