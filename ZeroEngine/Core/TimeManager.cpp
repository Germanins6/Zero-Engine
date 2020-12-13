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
