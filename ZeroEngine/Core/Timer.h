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
	void Stop();

	bool Started() const;

	Uint32 Read();

	double ReadMs() const;
	uint64 ReadTicks() const;

private:

	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
	Uint32	started_at_perf;
	static uint64 frequency;
};

#endif //__TIMER_H__