#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include <vector>

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool Init();
	bool CleanUp();

};

#endif // __ModuleAudio_H__