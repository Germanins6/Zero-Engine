#include "Application.h"
#include "p2Defs.h"

//External Libs
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"


Application::Application()
{
	PERF_START(ptimer);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	viewport_buffer = new ViewportBuffer(this);
	geometry = new ModuleGeometry(this);
	file_system = new ModuleFileSystem(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(file_system);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(geometry);
	
	// Scenes
	AddModule(viewport_buffer);
	AddModule(scene);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);

	//Control variable to close App
	closeEngine = false;
	vsync = false;
	fps = 0.0f;
	cap = 60;
	capped_ms = -1;

	//config_path = "../ZeroEngine/Config/Settings.JSON";


	PERF_PEEK(ptimer);
}

Application::~Application()
{
	
	for(uint i = list_modules.size(); i <= 0 ; i--)
		delete list_modules[i];
	
	list_modules.clear();

}

bool Application::Init()
{
	PERF_START(ptimer);
	bool ret = true;

	// Call Init() in all modules
	for (size_t i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (size_t i = 0; i < list_modules.size(); i++)
	{
		list_modules[i]->Start();
	}

	
	ms_timer.Start();
	PERF_PEEK(ptimer);

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{

	frame_count++;
	last_sec_frame_count++;

	dt = (float)frame_time.ReadSec();
	frame_time.Start();

}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (cap > 0)
		capped_ms = 1000 / cap;
	else
		capped_ms = -1;
	

	// Framerate calculations --
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		fps = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	uint32 last_frame_ms = frame_time.Read();

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{

	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (size_t i = 0; i < list_modules.size() && ret== UPDATE_CONTINUE; i++)
	{
		list_modules[i]->PreUpdate(dt);
	}

	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		list_modules[i]->Update(dt);
	}


	for (size_t i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		list_modules[i]->PostUpdate(dt);

	}

	// If main menu bar exit button pressed changes closeEngine bool to true and closes App
	if (closeEngine) ret = UPDATE_STOP;

	FinishUpdate();


	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (size_t i = 0; i < list_modules.size() && ret == true; i++)
	{
		list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::LoadJSON(const char* path) {

	//Opening file given by our path to work with data
	ifstream file;
	file.open(path);

	//Verify is our file succesfully opened if not close file
	if (file.is_open())
		LOG("File loaded succesfully")
	else 
		file.close();

	//Info into json object to load into each module
	config_file = json::parse(file);
	file.close();

	for (size_t i = 0; i < list_modules.size(); i++)
		list_modules[i]->Load(config_file);
	
}

void Application::SaveJSON(const char* path) {

	//Output file with json info saved.
	ofstream saved_file{path};

	//Verify is our file succesfully opened if not close file
	if (saved_file.is_open())
		LOG("File loaded succesfully")
	else
		saved_file.close();

	//New json file to save info of each module into
	json aux_JSONFile;

	for (size_t i = 0; i < list_modules.size(); i++)
		list_modules[i]->Save(config_file);

	saved_file.close();
	config_file = aux_JSONFile;
	
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::DrawFPSDiagram() {

	ImGui::InputText("App Name", TITLE, 20);
	ImGui::InputText("Organization", ORGANITZATION, 20);
	ImGui::SliderInt("Framerate", &cap, -1, 120);

	if (fps_log.size() != 30)
	{
		fps_log.push_back(fps);
		ms_log.push_back(dt * 1000);
	}
	else
	{
		fps_log.erase(fps_log.begin());
		fps_log.push_back(fps);
		ms_log.erase(ms_log.begin());
		ms_log.push_back(dt * 1000);
	}

	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	if (ImGui::Checkbox("VSYNC:", &App->renderer3D->vsync_active)) {

		if (App->renderer3D->vsync_active)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

	}

	ImGui::SameLine();
	if (App->renderer3D->vsync_active)ImGui::TextColored(ImVec4(1, 1, 0, 1), "On");
	else { ImGui::TextColored(ImVec4(1, 1, 0, 1), "Off"); }


}

void Application::DrawHardwareConsole() {

	ImGui::Text("SDL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "v2.0.12");

	ImGui::Separator();

	ImGui::Text("CPUs: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

	ImGui::Text("System RAM: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%iGb", SDL_GetSystemRAM() / 1000);

	ImGui::Text("Caps: ");
	ImGui::SameLine();

	if (SDL_Has3DNow() == SDL_TRUE) ImGui::TextColored(ImVec4(1, 1, 0, 1), "3DNow, ");
	ImGui::SameLine();
	if (SDL_HasAVX() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX, ");
	ImGui::SameLine();
	if (SDL_HasAVX2() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX2, ");
	ImGui::SameLine();
	if (SDL_HasAltiVec() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "AltiVec, ");
	ImGui::SameLine();
	if (SDL_HasMMX() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "MMX, ");
	
	if (SDL_HasRDTSC() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "RDTSC, ");
	ImGui::SameLine();
	if (SDL_HasSSE() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE, ");
	ImGui::SameLine();
	if (SDL_HasSSE2() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE2, ");
	ImGui::SameLine();
	if (SDL_HasSSE3() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE3, ");
	ImGui::SameLine();
	if (SDL_HasSSE41() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE41, ");
	ImGui::SameLine();
	if (SDL_HasSSE42() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE42, ");
	
	ImGui::Separator();

	ImGui::Text("GPU: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", glGetString(GL_RENDERER));

	ImGui::Text("Brand: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", glGetString(GL_VENDOR));

}