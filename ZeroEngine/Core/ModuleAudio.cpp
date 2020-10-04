#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init()
{

	return true;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	
	return true;
}