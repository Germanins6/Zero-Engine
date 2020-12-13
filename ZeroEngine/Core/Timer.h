#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "p2Defs.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void Resume();
	void Stop();


	Uint32 Read();
	float ReadSec() const;

public:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
};

#endif //__TIMER_H__