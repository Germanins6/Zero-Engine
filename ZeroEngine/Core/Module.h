#ifndef _MODULE_H_
#define _MODULE_H_
#include "Globals.h"

#include "JSON/parson.h"

class Application;

class Module
{
private :
	bool enabled;

public:
	Application* App;

	Module(Application* parent, bool start_enabled = true) : App(parent)
	{}

	virtual ~Module()
	{}

	// !!Remember ---> INIT = AWAKE. Previous function called once before Start.
	/*virtual bool Init(JSON_Object* object)
	{
		return true; 
	}*/

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual bool Load(JSON_Object* object) {
		return true;
	}

	virtual bool Save(JSON_Object* object) {
		return true;
	}
};

#endif