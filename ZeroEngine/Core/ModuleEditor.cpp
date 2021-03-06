//Modules
#include "Application.h"
#include "ModuleEditor.h"
#include "ViewportBuffer.h"
#include "PrimitivesGL.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics.h"

//Tools
#include "Globals.h"
#include <string>
#include "ImGui/imgui_internal.h"
#include "ImGuizmo/ImGuizmo.h"
#include <gl/GL.h>

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
   
    show_demo_window = false;
    show_another_window = false;
    show_about_window = false;
    show_conf_window = true;

    show_console_window = true;
    show_hierarchy_window = true;
    show_inspector_window = true;
    show_game_window = true;
    show_scene_window = true;
    show_project_window = true;
    show_reference_window = false;
    show_import_window = false;

    name_correct = false;
    is_cap = false;
    draw = false;
    draw_boundingBox = false;
    drawColliders = true;

    current_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    gameobject_selected = nullptr;
    transform = nullptr;

    extensions.push_back("meta");

    strcpy(sceneName, App->scene->name.c_str());

    //?? Init model texture settings
    modelSettings.cameraImport = true;
    modelSettings.lightImport = true;
    modelSettings.globalScale = 1;

    mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    mCurrentGizmoMode = ImGuizmo::WORLD;

    text = "";
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
    ImGui::StyleColorsCustom();

    // Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);

    LoadIconsImages();

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
    ImGui::Text("Zero Engine v0.3\n");
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
    ImGui::BulletText("PhysX v4.1");

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
    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, .0f);

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
            if (ImGui::MenuItem("Save", "Ctrl + S"))
                App->scene->SaveScene();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "(Alt+F4)")) App->closeEngine = true;
            ImGui::EndMenu();
        }

        /* ---- GAMEOBJECTS ---- */
        if (ImGui::BeginMenu("GameObject")) {

            if (ImGui::MenuItem("Create Empty GameObject")) {
                App->scene->CreateGameObject();
            }

            if (ImGui::MenuItem("Create Empty child")) {
                if (gameobject_selected != nullptr)
                    gameobject_selected->children.push_back(App->scene->CreateGameObject());
                else
                    App->scene->CreateGameObject();
            }

            if (ImGui::BeginMenu("3D Objects")) {
                if (ImGui::MenuItem("Cube")) {
                    //App->geometry->CubeGL();
                }
                if (ImGui::MenuItem("Pyramid")) {
                    //App->geometry->PyramidGL();
                }
                if (ImGui::MenuItem("Sphere")) {
                    //App->geometry->SphereGL();
                }
                if (ImGui::MenuItem("Cylinder")) {
                    //App->geometry->CylinderGL();
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Camera")) {
                GameObject* camera = App->scene->CreateGameObject();
                camera->CreateComponent(ComponentType::CAMERA);
                camera->CreateComponent(ComponentType::TRANSFORM);
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
            if (ImGui::MenuItem("Project")) show_project_window = !show_project_window;
            if (ImGui::MenuItem("Reference Count")) show_reference_window = !show_reference_window;
            
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

        if (ImGui::CollapsingHeader("Camera"))
            ShowCameraConfiguration();

        if(ImGui::CollapsingHeader("Vehicle"))
            ShowVehicleConfiguration();

        if(ImGui::CollapsingHeader("Physics"))
            ShowPhysicsConfiguration();


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

    //Import
    if (show_import_window) {

        ImGui::Begin("Import Settings");

        //Only shows import options depending if we have any file selected to get path and type of import
        if (object_selected.size() > 0) {
            //gameobject_selected = nullptr;
            ImportSettings(object_selected);
        }

        ImGui::End();
    }

    //Hierarchy
    if (show_hierarchy_window) {


        ImGui::Begin("Hierarchy");

        if (ImGui::InputText("Name", sceneName, IM_ARRAYSIZE(sceneName)))
            App->scene->name.assign(sceneName);

        //Just cleaning gameObjects(not textures,buffers...)
        if (ImGui::Button("Clear", { 60,20 })) {
            LOG("Cleaning:  Meshes ||| GameObjects-Childs-Components ||| Textures ||| Buffers");
            App->editor->gameobject_selected = nullptr;
            App->scene->CleanUp(); //Clean GameObjects childs and components
        }

        if (ImGui::Button("Active BoundingBox", { 140,20 }))
            draw_boundingBox = !draw_boundingBox;
       
        for (size_t i = 0; i < App->scene->gameobjects.size(); ++i)
        {
           if (App->scene->gameobjects[i]->parent != nullptr) continue;

           DrawHierarchyChildren(App->scene->gameobjects[i]);
        }

        ImGui::End();
    }

    //Game Window
    if (show_game_window) {
        ImGui::Begin("Game", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
        ImGui::End();
    }

    //Scene Window
    if (show_scene_window) {

        ImGui::Begin("Scene", 0, ImGuiWindowFlags_NoScrollbar);

        ImGui::SameLine((ImGui::GetWindowSize().x / 2)-75);
        if (ImGui::Button("Play", { 35,20 })) {
            
            if (!App->timeManager->started)
            {
                App->scene->SaveScene();
                App->timeManager->Play();
                App->timeManager->started = true;
            }
            else {
                App->timeManager->Finish();
                string scenePath;
                string sceneN(sceneName);
                scenePath = "Assets/Scenes/" + sceneN.append(".ZeroScene");
                gameobject_selected = nullptr;
                App->scene->CleanUp();
                ModelImporter::Load(scenePath.c_str());
                
                App->file_system->Remove(scenePath.c_str());
            }
        }

        ImGui::SameLine();
        char game_time[20] = "";
        sprintf_s(game_time, sizeof(game_time), "%.2f", App->timeManager->GetGameTime());
        ImGui::Text(game_time);
        
        ImGui::SameLine();
        if (ImGui::Button("Pause", { 40 , 20 })) {

            App->timeManager->Pause();

            if (App->timeManager->started) {
                App->timeManager->Resume();
            }

            App->timeManager->started = false;

        }

        // -- Calculate the new size of the texture when window is rescaled
        ImVec2 textureSize = { ImGui::GetWindowSize().x,0 };
        textureSize.y = textureSize.x / App->window->window_aspect_ratio;
       
        window_width = textureSize.x;
        window_height = textureSize.y;
        window_pos.x = ImGui::GetWindowPos().x;
        window_pos.y = ImGui::GetWindowPos().y;
        tab_size.x = ImGui::GetWindowContentRegionMin().x;
        tab_size.y = ImGui::GetWindowContentRegionMin().y;

        // -- Rendering texture info stored from frameBuffer to draw just into scene window
        ImGui::Image((ImTextureID)App->viewport_buffer->texture, ImVec2(textureSize.x, textureSize.y), ImVec2(0, 1), ImVec2(1, 0));
        
        if (gameobject_selected != nullptr)
            EditTransform(dynamic_cast<ComponentTransform*>(gameobject_selected->GetTransform()));

        ImGui::End();
    }

    //Project
    if (show_project_window) {
      
       ImGui::Begin("Project", 0); 
       
       ImGui::BeginChild("Left", { 200,0 }, true);

           if (draw_Folders) {
               assets = App->file_system->GetAllFiles("Assets", nullptr, &extensions);
               if (!drawDobleClick)
                   draw_Folders = !draw_Folders;
           }

          

           DrawAssetsChildren(assets);

           if (ImGui::Button("Delete Asset")) {
               App->resources->DeleteAsset(object_selected.c_str());
               draw_Folders = !draw_Folders;
               object_selected.clear();
           }

           ImGui::EndChild();
       
           
       ImGui::SameLine();
              
       ImGui::BeginChild("Right", { ImGui::GetWindowSize().x - 225, 0 }, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
       assets_size = ImGui::GetWindowSize().x - 255;
       
       if (object_selected.size() > 0)
           DrawFolderChildren(object_selected.c_str());
       else {
           //DrawFolderChildren("Assets");
       }

           ImGui::EndChild();
    
        
       ImGui::End();
   
    }

    //Reference
    if (show_reference_window) {
        ImGui::Begin("Resources");

        ShowResourceCount();

        ImGui::End();
    }

}

void ModuleEditor::DrawAssetsChildren(PathNode node) {

    tmp_flags = base_flags;
    
    if (object_selected == node.path) { 
        tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected; 
    }

    if (node.children.size() == 0) { 
        tmp_flags = tmp_flags | ImGuiTreeNodeFlags_Leaf; 
    }

    if (ImGui::TreeNodeEx(node.localPath.c_str(), tmp_flags)) {


        if (ImGui::IsItemClicked(0))
        {
            object_selected = node.path;
            draw_Folders = !draw_Folders;
        }

        if (node.children.size() > 0) {

            for (size_t i = 0; i < node.children.size(); i++)
            {
                DrawAssetsChildren(node.children[i]);
            }
            
        }
        //LOG("Path: %s", object_selected.c_str());
        ImGui::TreePop();

    }

    
}

void ModuleEditor::DrawFolderChildren(const char* path) {

    string itemPath;

    if (draw_Folders){
        folder = App->file_system->GetAllFiles(path, nullptr, &extensions);
        draw_Folders = false;
        drawDobleClick = false;
    }
    if (folder.isFile == false) {

        int columns = assets_size / 50;
        int iterator = columns;
        ImGui::Columns(columns, false);

        for(int i =0; i<folder.children.size(); i++){
           
            string format = App->resources->GetPathInfo(folder.children[i].path).format;

            ImGui::BeginGroup();

            switch (App->resources->GetTypeByFormat(format))
            {
                case ResourceType::Model:

                    ImGui::Image((ImTextureID)meshIcon->gpu_id, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

                    //Select item and open import options
                    if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
                        show_import_window = !show_import_window;
                        object_selected = folder.children[i].path;
                    }
                    //Open Model if double clicked
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
                        itemPath = App->resources->LoadMetaFile(folder.children[i].path.c_str(), ResourceType::Model);
                        ModelImporter::Load(itemPath.c_str()); 
                    }
                    break;

                case ResourceType::Texture:
                    

                    ImGui::Image((ImTextureID)textureIcon->gpu_id, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
                    
                    //Select item and open import options
                    if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered()) {
                        show_import_window = !show_import_window;
                        object_selected = folder.children[i].path;
                    }
                    //Set texture to gameobject material
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered() && gameobject_selected != nullptr) {
                        object_selected = folder.children[i].path;
                        itemPath = App->resources->LoadMetaFile(object_selected.c_str(), ResourceType::Texture);
                        resourceTexture = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(stoi(itemPath)));
                        dynamic_cast<ComponentMaterial*>(gameobject_selected->GetMaterial())->GetMaterial()->SetDiffuse(resourceTexture);
                    }                     
                    break;

                case ResourceType::Scene:

                    ImGui::Image((ImTextureID)sceneIcon->gpu_id, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
                        gameobject_selected = nullptr;
                        App->scene->CleanUp();
                        ModelImporter::Load(folder.children[i].path.c_str());
                    }

                    break;

                case ResourceType::None:

                    ImGui::Image((ImTextureID)folderIcon->gpu_id, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()){
                        object_selected = folder.children[i].path;
                        drawDobleClick = true;
                        draw_Folders = true;
                    }
                    break;
            }

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text(folder.children[i].localPath.c_str());
                ImGui::EndTooltip();
            }

            string local_name = folder.children[i].localPath;
            local_name.resize(10);
            ImGui::Text(local_name.c_str());
            
            ImGui::EndGroup();

            if (ImGui::GetColumnIndex() == iterator - 1) {
                ImGui::NewLine();
                iterator *= 2;
            }
                
            ImGui::SameLine();
            ImGui::NextColumn();

        }

        ImGui::EndColumns();

    }

}

void ModuleEditor::LoadIconsImages() {

    folderIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Assets/Icons/210543966").name));
    meshIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Assets/Icons/2119381571").name));
    sceneIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Assets/Icons/1349653408").name));
    textureIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Assets/Icons/54850365").name));
   
    TextureImporter::Load("Assets/Icons/210543966", folderIcon, textureSettings);
    TextureImporter::Load("Assets/Icons/2119381571", meshIcon, textureSettings);
    TextureImporter::Load("Assets/Icons/1349653408", sceneIcon, textureSettings);
    TextureImporter::Load("Assets/Icons/54850365", textureIcon, textureSettings);

    //LOG("folderIcon: %u meshIcon: %u", folderIcon, meshIcon);
}

void ModuleEditor::DrawHierarchyChildren(GameObject* gameobject) {

    ImGuiTreeNodeFlags tmp_flags = base_flags;

    if (gameobject == gameobject_selected) { tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected; }

    if (gameobject->children.size() == 0) { tmp_flags = tmp_flags | ImGuiTreeNodeFlags_Leaf; }

    if (ImGui::TreeNodeEx(gameobject->name.c_str(), tmp_flags)) {
        
        if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
        {
           gameobject_selected = gameobject;
        }
        
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("", gameobject, sizeof(gameobject));
            ImGui::Text("%s", gameobject->name.c_str());
            dragged_gameobject = gameobject; // Save the Drag GameObject
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
            {
                gameobject->ReParent(dragged_gameobject, gameobject);   //Re-Assign the child to the new parent
                //WE  NEED TO UPDATE THE TRANSFORM MATRIX TO HAVE THE NEW PARENT MATRIX
                dynamic_cast<ComponentTransform*>(gameobject->GetTransform())->UpdateNodeTransforms();
                dragged_gameobject = nullptr;
            }
            ImGui::EndDragDropTarget();
        }

        //Still Calling Children recursively if they exists
        if (gameobject->children.size() > 0)
        {
            for (int i = 0; i < gameobject->children.size(); ++i)
            {
                DrawHierarchyChildren(gameobject->children[i]);
            }
        }

        ImGui::TreePop();

    }
}

void ModuleEditor::InspectorGameObject() {

    transform = gameobject_selected->GetTransform();
    ComponentMesh* mesh_info = gameobject_selected->GetMesh();
    ComponentMaterial* material_info = gameobject_selected->GetMaterial();
    ComponentCamera* camera_info = gameobject_selected->GetCamera();
    ComponentRigidDynamic* rigidbody_info = gameobject_selected->GetRigidbody();
    ComponentCollider* collider_info = gameobject_selected->GetCollider();
    ComponentDistanceJoint* distancejoint_info = gameobject_selected->GetDistanceJoint();
    ComponentRevoluteJoint* revolutejoint_info = gameobject_selected->GetRevoluteJoint();
    ComponentSliderJoint* sliderjoint_info = gameobject_selected->GetSliderJoint();
    ComponentSphericalJoint* sphericaljoint_info = gameobject_selected->GetSphericalJoint();

    ImGui::Checkbox("Active", &gameobject_selected->active);

    ImGui::Separator();

    //ImGui::Checkbox("Active Bounding Box", &gameobject_selected->draw_boundingBox);
    
    // -- TRANSFORM INTO INSPECTOR -- //
    if (transform != nullptr) {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {


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
            if (ImGui::DragFloat("##Position.X", &transform->position.x)) {
                transform->SetPosition(transform->position);
                transform->UpdateNodeTransforms();

                if (camera_info != nullptr)
                    camera_info->SetPos(transform->position);

            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Position.Y", &transform->position.y)) {
                transform->SetPosition(transform->position);
                transform->UpdateNodeTransforms();

                if (camera_info != nullptr)
                    camera_info->SetPos(transform->position);
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Position.Z", &transform->position.z)) {
                transform->SetPosition(transform->position);
                transform->UpdateNodeTransforms();

                if (camera_info != nullptr)
                    camera_info->SetPos(transform->position);
            }


            //Rotation
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Rotation");
            ImGui::NextColumn();

            if (ImGui::DragFloat("##Rotation.X", &transform->euler.x, 1)) {
                transform->SetRotation(transform->euler);
                transform->UpdateNodeTransforms();

                if (camera_info != nullptr)
                    camera_info->SetRotation();

            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Rotation.Y", &transform->euler.y, 1)) {
                transform->SetRotation(transform->euler);
                transform->UpdateNodeTransforms();

                if (camera_info != nullptr)
                    camera_info->SetRotation();

            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Rotation.Z", &transform->euler.z, 1)) {
                transform->SetRotation(transform->euler);
                transform->UpdateNodeTransforms();

                if (camera_info != nullptr)
                    camera_info->SetRotation();

            }

            //Scale
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Scale");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Scale.X", &transform->scale.x)) {
                transform->SetScale(transform->scale);
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Scale.Y", &transform->scale.y)) {
                transform->SetScale(transform->scale);
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Scale.Z", &transform->scale.z)) {
                transform->SetScale(transform->scale);
                transform->UpdateNodeTransforms();
            }
            ImGui::Separator();

            ImGui::Columns(1);

            if (mCurrentGizmoOperation != ImGuizmo::SCALE)
            {

                ImGui::Text("Guizmo Mode: ");
                ImGui::SameLine();
                if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                    mCurrentGizmoMode = ImGuizmo::LOCAL;
                ImGui::SameLine();
                if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                    mCurrentGizmoMode = ImGuizmo::WORLD;

                ImGui::Separator();
            }
        }
    }

    // -- MESH INTO INSPECTOR -- //
    if (mesh_info != nullptr) {
        if (ImGui::CollapsingHeader("Mesh")) {

            //Enable/disable mesh draw
            ImGui::Checkbox("Active", &mesh_info->draw_mesh);

            //Header
            ImGui::Text("Mesh File: " );
            ImGui::SameLine();

            //File Name
            string name = App->resources->GetPathInfo(mesh_info->ourMesh->GetAssetFile()).name;
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", name.c_str());

            ImGui::Text("Mesh UID: ");
            ImGui::SameLine();

            //Resource UID
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->ourMesh->GetUID());
            
            //Normals
            ImGui::Text("Vertex Normals: ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->ourMesh->num_vertex);

            //Indexs
            ImGui::Text("Indexs: ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->ourMesh->num_index);

            //Vertexs
            ImGui::Text("Vertexs: ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->ourMesh->num_vertex);

            //Uv Coords
            ImGui::Text("UV Coords: ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", mesh_info->ourMesh->num_vertex);
            ImGui::Checkbox("Vertex Normals", &mesh_info->draw_vertexNormals);

        }
    }

    // -- MATERIAL INTO INSPECTOR -- //
    if (material_info != nullptr) {

        if (ImGui::CollapsingHeader("Material")) {

            //File Name
            ImGui::Text("Material File: ");
            ImGui::SameLine();

            //File Name
            string name;
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", material_info->GetMaterial()->assetsFile.c_str());

            //ImGui::Text("Texture UID: ");
            //ImGui::SameLine();

            ////Resource UID
            //ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", material_info->GetMaterial()->diffuse->GetUID());


            ImGui::Checkbox("Active", &material_info->draw_texture);
            ImGui::Checkbox("Checkers", &material_info->draw_checkers);

            ImGui::Image((ImTextureID)material_info->CheckersID, ImVec2(75, 75), ImVec2(0, 0), ImVec2(1, 1));
            
            //Diffuse
            if (material_info->GetMaterial()->diffuse != nullptr) {
                ImGui::Image((ImTextureID)(material_info->GetMaterial()->diffuse->gpu_id), ImVec2(75, 75), ImVec2(0, 0), ImVec2(1, 1));
                
                //If image Imgui item hovered show details and info about texture
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();

                    ImGui::Text("Width: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", material_info->GetMaterial()->diffuse->GetWidth());

                    ImGui::Text("Height: ");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%u", material_info->GetMaterial()->diffuse->GetHeight());

                    ImGui::EndTooltip();
                }
            }
            
            ImGui::ColorEdit4("Color", &material_info->GetMaterial()->materialColor);

            
        }
    }

    // -- CAMERA INTO INSPECTOR -- //
    if (camera_info != nullptr) {

        float near_distance = camera_info->GetNearDistance();
        float far_distance = camera_info->GetFarDistance();
        float fov = camera_info->GetFOV();

        ImGui::Text("Near Distance: ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Near Distance", &near_distance, 1.0f, 1.0f, PX_MAX_F32)) {
            camera_info->SetNearDistance(near_distance);
        }

        ImGui::Text("Far Distance: ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Far Distance", &far_distance, 1.0f, 1.0f, PX_MAX_F32)) {
            camera_info->SetFarDistance(far_distance);
        }

        ImGui::Text("Field Of View: ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Field Of View", &fov, 1.0f, 1.0f, 180.0f)) {
            camera_info->SetFOV(fov);
        }
        //LOG("%f", fov);
        //LOG("%f", camera_info->camera_aspect_ratio);
    }

    // -- RIGIDBODY INTO INSPECTOR -- //
    if (rigidbody_info != nullptr) {     
        if (ImGui::CollapsingHeader("RigidBody")) {
            
            //Show rigidbody info
            if(ImGui::Checkbox("Use Gravity", &rigidbody_info->use_gravity))
                rigidbody_info->EnableGravity(rigidbody_info->use_gravity);

            if (ImGui::Checkbox("Use Kinematic", &rigidbody_info->use_kinematic))
                rigidbody_info->EnableKinematic(rigidbody_info->use_kinematic);

            ImGui::Text("Mass: ");
            if (ImGui::DragFloat("##Mass", &rigidbody_info->mass, 1.0f, 1.0f, PX_MAX_F32))
                rigidbody_info->SetMass(rigidbody_info->mass);
               
            ImGui::Text("Density: ");
            if (ImGui::DragFloat("##Density", &rigidbody_info->density, 0.1f, 0.1f, PX_MAX_F32))
                rigidbody_info->SetDensity(rigidbody_info->density);

            ImGui::Text("Linear Damping: ");
            if (ImGui::DragFloat("##Linear Damping X", &rigidbody_info->linear_damping, 0.1f, 0.0f, PX_MAX_F32))
                rigidbody_info->SetLinearDamping(rigidbody_info->linear_damping);

            ImGui::Text("Angular Damping: ");
            if (ImGui::DragFloat("##Angular Damping X", &rigidbody_info->angular_damping, 0.1f, 0.0f, PX_MAX_F32))
                rigidbody_info->SetAngularDamping(rigidbody_info->angular_damping);

            //Grid
            {
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

                //Force
                ImGui::Separator();
                ImGui::NextColumn();
                ImGui::Text("Force: ");
                ImGui::NextColumn();

                if (ImGui::DragFloat("##Force X", &rigidbody_info->force.x))
                    rigidbody_info->AddForce(rigidbody_info->force);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Force Y", &rigidbody_info->force.y))
                    rigidbody_info->AddForce(rigidbody_info->force);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Force Z", &rigidbody_info->force.z))
                    rigidbody_info->AddForce(rigidbody_info->force);

                //Torque
                ImGui::Separator();
                ImGui::NextColumn();

                ImGui::Text("Torque: ");
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Torque X", &rigidbody_info->torque.x))
                    rigidbody_info->AddTorque(rigidbody_info->torque);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Torque Y", &rigidbody_info->torque.y))
                    rigidbody_info->AddTorque(rigidbody_info->torque);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Torque Z", &rigidbody_info->torque.z))
                    rigidbody_info->AddTorque(rigidbody_info->torque);

                //Linear Velocity
                ImGui::Separator();
                ImGui::NextColumn();

                ImGui::Text("Linear Velocity: ");
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Linear Velocity X", &rigidbody_info->linear_vel.x))
                    rigidbody_info->SetLinearVelocity(rigidbody_info->linear_vel);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Linear Velocity Y", &rigidbody_info->linear_vel.y))
                    rigidbody_info->SetLinearVelocity(rigidbody_info->linear_vel);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Linear Velocity Z", &rigidbody_info->linear_vel.z))
                    rigidbody_info->SetLinearVelocity(rigidbody_info->linear_vel);

                //Angular Velocity
                ImGui::Separator();
                ImGui::NextColumn();
                ImGui::Text("Angular Velocity: ");
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Angular Velocity X", &rigidbody_info->angular_vel.x))
                    rigidbody_info->SetAngularVelocity(rigidbody_info->angular_vel);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Angular Velocity Y", &rigidbody_info->angular_vel.y))
                    rigidbody_info->SetAngularVelocity(rigidbody_info->angular_vel);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##Angular Velocity Z", &rigidbody_info->angular_vel.z))
                    rigidbody_info->SetAngularVelocity(rigidbody_info->angular_vel);

                ImGui::Separator();
                ImGui::Columns(1);
            }

            //Constraints
            ImGui::Text("Lock Linear Axis:");
            ImGui::Text("X:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##X:", &rigidbody_info->lock_linearX))
                rigidbody_info->LockLinearX(rigidbody_info->lock_linearX);
            ImGui::SameLine();
            ImGui::Text("Y:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##Y:", &rigidbody_info->lock_linearY))
                rigidbody_info->LockLinearY(rigidbody_info->lock_linearY);
            ImGui::SameLine();
            ImGui::Text("Z:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##Z:", &rigidbody_info->lock_linearZ))
                rigidbody_info->LockLinearZ(rigidbody_info->lock_linearZ);

            ImGui::Text("Lock Angular Axis:");
            ImGui::Text("X:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##X:", &rigidbody_info->lock_angularX))
                rigidbody_info->LockAngularX(rigidbody_info->lock_angularX);
            ImGui::SameLine();
            ImGui::Text("Y:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##Y:", &rigidbody_info->lock_angularY))
                rigidbody_info->LockAngularY(rigidbody_info->lock_angularY);
            ImGui::SameLine();
            ImGui::Text("Z:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##Z:", &rigidbody_info->lock_angularZ))
                rigidbody_info->LockAngularZ(rigidbody_info->lock_angularZ);
           
        }
    }

    // -- COLLIDER INTO INSPECTOR -- //
    if (collider_info != nullptr) {

        if (ImGui::CollapsingHeader("Collider")) {

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
            ImGui::Text("Center");
            ImGui::NextColumn();

            if (ImGui::DragFloat("##PositionCollider.X", &collider_info->colliderPos.x))
                collider_info->SetPosition(collider_info->colliderPos);
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionCollider.Y", &collider_info->colliderPos.y))
                collider_info->SetPosition(collider_info->colliderPos);
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionCollider.Z", &collider_info->colliderPos.z))
                collider_info->SetPosition(collider_info->colliderPos);

            if (collider_info->type == GeometryType::BOX) {

                //Scale
                ImGui::Separator();
                ImGui::NextColumn();
                ImGui::Text("Size");
                ImGui::NextColumn();

                if (ImGui::DragFloat("##ScaleCollider.X", &collider_info->colliderSize.x, 0.1f, 0.1f, PX_MAX_F32))
                    collider_info->SetScale(collider_info->colliderSize);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##ScaleCollider.Y", &collider_info->colliderSize.y, 0.1f, 0.1f, PX_MAX_F32))
                    collider_info->SetScale(collider_info->colliderSize);
                ImGui::NextColumn();
                if (ImGui::DragFloat("##ScaleCollider.Z", &collider_info->colliderSize.z, 0.1f, 0.1f, PX_MAX_F32))
                    collider_info->SetScale(collider_info->colliderSize);

                ImGui::Separator();
                ImGui::Columns(1);

            }

            else if (collider_info->type == GeometryType::CAPSULE) {
                //Scale
                ImGui::Separator();
                ImGui::Columns(1);

                ImGui::Text("Size");
                if (ImGui::DragFloat("##ScaleCollider.Y", &collider_info->colliderSize.y, 0.1f, 0.1f, PX_MAX_F32))
                    collider_info->SetScale(collider_info->colliderSize, collider_info->colliderRadius);

                ImGui::Text("Radius");
                if (ImGui::DragFloat("##RADIUS", &collider_info->colliderRadius, 0.1f, 0.1f, PX_MAX_F32))
                    collider_info->SetScale(collider_info->colliderSize, collider_info->colliderRadius);
            }

            else if (collider_info->type == GeometryType::SPHERE) {
                ImGui::Separator();
                ImGui::Columns(1);
                ImGui::Text("Radius");
                if (ImGui::DragFloat("##RADIUS", &collider_info->colliderRadius, 0.1f, 0.1f, PX_MAX_F32))
                    collider_info->SetScale({ NULL, NULL, NULL }, collider_info->colliderRadius);
            }


            ImGui::Text("Trigger:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##TRIGGER:", &collider_info->isTrigger))
                collider_info->SetTrigger(collider_info->isTrigger);

            float staticFriction = collider_info->colliderMaterial->getStaticFriction();
            float dynamicFriction = collider_info->colliderMaterial->getDynamicFriction();
            float restitution = collider_info->colliderMaterial->getRestitution();

            //staticFriction
            ImGui::Text("static Friction");

            if (ImGui::DragFloat("##staticFriction", &staticFriction), 0.1f, 0.0f, PX_MAX_F32){
                collider_info->colliderMaterial->setStaticFriction(staticFriction);
                collider_info->colliderShape->setMaterials(&collider_info->colliderMaterial, 1);
            }

            //dynamicFriction
            ImGui::Text("dynamic Friction");

            if (ImGui::DragFloat("##dynamicFriction", &dynamicFriction), 0.1f, 0.0f, PX_MAX_F32) {

                collider_info->colliderMaterial->setDynamicFriction(dynamicFriction);
                collider_info->colliderShape->setMaterials(&collider_info->colliderMaterial, 1);
            }

            //restitution
            ImGui::Text("restitution");
            if(ImGui::DragFloat("##restitution", &restitution), 0.01f, 0.0f, 1.0f){
                collider_info->colliderMaterial->setRestitution(restitution);
                collider_info->colliderShape->setMaterials(&collider_info->colliderMaterial, 1);
            }


            if(ImGui::Button("Delete Collider")) {
                gameobject_selected->DeleteComponent(collider_info);
                delete collider_info;
                collider_info = nullptr;
            }
        }
    }

    // -- DISTANCEJOINT INTO INSPECTOR -- //
    if (distancejoint_info != nullptr) {
        if (ImGui::CollapsingHeader("Distance Joint")) {

            //DRAG AND DROP ANTOHER GAMEOBJECT 
            bool enable = false;
            ImGui::Text("Second Actor:");
            ImGui::SameLine();
            ImGui::Text(text.c_str());
            ImGui::SameLine();
            ImGui::Checkbox("##Second Actor:", &enable);

            if (distancejoint_info->joint == nullptr) {
                
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        distancejoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }
            else {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        distancejoint_info->joint = nullptr;
                        distancejoint_info->actorExtern = nullptr;

                        distancejoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }

            //INFO ABOUT JOINT WHEN EXISTS
            if (distancejoint_info->joint != nullptr) {

                //GRID
                {
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

                    physx::PxVec3 pivotA = distancejoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
                    physx::PxVec3 pivotB = distancejoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

                    //Pivot1
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 1");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotA.X", &pivotA.x))
                        distancejoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Y", &pivotA.y))
                        distancejoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Z", &pivotA.z))
                        distancejoint_info->SetPosition(0, pivotA);

                    //Pivot2
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 2");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotB.X", &pivotB.x))
                        distancejoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Y", &pivotB.y))
                        distancejoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Z", &pivotB.z))
                        distancejoint_info->SetPosition(1, pivotB);

                    ImGui::Separator();
                    ImGui::Columns(1);
                }

                physx::PxReal min_distance = distancejoint_info->joint->getMinDistance(), max_distance = distancejoint_info->joint->getMaxDistance();

                //MIN DISTANCE
                ImGui::Text("Use Min Distance:");
                ImGui::SameLine();
                if (ImGui::Checkbox("##MINENABLE:", &distancejoint_info->min_enable))
                    distancejoint_info->joint->setDistanceJointFlag(PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, distancejoint_info->min_enable);
                
                if (distancejoint_info->min_enable) {
                    ImGui::Text("Min Distance");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##MinDistance", &min_distance, 1.0f, 1.0f, PX_MAX_F32))
                        distancejoint_info->joint->setMinDistance(min_distance);
                }

                //MAX DISTANCE
                ImGui::Text("Use Max Distance:");
                ImGui::SameLine();
                if (ImGui::Checkbox("##MAXENABLE:", &distancejoint_info->max_enable))
                    distancejoint_info->joint->setDistanceJointFlag(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, distancejoint_info->max_enable);

                if (distancejoint_info->max_enable) {
                    ImGui::Text("Max Distance");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##MaxDistance", &max_distance, 0.1f, 0.1f, PX_MAX_F32))
                        distancejoint_info->joint->setMaxDistance(max_distance);
                }

            }

        }
    }

    // -- REVOLUTEJOINT INTO INSPECTOR -- //
    if (revolutejoint_info != nullptr) {
        if (ImGui::CollapsingHeader("Revolute Joint")) {
            //DRAG AND DROP ANTOHER GAMEOBJECT 
            bool enable = false;
            ImGui::Text("Connected Body:");
            ImGui::SameLine();
            ImGui::Text(text.c_str());
            ImGui::SameLine();
            ImGui::Checkbox("##Second Actor:", &enable);

            if (revolutejoint_info->joint == nullptr) {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        revolutejoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }
            else {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        revolutejoint_info->joint = nullptr;
                        revolutejoint_info->actorExtern = nullptr;

                        revolutejoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }
            
            //INFO ABOUT JOINT WHEN EXISTS
            if (revolutejoint_info->joint != nullptr) {

                //GRID
                {
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

                    physx::PxVec3 pivotA = revolutejoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
                    physx::PxVec3 pivotB = revolutejoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

                    //Pivot1
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 1");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotA.X", &pivotA.x))
                        revolutejoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Y", &pivotA.y))
                        revolutejoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Z", &pivotA.z))
                        revolutejoint_info->SetPosition(0, pivotA);

                    //Pivot2
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 2");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotB.X", &pivotB.x))
                        revolutejoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Y", &pivotB.y))
                        revolutejoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Z", &pivotB.z))
                        revolutejoint_info->SetPosition(1, pivotB);

                    ImGui::Separator();
                    ImGui::Columns(1);
                }

                revolutejoint_info->upper_limit = revolutejoint_info->joint->getLimit().upper;
                revolutejoint_info->lower_limit = revolutejoint_info->joint->getLimit().lower;

                //Use Limit
                ImGui::Text("Use Limit:");
                ImGui::SameLine();
                if (ImGui::Checkbox("##UseLimit:", &revolutejoint_info->use_limit))
                    revolutejoint_info->joint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, revolutejoint_info->use_limit);

                if (revolutejoint_info->use_limit) {

                    ImGui::Text("Lower Limit");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##LowerLimit", &revolutejoint_info->lower_limit))
                        revolutejoint_info->joint->setLimit(PxJointAngularLimitPair(revolutejoint_info->lower_limit, revolutejoint_info->upper_limit));

                    ImGui::Text("Upper Limit");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##UpperLimit", &revolutejoint_info->upper_limit))
                        revolutejoint_info->joint->setLimit(PxJointAngularLimitPair(revolutejoint_info->lower_limit, revolutejoint_info->upper_limit));
                }

                //Use Motor
                ImGui::Text("Use Motor:");
                ImGui::SameLine();
                if (ImGui::Checkbox("##UseMotor:", &revolutejoint_info->use_motor))
                    revolutejoint_info->joint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, revolutejoint_info->use_motor);

                if (revolutejoint_info->use_motor) {

                    ImGui::Text("Drive Velocity");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##DriveVelocity", &revolutejoint_info->velocity, 0.1f, 0.0f, PX_MAX_F32))
                        revolutejoint_info->joint->setDriveVelocity(revolutejoint_info->velocity);

                }

                //Enable FreeSpin
                ImGui::Text("Enable Drive FreeSpin:");
                ImGui::SameLine();
                if (ImGui::Checkbox("##EnableFreeSpinr:", &revolutejoint_info->use_freespin))
                    revolutejoint_info->joint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_FREESPIN, revolutejoint_info->use_freespin);

            }
        }    
    }

    // -- SLIDERJOINT INTO INSPECTOR -- //
    if (sliderjoint_info != nullptr) {
        if (ImGui::CollapsingHeader("Prismatic Joint")) {

            //DRAG AND DROP ANTOHER GAMEOBJECT 
            bool enable = false;
            ImGui::Text("Second Actor:");
            ImGui::SameLine();
            ImGui::Text(text.c_str());
            ImGui::SameLine();
            ImGui::Checkbox("##Second Actor:", &enable);

            if (sliderjoint_info->joint == nullptr) {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        sliderjoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }
            else {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        sliderjoint_info->joint = nullptr;
                        sliderjoint_info->actorExtern = nullptr;

                        sliderjoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }

            //INFO ABOUT JOINT WHEN EXISTS
            if (sliderjoint_info->joint != nullptr) {

                //GRID
                {
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

                    physx::PxVec3 pivotA = sliderjoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
                    physx::PxVec3 pivotB = sliderjoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

                    //Pivot1
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 1");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotA.X", &pivotA.x))
                        sliderjoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Y", &pivotA.y))
                        sliderjoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Z", &pivotA.z))
                        sliderjoint_info->SetPosition(0, pivotA);

                    //Pivot2
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 2");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotB.X", &pivotB.x))
                        sliderjoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Y", &pivotB.y))
                        sliderjoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Z", &pivotB.z))
                        sliderjoint_info->SetPosition(1, pivotB);

                    ImGui::Separator();
                    ImGui::Columns(1);
                }

                //sliderjoint_info->lower_limit = sliderjoint_info->joint->getLimit().lower;
                //sliderjoint_info->upper_limit = sliderjoint_info->joint->getLimit().upper;

                ////Use Limit
                //ImGui::Text("Use Limit:");
                //ImGui::SameLine();
                //if (ImGui::Checkbox("##UseLimitSLIDER:", &sliderjoint_info->use_limit))
                //    sliderjoint_info->joint->setPrismaticJointFlag(PxPrismaticJointFlag::eLIMIT_ENABLED, sliderjoint_info->use_limit);

                //if (sliderjoint_info->use_limit) {

                //    ImGui::Text("Lower Limit");
                //    ImGui::SameLine();

                //    if (ImGui::DragFloat("##LowerLimitSLIDER", &sliderjoint_info->lower_limit))
                //        sliderjoint_info->joint->setLimit(PxJointLinearLimitPair(PxTolerancesScale(), sliderjoint_info->lower_limit, sliderjoint_info->upper_limit));

                //    ImGui::Text("Upper Limit");
                //    ImGui::SameLine();

                //    if (ImGui::DragFloat("##UpperLimitSLIDER", &sliderjoint_info->upper_limit))
                //        sliderjoint_info->joint->setLimit(PxJointLinearLimitPair(PxTolerancesScale(), sliderjoint_info->lower_limit, sliderjoint_info->upper_limit));
                //}
            }

        }
    }

    // -- SPHERICALJOINT INTO INSPECTOR -- //
    if (sphericaljoint_info != nullptr) {
        if (ImGui::CollapsingHeader("Spherical Joint")) {

            //DRAG AND DROP ANTOHER GAMEOBJECT 
            bool enable = false;
            ImGui::Text("Second Actor:");
            ImGui::SameLine();
            ImGui::Text(text.c_str());
            ImGui::SameLine();
            ImGui::Checkbox("##Second Actor:", &enable);

            if (sphericaljoint_info->joint == nullptr) {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        sphericaljoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }
            else {

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(""))
                    {
                        sphericaljoint_info->joint = nullptr;
                        sphericaljoint_info->actorExtern = nullptr;

                        sphericaljoint_info->CreateJoint(dragged_gameobject);
                        text = dragged_gameobject->name;
                        dragged_gameobject = nullptr;
                    }
                    ImGui::EndDragDropTarget();
                }

            }
            
            //INFO ABOUT JOINT WHEN EXISTS
            if (sphericaljoint_info->joint != nullptr) {

                //GRID
                {
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

                    physx::PxVec3 pivotA = sphericaljoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR0).p;
                    physx::PxVec3 pivotB = sphericaljoint_info->joint->getLocalPose(physx::PxJointActorIndex::eACTOR1).p;

                    //Pivot1
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 1");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotA.X", &pivotA.x))
                        sphericaljoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Y", &pivotA.y))
                        sphericaljoint_info->SetPosition(0, pivotA);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotA.Z", &pivotA.z))
                        sphericaljoint_info->SetPosition(0, pivotA);

                    //Pivot2
                    ImGui::Separator();
                    ImGui::NextColumn();
                    ImGui::Text("Pivot Actor 2");
                    ImGui::NextColumn();

                    if (ImGui::DragFloat("##PivotB.X", &pivotB.x))
                        sphericaljoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Y", &pivotB.y))
                        sphericaljoint_info->SetPosition(1, pivotB);
                    ImGui::NextColumn();
                    if (ImGui::DragFloat("##PivotB.Z", &pivotB.z))
                        sphericaljoint_info->SetPosition(1, pivotB);

                    ImGui::Separator();
                    ImGui::Columns(1);
                }

                sphericaljoint_info->lower_limit = sphericaljoint_info->joint->getLimitCone().yAngle;
                sphericaljoint_info->upper_limit = sphericaljoint_info->joint->getLimitCone().zAngle;

                //Use Limit
                ImGui::Text("Use Limit:");
                ImGui::SameLine();
                if (ImGui::Checkbox("##UseLimitSPHERICAL:", &sphericaljoint_info->use_limit))
                    sphericaljoint_info->joint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, sphericaljoint_info->use_limit);

                if (sphericaljoint_info->use_limit) {

                    ImGui::Text("Lower Limit");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##LowerLimitSPHERICAL", &sphericaljoint_info->lower_limit))
                        sphericaljoint_info->joint->setLimitCone(PxJointLimitCone(sphericaljoint_info->lower_limit, sphericaljoint_info->upper_limit));

                    ImGui::Text("Upper Limit");
                    ImGui::SameLine();

                    if (ImGui::DragFloat("##UpperLimitSPHERICAL", &sphericaljoint_info->upper_limit))
                        sphericaljoint_info->joint->setLimitCone(PxJointLimitCone(sphericaljoint_info->lower_limit, sphericaljoint_info->upper_limit));
                }
            }

        }
    }

    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("ComponentItem");

    if (ImGui::BeginPopup("ComponentItem", ImGuiWindowFlags_NoScrollbar)) {
        const char* components[] = { "Default", "Box Collider", "Sphere Collider", "Capsule Collider", "Rigidbody", "Distance Joint", "Revolute Joint", "Prismatic Joint", "Spherical Joint" };

        for (size_t i = 0; i < IM_ARRAYSIZE(components); i++)
        {
            if (ImGui::Selectable(components[i])) {

                if (components[i] == "Box Collider")
                    gameobject_selected->CreateComponent(ComponentType::COLLIDER, nullptr, GeometryType::BOX);

                if (components[i] == "Sphere Collider")
                    gameobject_selected->CreateComponent(ComponentType::COLLIDER, nullptr, GeometryType::SPHERE);

                if (components[i] == "Capsule Collider")
                    gameobject_selected->CreateComponent(ComponentType::COLLIDER, nullptr, GeometryType::CAPSULE);

                if(components[i] == "Rigidbody")
                    gameobject_selected->CreateComponent(ComponentType::RIGIDBODY);

                if (components[i] == "Distance Joint")
                    gameobject_selected->CreateComponent(ComponentType::DISTANCE_JOINT);

                if (components[i] == "Revolute Joint")
                    gameobject_selected->CreateComponent(ComponentType::REVOLUTE_JOINT);

                if (components[i] == "Prismatic Joint")
                    gameobject_selected->CreateComponent(ComponentType::PRISMATIC_JOINT);

                if (components[i] == "Spherical Joint")
                    gameobject_selected->CreateComponent(ComponentType::SPHERICAL_JOINT);

                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
}

void ModuleEditor::ImportSettings(string itemSelected) {

    string format = App->resources->GetPathInfo(itemSelected).format;

    switch (App->resources->GetTypeByFormat(format)) {
    case ResourceType::Model: MeshImportOptions(); break;
    case ResourceType::Texture: TextureImportOptions(); break;
    }
}

void ModuleEditor::MeshImportOptions() {

    ImGui::Text("Scene");

    ImGui::Separator();

    ImGui::TextUnformatted("Scale Factor");
    ImGui::SameLine();
    ImGui::InputInt("", &modelSettings.globalScale, 1, 100);
    ImGui::Checkbox("Import Cameras", &modelSettings.cameraImport);
    ImGui::Checkbox("Import Lights", &modelSettings.lightImport);

    ImGui::Button("Import");
}

void ModuleEditor::TextureImportOptions() {

    const char* items[] = { "Repeat", "Mirrored Repeat", "Clamp", "Clamp Border" };
    static int item_current_idx = 0;
    const char* combo_label = items[item_current_idx];

    //Wrap Combo
    if (ImGui::BeginCombo("Wrapping", combo_label, 0)) {

        for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
            const bool is_selected = (item_current_idx == i);
            if (ImGui::Selectable(items[i], is_selected))
                item_current_idx = i;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        
        if (items[item_current_idx] == "Repeat") textureSettings.wrapMode = WrappingMode::Repeat;
        else if (items[item_current_idx] == "Mirrored Repeat") textureSettings.wrapMode = WrappingMode::MirroredRepeat;
        else if (items[item_current_idx] == "Clamp") textureSettings.wrapMode = WrappingMode::Clamp;
        else if (items[item_current_idx] == "Clamp Border") textureSettings.wrapMode = WrappingMode::ClampBorder;

        ImGui::EndCombo();
    }

    const char* filters[] = { "FilterMipMapNearest", "FilterMipMapLinear", "FilterNearest", "FilterLinear" };
    static int filter_current_idx = 0;
    const char* filter_label = filters[filter_current_idx];

    ImGui::Checkbox("MipMap Enable", &textureSettings.enableMipMap);

    //Filter Combo
    if (textureSettings.enableMipMap) {
        if (ImGui::BeginCombo("Filter", filter_label, 0)) {

            for (int i = 0; i < IM_ARRAYSIZE(filters); i++) {
                const bool is_selected = (filter_current_idx == i);
                if (ImGui::Selectable(filters[i], is_selected))
                    filter_current_idx = i;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            if (filters[filter_current_idx] == "FilterMipMapLinear") textureSettings.filterMode = filteringMode::FilterMipMapLinear;
            else if (filters[filter_current_idx] == "FilterMipMapNearest") textureSettings.filterMode = filteringMode::FilterMipMapNearest;
            else if (filters[filter_current_idx] == "FilterLinear") textureSettings.filterMode = filteringMode::FilterLinear;
            else if (filters[filter_current_idx] == "FilterNearest") textureSettings.filterMode = filteringMode::FilterNearest;

            ImGui::EndCombo();
        }
    }

    const char* flips[] = { "FlipNone", "FlipX", "FlipY", "FlipXY" };
    static int flips_current_idx = 0;
    const char* flips_label = flips[flips_current_idx];

    //Flip Combo
    if (ImGui::BeginCombo("Flip", flips_label, 0)) {

        for (int i = 0; i < IM_ARRAYSIZE(flips); i++) {
            const bool is_selected = (flips_current_idx == i);
            if (ImGui::Selectable(flips[i], is_selected))
                flips_current_idx = i;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        if (flips[flips_current_idx] == "FlipNone") textureSettings.flipMode = flipMode::FlipNone;
        else if (flips[flips_current_idx] == "FlipX") textureSettings.flipMode = flipMode::FlipX;
        else if (flips[flips_current_idx] == "FlipY") textureSettings.flipMode = flipMode::FlipY;
        else if (flips[flips_current_idx] == "FlipXY") textureSettings.flipMode = flipMode::FlipXY;

        ImGui::EndCombo();
    }

    if(ImGui::Button("Import")) {
        //IF pressed takes node.child[i]name meta get UID get resource and Load (old resource with new settings) 
        UID resourceUID = stoi(App->resources->LoadMetaFile(object_selected.c_str(), ResourceType::Texture));
        ResourceTexture* resourceToReimport = dynamic_cast<ResourceTexture*>(App->resources->RequestResource(resourceUID));
        TextureImporter::Load(App->resources->SetPathFormated(resourceUID, ResourceType::Texture).c_str(), resourceToReimport, textureSettings);

        char* buffer;
        uint size = TextureImporter::Save(&buffer);
        App->file_system->Save(App->resources->SetPathFormated(resourceUID, ResourceType::Texture).c_str(), buffer, size);
    }
}

int ModuleEditor::ReturnNameObject(std::string path, char buscar) {

    for (int i = path.size(); i >= 0; i--) {
        if (path[i] == buscar) {
            name_correct = true;
            return i + 1;
        }
    }


    return -1;
}

void ModuleEditor::EditTransform(ComponentTransform* transform)
{
    
    ComponentCamera* camera_info = dynamic_cast<ComponentCamera*>(gameobject_selected->GetCamera());
    float4x4 original_Transform = transform->GetGlobalMatrix().Transposed();
    /*float3 pos, scale;
    Quat rot;
    original_Transform.Decompose(pos, rot, scale);
    LOG("POSITION: %f %f %f", pos.x, pos.y, pos.z);
    LOG("ROTATION: %f %f %f", rot.x, rot.y, rot.z, rot.w);
    LOG("SCLAAALE: %f %f %f", scale.x, scale.y, scale.z);*/
    ImGuizmo::Enable(true);

    if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    float4x4 global_matrix_ = transform->GetGlobalMatrix();

    float4x4 temp_matrix = global_matrix_;
   
    ImGuizmo::SetDrawlist();
    float posx = window_pos.x + tab_size.x;
    float posy = window_pos.y + tab_size.y;
    ImGuizmo::SetRect(posx, posy, window_width, window_height);

    ImGuizmo::Manipulate(
        App->camera->editor_camera_info->ViewMatrix().ptr(), 
        App->camera->editor_camera_info->ProjectionMatrix().ptr(), 
        mCurrentGizmoOperation, mCurrentGizmoMode, 
        temp_matrix.ptr()
    );

    if (App->camera->editor_camera_info != nullptr)
        ImGuizmo::ViewManipulate(App->camera->editor_camera_info->ViewMatrix().ptr(), App->camera->editor_camera_info->GetFarDistance(), ImVec2((window_pos.x + window_width) - 100, window_pos.y + 40), ImVec2(100, 100), 0xFFFFFF);

    //COLLIDER OFFSET
    float4x4 matrix_guizmo;
    matrix_guizmo.Set(temp_matrix);
    matrix_guizmo.Transpose();
    
    GetOffsets(matrix_guizmo, original_Transform);

    if (ImGuizmo::IsUsing())
    {
        float4x4 new_transform_matrix;
        new_transform_matrix.Set(temp_matrix);

        //MESH
        new_transform_matrix.Transpose();
        transform->SetTransformMatrix(new_transform_matrix);
        
        //COLLIDER
        //if (gameobject_selected->GetCollider() != nullptr)
            //gameobject_selected->GetCollider()->UpdateValues();

        //CAMERA
        if (camera_info != nullptr)
            camera_info->SetViewMatrix(new_transform_matrix);

        //RIGIDBODY
        //if (gameobject_selected->GetRigidbody() != nullptr)
            //App->physX->WakeUpGeometry(gameobject_selected);

        
    }

}

void ModuleEditor::GetOffsets(float4x4 guizmo_matrix, float4x4 original_matrix) {

    if (!guizmo_matrix.IsIdentity()) {

        //Get Info From original
        float3 pos1, scale1;
        Quat rot1;
        original_matrix.Decompose(pos1, rot1, scale1);
        float3 euler1 = rot1.ToEulerXYZ() * RADTODEG;

        //Get Info From guizmo moved matrix
        float3 pos2, scale2;
        Quat rot2;
        guizmo_matrix.Decompose(pos2, rot2, scale2);
        float3 euler2 = rot2.ToEulerXYZ() * RADTODEG;

        //Substract the difference between original and moved matrix
        float3 pos3, scale3, euler3;
        Quat rot3;

        pos3 = pos2 - pos1;
        euler3 = euler2 - euler1;
        scale3 = scale2 - scale1;

        //Add Offset to the variables
        gameobject_selected->PositionOffset += pos3;
        gameobject_selected->RotationOffset += euler3;
        gameobject_selected->SizeOffset += scale3;
    }

}

void ModuleEditor::ShowResourceCount() {

    if (ImGui::CollapsingHeader("Meshes"))
        FilterResourceType(App->resources->GetResourcesLoaded(), ResourceType::Mesh);

    if (ImGui::CollapsingHeader("Materials"))
        FilterResourceType(App->resources->GetResourcesLoaded(), ResourceType::Material);

    if (ImGui::CollapsingHeader("Textures"))
        FilterResourceType(App->resources->GetResourcesLoaded(), ResourceType::Texture);
}

void ModuleEditor::FilterResourceType(map<UID, Resource*> resources, ResourceType type) {
    
    string path;
    
    for (map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); it++) {


        if (it->second->type == type) {
            path = App->resources->GetPathInfo(it->second->GetLibraryFile()).path;

            ImGui::BeginGroup();
            ImGui::Text("UID - %u", it->second->GetUID());
            ImGui::SameLine();
            ImGui::Text("Using: %i" , it->second->referenceCount);
            ImGui::EndGroup();

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Source Path: %s", it->second->GetAssetFile());
                ImGui::EndTooltip();
            }
        }
    }
}

void ModuleEditor::ShowPhysicsConfiguration() {

    if (ImGui::CollapsingHeader("Gravity Settings", ImGuiWindowFlags_NoCollapse)) {

        ImGui::Text("X: ");
        ImGui::DragFloat("##GravityX", &App->physX->gravity.x);

        ImGui::Text("Y: ");
        ImGui::DragFloat("##GravityY", &App->physX->gravity.y);

        ImGui::Text("Z: ");
        ImGui::DragFloat("##GravityZ", &App->physX->gravity.z);

    }

    if (App->physX->mMaterial != nullptr) {

        float staticFriction = App->physX->mMaterial->getStaticFriction();
        float dynamicFriction = App->physX->mMaterial->getDynamicFriction();
        float restitution = App->physX->mMaterial->getRestitution();

        if (ImGui::CollapsingHeader("Physic Material Settings", ImGuiWindowFlags_NoCollapse)) {

            ImGui::Text("Static Friction: ");
            if (ImGui::DragFloat("##Static Friction", &staticFriction, 1.0f, 0.0f, PX_MAX_F32))
                App->physX->mMaterial->setStaticFriction(staticFriction);

            ImGui::Text("Dynamic Friction: ");
            if (ImGui::DragFloat("##Dynamic Friction", &dynamicFriction, 1.0f, 1.0f, PX_MAX_F32))
                App->physX->mMaterial->setDynamicFriction(dynamicFriction);

            ImGui::Text("Restitution: ");
            if (ImGui::DragFloat("##Restitution", &restitution, 0.01f, 0.0f, 1.0f))
                App->physX->mMaterial->setRestitution(restitution);
        }
    }
    else {
        if (ImGui::Button("Create Material"))
            App->physX->mMaterial = App->physX->CreateMaterial();
    }

    ImGui::Checkbox("Draw Colliders", &drawColliders);
    
}

void ModuleEditor::ShowVehicleConfiguration() {

    if (App->vehicle->gVehicle4W != nullptr) {

        PxRigidDynamic* vehicle = App->vehicle->gVehicle4W->getRigidDynamicActor();

       //Show vehicle info
        if (ImGui::Checkbox("Use Gravity", &App->vehicle->use_gravity))
            vehicle->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !App->vehicle->use_gravity);

        if (ImGui::Checkbox("Use Kinematic", &App->vehicle->use_kinematic))
            vehicle->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, App->vehicle->use_kinematic);

        ImGui::Text("Mass: ");
        if (ImGui::DragFloat("##Mass", &App->vehicle->mass, 1.0f, 1.0f, PX_MAX_F32))
            vehicle->setMass(App->vehicle->mass);

        ImGui::Text("Linear Damping: ");
        if (ImGui::DragFloat("##Linear Damping X", &App->vehicle->linear_damping, 1.0f, 0.0f, PX_MAX_F32))
            vehicle->setLinearDamping(App->vehicle->linear_damping);

        ImGui::Text("Angular Damping: ");
        if (ImGui::DragFloat("##Angular Damping X", &App->vehicle->angular_damping, 1.0f, 0.0f, PX_MAX_F32))
            vehicle->setAngularDamping(App->vehicle->angular_damping);

        ImGui::Separator();

        if (ImGui::Button("Delete Vehicle", { 140,20 })) {
            vehicle->release();
            App->vehicle->gVehicle4W->free();
            App->vehicle->gVehicle4W = nullptr;
            PX_RELEASE(App->vehicle->gGroundPlane);
        }
            
    }
    else {
       
        ImGui::Text("Chassis Options: ");
        ImGui::Separator();

        ImGui::Text("Mass: ");
        ImGui::DragFloat("##ChassisMass", &App->vehicle->mass, 1.0f, 1.0f, PX_MAX_F32);
           
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

        //Dimensions
        ImGui::Separator();
        ImGui::NextColumn();
        ImGui::Text("Dimensions: ");
        ImGui::NextColumn();

        ImGui::DragFloat("##Dimensions X", &App->vehicle->dimensions.x, 1.0f, 1.0f, PX_MAX_F32);
        ImGui::NextColumn();              
        ImGui::DragFloat("##Dimensions Y", &App->vehicle->dimensions.y, 1.0f, 1.0f, PX_MAX_F32);
        ImGui::NextColumn();              
        ImGui::DragFloat("##Dimensions Z", &App->vehicle->dimensions.z, 1.0f, 1.0f, PX_MAX_F32);

        ImGui::Columns(1);

        ImGui::Separator();

        ImGui::Text("Wheels Options: ");
        ImGui::Separator();

        ImGui::Text("Mass: ");
        ImGui::DragFloat("##WheelMass", &App->vehicle->wmass, 1.0f, 1.0f, PX_MAX_F32);

        ImGui::Text("Radius: ");
        ImGui::DragFloat("##WheelRadius", &App->vehicle->wradius, 1.0f, 0.2f, PX_MAX_F32);

        ImGui::Text("Width: ");
        ImGui::DragFloat("##WheelWidth", &App->vehicle->wwidth, 1.0f, 0.1f, PX_MAX_F32);

        ImGui::Separator();

        if (ImGui::Button("Create Vehicle", { 140,20 }))
            App->vehicle->CreateVehicle(App->vehicle->mass, { App->vehicle->dimensions.x, App->vehicle->dimensions.y, App->vehicle->dimensions.z }, App->vehicle->wmass, 
                App->vehicle->wradius, App->vehicle->wwidth);

    }

}

void ModuleEditor::ShowCameraConfiguration() {

    ComponentCamera* camera_info_owner = App->camera->editor_camera_info;
    ComponentCollider* camera_collider = App->camera->editor_camera_collider;
    ComponentTransform* camera_transform = App->camera->editor_camera_transform;
    
    // -- CAMERA TRANSFORM INTO INSPECTOR -- //
    if (camera_transform != nullptr) {
        if (ImGui::TreeNode("Transform")) {


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
            if (ImGui::DragFloat("##Position.X", &camera_transform->position.x)) {

                if (camera_transform != nullptr)
                    camera_info_owner->SetPos(camera_transform->position);

            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Position.Y", &camera_transform->position.y)) {


                if (camera_transform != nullptr)
                    camera_info_owner->SetPos(camera_transform->position);
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Position.Z", &camera_transform->position.z)) {
  
                if (camera_transform != nullptr)
                    camera_info_owner->SetPos(camera_transform->position);
            }


           
            ImGui::Separator();

            ImGui::Columns(1);

            ImGui::TreePop();
        }
    }

    if (ImGui::TreeNode("Camera Settings")) {
        // -- CAMERA INTO INSPECTOR -- //
        if (camera_info_owner != nullptr) {



            float near_distance = camera_info_owner->GetNearDistance();
            float far_distance = camera_info_owner->GetFarDistance();
            float fov = camera_info_owner->GetFOV();

            ImGui::Text("Near Distance: ");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Near Distance", &near_distance, 1.0f, 1.0f, PX_MAX_F32)) {
                camera_info_owner->SetNearDistance(near_distance);
            }

            ImGui::Text("Far Distance: ");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Far Distance", &far_distance, 1.0f, 1.0f, PX_MAX_F32)) {
                camera_info_owner->SetFarDistance(far_distance);
            }

            ImGui::Text("Field Of View: ");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Field Of View", &fov, 1.0f, 1.0f, 180.0f)) {
                camera_info_owner->SetFOV(fov);
            }
        }
        ImGui::TreePop();
    }

    // -- CAMERA COLLIDER INTO INSPECTOR -- //
    if (camera_collider != nullptr) {

        if (ImGui::TreeNode("Collider")) {

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
            ImGui::Text("Center");
            ImGui::NextColumn();

            if (ImGui::DragFloat("##PositionColliderCAMERA.X", &camera_collider->colliderPos.x))
                camera_collider->SetPosition(camera_collider->colliderPos);
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionColliderCAMERA.Y", &camera_collider->colliderPos.y))
                camera_collider->SetPosition(camera_collider->colliderPos);
            ImGui::NextColumn();
            if (ImGui::DragFloat("##PositionColliderCAMERA.Z", &camera_collider->colliderPos.z))
                camera_collider->SetPosition(camera_collider->colliderPos);

            ImGui::Separator();
            ImGui::Columns(1);

            //Scale
            ImGui::Separator();
            ImGui::Text("Radius");

            if (ImGui::DragFloat("##RadiusCollider.X", &camera_collider->colliderRadius, 1.0f, 3.0f, PX_MAX_F32))
                camera_collider->SetScale({ NULL, NULL, NULL }, camera_collider->colliderRadius);
            
            ImGui::Text("Trigger:");
            ImGui::SameLine();
            if (ImGui::Checkbox("##TRIGGERCAMERA:", &camera_collider->isTrigger))
                camera_collider->SetTrigger(camera_collider->isTrigger);

            ImGui::TreePop();
        }
    }
}