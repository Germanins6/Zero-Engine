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
	bool CleanUp();

	inline float GetGameTime() { return gameTime / 1000; }
	inline float GetRealTime() { return realTime / 1000; }

	inline void Play() { isPaused = false; }
	inline void Pause() { isPaused = true; }

	void StartGame();
	void Finish(float dt);

public:

	bool isPaused;
	bool tick;

	float realTime, gameTime;
	float speed;
	uint frames;

};

#endif // __TimeManager_H__