#ifndef __TimeManager_H__
#define __TimeManager_H__

#include "Module.h"
#include <vector>

class TimeManager : public Module
{
public:

	TimeManager(Application* app, bool start_enabled = true);
	~TimeManager();

	bool Init();

	inline float GetGameTime() { return gameTime.ReadSec(); };
	inline float GetRealTime() { return engineTime.ReadSec(); };

	inline void Play() { gameTime.Start(); }
	inline void Pause() { gameTime.Stop(); }
	inline void Resume() { gameTime.Resume(); }

public:
	
	bool started;

	Timer gameTime, engineTime;
	float speed;

};

#endif // __TimeManager_H__