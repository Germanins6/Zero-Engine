#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;

	width = 0;
	height = 0;
	brightness = 1.0f;
	
	//Aspect Ratio of the Window
	window_aspect_ratio = (float)(SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	context = nullptr;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		//Create Window with default size
		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		//Update window  with brigtness value
		SDL_SetWindowBrightness(App->window->window, App->window->brightness);

		
		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			context = SDL_GL_CreateContext(window);
		}
	}


	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::WindowOptions() {
		
	//ImGui::Checkbox("Active", false);

	ImGui::Text("Icon:  *default*");

	if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0.0f, 1.0f)) {
		SDL_SetWindowBrightness(App->window->window, App->window->brightness);
		SDL_UpdateWindowSurface(App->window->window);
	};

	//Value resize window
	if (ImGui::SliderInt("Width", &App->window->width, 0, 1920) || ImGui::SliderInt("Height", &App->window->height, 0, 1080)) {
		SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
		SDL_UpdateWindowSurface(App->window->window);
	};

	//Refresh rate
	ImGui::Text("Refresh Rate: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%f", App->fps);

	//First Checkbox Row [FULLSCREEN AND RESIZABLE]
	if (ImGui::Checkbox("FullScreen", &fullscreen)) {
		if (fullscreen) SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
		else{ SDL_SetWindowFullscreen(App->window->window, flags); }
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable)) {
		if (resizable)SDL_SetWindowResizable(App->window->window, SDL_TRUE);
		else { SDL_SetWindowResizable(App->window->window, SDL_FALSE); }
	}

	//Second Checkbox Row [BORDERLESS AND FULLDESKTOP]
	if (ImGui::Checkbox("Borderless", &borderless)) {
		if (borderless) SDL_SetWindowBordered(App->window->window, SDL_FALSE);
		else { SDL_SetWindowBordered(App->window->window, SDL_TRUE); }
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Full Desktop", &fulldesktop)) {
		if (fulldesktop) SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else{ SDL_SetWindowFullscreen(App->window->window, flags); }
	}
}
