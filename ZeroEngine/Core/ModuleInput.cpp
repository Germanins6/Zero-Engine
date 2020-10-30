#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleGeometry.h"
#include "GameObject.h"
#include "ModuleTextures.h"
#include "ImGui/imgui_internal.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	file_path = nullptr;
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			//if (mouse_z < 0)App->camera->Position *= 1.2f;
			//else if (mouse_z > 0)App->camera->Position /= 1.2f;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);

				if (e.window.event == SDL_WINDOWEVENT_CLOSE)   // exit game
					App->closeEngine = true;

				break;
			}
			break;

			case SDL_DROPFILE:
			{
				file_path = e.drop.file;
				std::string file_name(file_path);
				if (file_name.substr(file_name.find_last_of(".")) == ".fbx" || file_name.substr(file_name.find_last_of(".")) == ".FBX" || file_name.substr(file_name.find_last_of(".")) == ".OBJ" || file_name.substr(file_name.find_last_of(".")) == ".obj") {
					LOG("Path of file dropped will be %s", file_path);
					App->geometry->LoadGeometry(file_path);
				}
				else if (file_name.substr(file_name.find_last_of(".")) == ".jpg" || file_name.substr(file_name.find_last_of(".")) == ".png" || file_name.substr(file_name.find_last_of(".")) == ".PNG" || file_name.substr(file_name.find_last_of(".")) == ".JPG") {
					LOG("Path of file dropped will be %s", file_path);

					//If drag and drop an image and a GO selected create material and all info
					if (App->editor->gameobject_selected != nullptr && App->editor->gameobject_selected->GetMesh() != nullptr) {

						//If we have any current material filled we just update info, on the other hand if isnt filled its because we dont have any material component so our func return nullptr
						if (App->editor->gameobject_selected->GetMaterial() != nullptr)
							dynamic_cast<ComponentMaterial*>(App->editor->gameobject_selected->GetMaterial())->UpdateTextureInfo(file_path);
						else 
							App->editor->gameobject_selected->CreateComponent(ComponentType::MATERIAL, file_path);
					}
				}
			};
			SDL_free(&file_path);
			break;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::ImGuiEventInput() {
	/*SDL_Event event;
	while (SDL_PollEvent(&event))
		ImGui_ImplSDL2_ProcessEvent(&event);*/
	
}

void ModuleInput::InputInfo() {
	ImGui::Text("Mouse Position");

	ImGui::Text("X: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_x);
	ImGui::SameLine();
	ImGui::Text("Y: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_y);

}