// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

uint64 Timer::frequency = 0;

// ---------------------------------------------
Timer::Timer()
{
	if (frequency == 0)
		frequency = SDL_GetPerformanceFrequency();

	Start();
	running = false;
}

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
	started_at_perf = SDL_GetPerformanceCounter();
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at / 1000.0f;
	}
	else
	{
		return stopped_at - started_at;
	}
}

bool Timer::Started() const {
	return running;
}

uint64 Timer::ReadTicks() const
{
	return SDL_GetPerformanceCounter() - started_at_perf;
}

double Timer::ReadMs() const
{
	return 1000.0 * (double(SDL_GetPerformanceCounter() - started_at_perf) / double(frequency));
}


