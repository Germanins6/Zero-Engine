#ifndef __ModuleFileSystem_H__
#define __ModuleFileSystem_H__

#include "Module.h"

#include "PhysFS/include/physfs.h"
#include "SDL/include/SDL.h"

#pragma comment( lib, "Core/PhysFS/libx86/physfs.lib" )

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();

	// Called before quitting
	bool CleanUp();

	// Return the bytes of a PhysFS filehandle
	uint Load(const char* path, char** buffer) const;

	// Allows you to use pointers to memory instead of files or things such as images or samples
	SDL_RWops* Load(const char* path) const;

};

#endif // __ModuleFileSystem_H__
