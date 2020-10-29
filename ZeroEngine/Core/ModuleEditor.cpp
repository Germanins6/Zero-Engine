//Modules
#include "Application.h"
#include "ModuleEditor.h"
#include "ViewportBuffer.h"
#include "PrimitivesGL.h"

//Tools
#include "Globals.h"
#include <string>
#include "ImGui/imgui_internal.h"
#include <gl/GL.h>

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
   
    show_demo_window = false;
    show_another_window = false;
    show_about_window = false;
    show_conf_window = false;

    show_console_window = true;
    show_hierarchy_window = true;
    show_inspector_window = true;
    show_game_window = true;
    show_scene_window = true;

    is_cap = false;
    draw = false;

    current_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    gameobject_selected = nullptr;
}


// Destructor
ModuleEditor::~ModuleEditor()
{

    gameobject_selected = nullptr;
    transform = nullptr;
}

bool ModuleEditor::Init() {
    bool ret = true;

    return ret;
}

// Called before render is available
bool ModuleEditor::Start()
{
	bool ret = true;
	
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    scene_window |= ImGuiWindowFlags_NoScrollbar;

    // Setup ImGui style by default
    ImGui::StyleColorsDark();

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
        //Creating MenuBar item as a root for docking windows
        if (DockingRootItem("Viewport", ImGuiWindowFlags_MenuBar)) {
            MenuBar();
            ImGui::End();
        }

        //Update status of each window and shows ImGui elements
        UpdateWindowStatus();

        //Update Input events needed to work with ImGui
        App->input->ImGuiEventInput();

    return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt) {

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Rendering
    ImGui::Render();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    ImGui::EndFrame();

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

void ModuleEditor::About_Window() {

    ImGui::Begin("About Zero Engine");

    ImGui::Separator();
    ImGui::Text("Zero Engine v0.1\n");
    ImGui::Separator();

    ImGui::Text("By German Insua & Christian Piña\n");
    ImGui::Text("3rd Party Libraries used: ");
    ImGui::BulletText("SDL v2.0.12");
    ImGui::BulletText("Glew v2.1.0");
    ImGui::BulletText("OpenGL v3.1.0");
    ImGui::BulletText("ImGui v1.78");
    ImGui::BulletText("MathGeoLib v1.5");
    ImGui::BulletText("PhysFS v3.0.2");
    ImGui::BulletText("DevIL v1.7.8");
    ImGui::BulletText("Assimp v3.1.1");

    ImGui::Separator();
    ImGui::Text("LICENSE\n");
    ImGui::Separator();

    ImGui::Text("MIT License\n\n");
    ImGui::Text("Copyright(c) 2020 Germán Insua & Christian Piña\n\n");
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

void ModuleEditor::UpdateText(const char* consoleText) {
    console_text.appendf(consoleText);
}

bool ModuleEditor::DockingRootItem(char* id, ImGuiWindowFlags winFlags)
{
    //Setting windows as viewport size
    ImGuiViewport* viewport = ImGui::GetWindowViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    //Setting window style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);

    //Viewport window flags just to have a non interactable white space where we can dock the rest of windows
    winFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar;
    winFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

    bool temp = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    temp = ImGui::Begin(id, &temp, winFlags);
    ImGui::PopStyleVar(3);

    BeginDock(id, ImGuiDockNodeFlags_PassthruCentralNode);

    return temp;
}

void ModuleEditor::BeginDock(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size)
{
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dock = ImGui::GetID(dockSpaceId);
        ImGui::DockSpace(dock, size, dockFlags);
    }
}

void ModuleEditor::MenuBar() {

    /* ---- MAIN MENU BAR DOCKED ----*/
    if (ImGui::BeginMainMenuBar()) {

        /* ---- FILE ---- */
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl + S")); //DO SOMETHING
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "(Alt+F4)")) App->closeEngine = true;
            ImGui::EndMenu();
        }

        /* ---- GAMEOBJECTS ---- */
        if (ImGui::BeginMenu("GameObject")) {

            if (ImGui::MenuItem("Create empty GameObject")) {
                App->scene->CreateGameObject();
            }

            if (ImGui::BeginMenu("3D Objects")) {
                if (ImGui::MenuItem("Cube")) {
                    App->geometry->CubeGL();
                }
                if (ImGui::MenuItem("Pyramid")) {
                    App->geometry->PyramidGL();
                }
                if (ImGui::MenuItem("Sphere")) {
                    App->geometry->SphereGL();
                }
                if (ImGui::MenuItem("Cylinder")) {
                    App->geometry->CylinderGL();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }


        /* ---- WINDOW ----*/
        if (ImGui::BeginMenu("Window")) {

            if (ImGui::MenuItem("Examples")) show_demo_window = !show_demo_window;
            ImGui::Separator();

            if (ImGui::BeginMenu("Workspace Style")) {
                if (ImGui::MenuItem("Dark")) ImGui::StyleColorsDark();
                if (ImGui::MenuItem("Classic")) ImGui::StyleColorsClassic();
                if (ImGui::MenuItem("Light")) ImGui::StyleColorsLight();
                if (ImGui::MenuItem("Custom")) ImGui::StyleColorsCustom();
                ImGui::EndMenu();
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Hierarchy")) show_hierarchy_window = !show_hierarchy_window;
            if (ImGui::MenuItem("Inspector")) show_inspector_window = !show_inspector_window;
            if (ImGui::MenuItem("Scene")) show_scene_window = !show_scene_window;
            if (ImGui::MenuItem("Game")) show_game_window = !show_game_window;
            if (ImGui::MenuItem("Console")) show_console_window = !show_console_window;

            ImGui::Separator();
            if (ImGui::MenuItem("Configuration")) show_conf_window = !show_conf_window;
            

            ImGui::EndMenu();
        }

        /* ---- HELP ----*/
        if (ImGui::BeginMenu("Help")) {

            if (ImGui::MenuItem("Documentation")) ShellExecute(NULL, "open", "https://github.com/Germanins6/Zero-Engine/wiki", NULL, NULL, SW_SHOWNORMAL);
            if (ImGui::MenuItem("Download Latest")) ShellExecute(NULL, "open", "https://github.com/Germanins6/Zero-Engine/releases", NULL, NULL, SW_SHOWNORMAL);
            if (ImGui::MenuItem("Report Issue")) ShellExecute(NULL, "open", "https://github.com/Germanins6/Zero-Engine/issues", NULL, NULL, SW_SHOWNORMAL);
            if (ImGui::MenuItem("About")) show_about_window = !show_about_window;
            ImGui::EndMenu();
        }

    }

    ImGui::EndMainMenuBar();
}

void ModuleEditor::UpdateWindowStatus() {

    //Demo
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    //About info
    if (show_about_window)  About_Window();

    //Config
    if (show_conf_window) {

        ImGui::Begin("Configuration");

        if (ImGui::CollapsingHeader("Application"))
            App->DrawFPSDiagram();


        if (ImGui::CollapsingHeader("Window"))
            App->window->WindowOptions();


        if (ImGui::CollapsingHeader("Hardware"))
            App->DrawHardwareConsole();


        if (ImGui::CollapsingHeader("Render"))
            App->renderer3D->VSYNC_();

        if (ImGui::CollapsingHeader("Input"))
            App->input->InputInfo();



        ImGui::End();

    }

    //Console
    if (show_console_window) {

        ImGui::Begin("Console");
        ImGui::TextUnformatted(console_text.begin(), console_text.end());
        ImGui::SetScrollHere(1.0f);
        ImGui::End();
    }

    //Inspector
    if (show_inspector_window) {

        ImGui::Begin("Inspector");
        //Only shows info if any gameobject selected
        if (gameobject_selected != nullptr) 
            InspectorGameObject(); 

        ImGui::End();

    }

    //Hierarchy
    if (show_hierarchy_window) {


        ImGui::Begin("Hierarchy");

        //Just cleaning gameObjects(not textures,buffers...)
        if (ImGui::Button("Clear", { 60,20 })) App->scene->CleanUp();


        for (size_t i = 0; i < App->scene->gameobjects.size(); i++)
        {
            string name = App->scene->gameobjects[i]->name;

            //If our current gameobject does have any parent we continue in for cicle to next iterator.
            if (App->scene->gameobjects[i]->parent != nullptr) continue;
            
            //If we dont have any parent and passed before filter we continue creating tree node just with roots or individual elements.
            if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf)) {

                if (ImGui::IsItemClicked()) {
                    gameobject_selected = App->scene->gameobjects[i];
                    LOG("Game Object Selected name: %s", gameobject_selected->name.c_str());
                }

                //If we detect any gameObject with childs we root
                if (App->scene->gameobjects[i]->children.size() > 0) {
                    for (size_t j = 0; j < App->scene->gameobjects[i]->children.size(); j++)
                    {
                        if (ImGui::TreeNodeEx(App->scene->gameobjects[i]->children[j]->name.c_str(), ImGuiTreeNodeFlags_Leaf)) {

                            if (ImGui::IsItemClicked()) {
                                gameobject_selected = App->scene->gameobjects[i]->children[j];
                                LOG("Game Object Selected name: %s", gameobject_selected->name.c_str());
                            }

                            ImGui::TreePop();
                        }
                    }
                }

                ImGui::TreePop();
            }
        }
           
        ImGui::End();
    }

    if (show_game_window) {
        ImGui::Begin("Game", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
        ImGui::End();
    }

    if (show_scene_window) {
        ImGui::Begin("Scene", 0 , ImGuiWindowFlags_NoScrollbar);
        /*if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Shading Mode")) {
                ImGui::MenuItem("Shaded");
                ImGui::MenuItem("Wireframe");
                ImGui::MenuItem("Shaded Wireframe");
                ImGui::EndMenu();
            }
        }

        ImGui::EndMenuBar();*/
        ImGui::Image((ImTextureID)App->viewport_buffer->texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }

    
}

void ModuleEditor::InspectorGameObject() {

    transform = dynamic_cast<ComponentTransform*>(gameobject_selected->GetTransform());
    if (transform != nullptr) {
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {


            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            ImGui::Columns(4, NULL, true);

            //Title Names
            ImGui::Separator();
            ImGui::Text("");
            ImGui::NextColumn();
            ImGui::Text("X");
            ImGui::NextColumn();
            ImGui::Text("Y");
            ImGui::NextColumn();
            ImGui::Text("Z");

            //Position
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Position");
            ImGui::NextColumn();
            ImGui::DragFloat("##Position.X", &transform->position.x);
            ImGui::NextColumn();
            ImGui::DragFloat("##Position.Y", &transform->position.y);
            ImGui::NextColumn();
            ImGui::DragFloat("##Position.Z", &transform->position.z);

            //Rotation
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Rotation");
            ImGui::NextColumn();
            ImGui::DragFloat("##Rotation.X", &transform->rotation.x);
            ImGui::NextColumn();
            ImGui::DragFloat("##Rotation.Y", &transform->rotation.y);
            ImGui::NextColumn();
            ImGui::DragFloat("##Rotation.Z", &transform->rotation.z);

            //Scale
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Scale");
            ImGui::NextColumn();
            ImGui::DragFloat("##Scale.X", &transform->scale.x);
            ImGui::NextColumn();
            ImGui::DragFloat("##Scale.Y", &transform->scale.y);
            ImGui::NextColumn();
            ImGui::DragFloat("##Scale.Z", &transform->scale.z);
            ImGui::Separator();

            ImGui::Columns(1);

            ImGui::TreePop();

        }
    }

    ComponentMesh* mesh_info = dynamic_cast<ComponentMesh*>(gameobject_selected->GetMesh());
    
    if (mesh_info != nullptr) {

        if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::Checkbox("Active", &mesh_info->draw_mesh);

                //File Name
                if(mesh_info->mesh->type == PrimitiveTypesGL::PrimitiveGL_NONE){
                    
                    ImGui::Text("Mesh File: ");
                    ImGui::SameLine();
                
                    //File Name
                     //Set the character we want to found
                    string mesh_path(mesh_info->path_info);
                 //   string name = mesh_path.substr(ReturnNameObject(mesh_path, 0x5c));
                    //ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", name.c_str());

                }

                //Normals
                ImGui::Text("Vertex Normals: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->mesh->num_vertex);

                //Indexs
                ImGui::Text("Indexs: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->mesh->num_index);

                //Vertexs
                ImGui::Text("Vertexs: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->mesh->num_vertex);
                //Faces
                //ImGui::Text("Faces: ", &mesh_info->mesh->num_vertex);
                //ImGui::SameLine();

                //Uv Coords
                ImGui::Text("UV Coords: ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->mesh->num_vertex);

                if(mesh_info->mesh->type==PrimitiveTypesGL::PrimitiveGL_NONE){
                    ImGui::Checkbox("Vertex Normals", &mesh_info->draw_vertexNormals);
                    ImGui::Checkbox("Face Normals", &mesh_info->draw_faceNormals);
                }

                ImGui::TreePop();

        }
        
        if(mesh_info->mesh->type == PrimitiveTypesGL::PrimitiveGL_NONE){

            if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {

                if (mesh_info->mesh->tex_info != nullptr) {
                    ComponentMaterial* texture_info = dynamic_cast<ComponentMaterial*>(gameobject_selected->GetMaterial());
           
                    //File Name
                    ImGui::Text("Texture File: ");
                    ImGui::SameLine();

                    //File Name
                    //Set the character we want to found

                    //FIX
                    /*string texture_path(texture_info->texture_path);
                    string name = texture_path.substr(ReturnNameObject(texture_path, 0x5c));

                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", name.c_str());*/

                    ImGui::Text("Width: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", texture_info->TextureData->GetWidth());

                    ImGui::Text("Height: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", texture_info->TextureData->GetHeight());
                
                    ImGui::Checkbox("Active", &mesh_info->mesh->draw_texture);
                    ImGui::Checkbox("Checkers", &mesh_info->mesh->draw_checkers);

                }
            
                if (mesh_info->mesh->tex_info != nullptr) {
                    ImGui::ImageButton((ImTextureID)mesh_info->mesh->textureID, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), 2);
                    ImGui::ImageButton((ImTextureID)(mesh_info->mesh->tex_info->id), ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), 2);
                }
                else {

                    //File Name
                    ImGui::Text("Texture File: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Drag a Texture to Set in Mesh");

                    ImGui::Text("Width: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", 0);

                    ImGui::Text("Height: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", 0);

                    ImGui::Checkbox("Checkers", &mesh_info->mesh->draw_checkers);
                    ImGui::ImageButton((ImTextureID)mesh_info->mesh->textureID, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), 2);
                    ImGui::ImageButton(NULL, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), 2);
                }

                //ImGui::DragInt("##columns_count", &columns_count, 0.1f, 2, 10, "%d columns");

                ImGui::TreePop();

            }
        }
    }

    
}

    //ImGui::ColorEdit4("Color", (float*)&current_color);
int ModuleEditor::ReturnNameObject(std::string path, char buscar) {

    for (int i = path.size(); i >= 0; i--) {
        if (path[i] == buscar)
            return i + 1;
    }

    return -1;
}