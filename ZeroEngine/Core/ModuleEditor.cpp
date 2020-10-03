#include "Application.h"
#include "Globals.h"
#include "ModuleEditor.h"

#include "ImGui/imgui_internal.h"

#include <gl/GL.h>

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
   
    show_demo_window = false;
    show_another_window = false;
    show_about_window = false;
    show_conf_window = false;

}


// Destructor
ModuleEditor::~ModuleEditor()
{}

// Called before render is available
bool ModuleEditor::Start()
{
	bool ret = true;
	
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    

	
    return ret;
}

update_status ModuleEditor::PreUpdate(float dt) {

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    return UPDATE_CONTINUE;

}

// PreUpdate: clear buffer
update_status ModuleEditor::Update(float dt)
{
    
    //Main menu bar
    if (ImGui::BeginMainMenuBar()) {

            //Exit Menu
            if(ImGui::MenuItem("Exit", "(Alt+F4)")) App->closeEngine = true;

            //Examples Menu
            if (ImGui::MenuItem("Examples")) show_demo_window = !show_demo_window;
          
            //Configuration Menu
            if (ImGui::MenuItem("Configuration")) show_conf_window = !show_conf_window;

            //Help Menu
            if (ImGui::BeginMenu("Help")) {

                if (ImGui::MenuItem("Documentation")) ShellExecute(NULL, "open", "https://github.com/Germanins6/Zero-Engine/wiki", NULL, NULL, SW_SHOWNORMAL);
                if (ImGui::MenuItem("Download Latest")) ShellExecute(NULL, "open", "https://github.com/Germanins6/Zero-Engine/releases", NULL, NULL, SW_SHOWNORMAL);
                if (ImGui::MenuItem("Report Issue")) ShellExecute(NULL, "open", "https://github.com/Germanins6/Zero-Engine/issues", NULL, NULL, SW_SHOWNORMAL);
                if (ImGui::MenuItem("About")) show_about_window = !show_about_window;

                ImGui::EndMenu();

            }

    }

    ImGui::EndMainMenuBar();

    //Demo Window with examples
    if(show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    //About Window with information
    if (show_about_window) {
       
        ImGui::Begin("About Zero Engine");

        ImGui::Separator();
        ImGui::Text("Zero Engine v0.0\n");
        ImGui::Separator();

        ImGui::Text("By German Insua & Christian Pi�a\n");
        ImGui::Text("3rd Party Libraries used: ");
        ImGui::BulletText("SDL 2.0.12");
        ImGui::BulletText("Glew 2.0.0");
        ImGui::BulletText("ImGui v1.78");
        ImGui::BulletText("MathGeoLib\n");
        
        ImGui::Separator();
        ImGui::Text("LICENSE\n");
        ImGui::Separator();

        ImGui::Text("MIT License\n\n");
        ImGui::Text("Copyright(c) 2020 Germ�n Insua & Christian Pi�a\n\n");
        ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy\n\nof this software and associated documentation files (the 'Software'), to deal\n");
        ImGui::Text("in the Software without restriction, including without limitation the rights\n\nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell\n");
        ImGui::Text("copies of the Software, and to permit persons to whom the Software is\n\nfurnished to do so, subject to the following conditions : \n");
        ImGui::Text("\n");
        ImGui::Text("The above copyright notice and this permission notice shall be included in all\n\ncopies or substantial portions of the Software.\n");
        ImGui::Text("\n");
        ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n");
        ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
        ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n");
        ImGui::Text("SOFTWARE.\n");

        //ImGui::Separator();


        ImGui::End();

    }

    //Configuration Window
    if (show_conf_window) {

        ImGui::Begin("Configuration");

        if (ImGui::CollapsingHeader("Application"))
        {
            App->DrawFPSDiagram();
            ImGui::Text("%s", TITLE);
            //char title[25];
            //sprintf_s(title, 25, "Framerate %.1f", fps_log[fps_log.size() - 1]);
            //ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
            //sprintf_s(title, 25, "Milliseconds %.1f", ms_log[ms_log.size() - 1]);
            //ImGui::PlotLines("##framerate", &ms_log[0], ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
           
        }

        if (ImGui::CollapsingHeader("Window"))
        {
           
        }

        if (ImGui::CollapsingHeader("Hardware"))
        {
          
        }

        if (ImGui::CollapsingHeader("Render"))
        {
            
        }

        if (ImGui::CollapsingHeader("Input"))
        {
           
        }

        if (ImGui::CollapsingHeader("Audio"))
        {
            
        }

        ImGui::End();
    }

    return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt) {

    // Rendering
    ImGui::Render();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

	return true;
}


