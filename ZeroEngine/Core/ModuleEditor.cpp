//Modules
#include "Application.h"
#include "ModuleEditor.h"
#include "ViewportBuffer.h"
#include "PrimitivesGL.h"
#include "ModuleCamera3D.h"

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
    show_conf_window = false;

    show_console_window = true;
    show_hierarchy_window = true;
    show_inspector_window = true;
    show_game_window = true;
    show_scene_window = true;
    show_project_window = true;
    show_reference_window = true;

    name_correct = false;
    is_cap = false;
    draw = false;
    draw_boundingBox = false;

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
    ImGui::Text("Zero Engine v0.2\n");
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
            App->camera->CameraInfo();

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

        //Only shows import options depending if we have any file selected to get path and type of import
        if (object_selected.size() > 0) {
            gameobject_selected = nullptr;
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

        if (ImGui::Button("Delete GameObject", { 80,20 })) {
            std::vector<GameObject*>::iterator toDelete;
            toDelete = std::find(App->scene->gameobjects.begin(), App->scene->gameobjects.end(), gameobject_selected);

            if (toDelete != App->scene->gameobjects.end()) {
                delete* toDelete;
                App->scene->gameobjects.erase(toDelete);
            }
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

    if (show_game_window) {
        ImGui::Begin("Game", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
        ImGui::End();
    }

    if (show_scene_window) {

        ImGui::Begin("Scene", 0, ImGuiWindowFlags_NoScrollbar);

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

    if (show_project_window) {
      
       ImGui::Begin("Project", 0); 
       
       ImGui::BeginChild("Left", { 200,0 }, true);

           if (draw_Folders) {
               assets = App->file_system->GetAllFiles("Assets", nullptr, &extensions);
               //library = App->file_system->GetAllFiles("Library", nullptr, &extensions);
               if (!drawDobleClick)
                   draw_Folders = !draw_Folders;
           }

           DrawAssetsChildren(assets);
           //DrawAssetsChildren(library);

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

    if (show_reference_window) {
        ImGui::Begin("Reference Counting");


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
                    ImGui::ImageButton((ImTextureID)meshIcon->gpu_id, ImVec2(50, 50), ImVec2(0, 1), ImVec2(1, 0));
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
                        App->resources->LoadMetaFile(folder.children[i].path.c_str(), ResourceType::Model);
                     break;
                case ResourceType::Texture:
                    break;
                case ResourceType::Scene:
                    ImGui::ImageButton((ImTextureID)sceneIcon->gpu_id, ImVec2(50, 50), ImVec2(0, 1), ImVec2(1, 0));
                    break;
                case ResourceType::None:
                    ImGui::ImageButton((ImTextureID)folderIcon->gpu_id, ImVec2(50, 50), ImVec2(0, 1), ImVec2(1, 0));
                    if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()){
                        object_selected = folder.children[i].path;
                        drawDobleClick = true;
                        draw_Folders = true;
                    }
                    break;
                default:
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

    folderIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Library/Textures/210543966.dds").name));
    meshIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Library/Textures/2119381571.dds").name));
    sceneIcon = new ResourceTexture(stoi(App->resources->GetPathInfo("Library/Textures/1349653408.dds").name));
   
    TextureImporter::Load("Library/Textures/210543966.dds", folderIcon);
    TextureImporter::Load("Library/Textures/2119381571.dds", meshIcon);
    TextureImporter::Load("Library/Textures/1349653408.dds", sceneIcon);

    //LOG("folderIcon: %u meshIcon: %u", folderIcon, meshIcon);
}

void ModuleEditor::DrawHierarchyChildren(GameObject* gameobject) {

    ImGuiTreeNodeFlags tmp_flags = base_flags;

    if (gameobject == gameobject_selected) { tmp_flags = base_flags | ImGuiTreeNodeFlags_Selected; }

    if (gameobject->children.size() == 0) { tmp_flags = tmp_flags | ImGuiTreeNodeFlags_Leaf; }

    if (ImGui::TreeNodeEx(gameobject->name.c_str(), tmp_flags)) {
        
        if (ImGui::IsItemClicked(0))
        {
           object_selected.clear();
           gameobject_selected = gameobject;
        }
        else if (ImGui::IsItemClicked(1) && ImGui::IsWindowHovered())
        {
            object_selected.clear();
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
                dynamic_cast<ComponentTransform*>(gameobject->GetTransform())->UpdateGlobalMatrix();
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

    transform = dynamic_cast<ComponentTransform*>(gameobject_selected->GetTransform());
    ComponentMesh* mesh_info = dynamic_cast<ComponentMesh*>(gameobject_selected->GetMesh());
    ComponentMaterial* material_info = dynamic_cast<ComponentMaterial*>(gameobject_selected->GetMaterial());
    ComponentCamera* camera_info = dynamic_cast<ComponentCamera*>(gameobject_selected->GetCamera());

    ImGui::Checkbox("Active", &gameobject_selected->active);

    ImGui::Separator();

    //ImGui::Checkbox("Active Bounding Box", &gameobject_selected->draw_boundingBox);
    
    // -- TRANSFORM INTO INSPECTOR -- //
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
            if (ImGui::DragFloat("##Position.X", &transform->position.x)) {
                transform->SetPosition(transform->position.x, transform->position.y, transform->position.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Position.Y", &transform->position.y)) {
                transform->SetPosition(transform->position.x, transform->position.y, transform->position.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Position.Z", &transform->position.z)) {
                transform->SetPosition(transform->position.x, transform->position.y, transform->position.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }


            //Rotation
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Rotation");
            ImGui::NextColumn();
            float3 rot = transform->GetEulerAngles();
            if (ImGui::DragFloat("##Rotation.X", &rot.x, 1, -180, 180)) {
                transform->SetRotation(rot.x, rot.y, rot.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Rotation.Y", &rot.y, 1, -180, 180)) {
                transform->SetRotation(rot.x, rot.y, rot.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Rotation.Z", &rot.z, 1, -180, 180)) {
                transform->SetRotation(rot.x, rot.y, rot.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }

            //Scale
            ImGui::Separator();
            ImGui::NextColumn();
            ImGui::Text("Scale");
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Scale.X", &transform->scale.x)) {
                transform->SetScale(transform->scale.x, transform->scale.y, transform->scale.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Scale.Y", &transform->scale.y)) {
                transform->SetScale(transform->scale.x, transform->scale.y, transform->scale.z);
                transform->UpdateGlobalMatrix();
                transform->UpdateNodeTransforms();
            }
            ImGui::NextColumn();
            if (ImGui::DragFloat("##Scale.Z", &transform->scale.z)) {
                transform->SetScale(transform->scale.x, transform->scale.y, transform->scale.z);
                transform->UpdateGlobalMatrix();
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

            ImGui::TreePop();

        }
    }

    // -- MESH INTO INSPECTOR -- //
    if (mesh_info != nullptr) {
        if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {

            //Enable/disable mesh draw
            ImGui::Checkbox("Active", &mesh_info->draw_mesh);

            //Header
            ImGui::Text("Mesh File: ");
            ImGui::SameLine();

            //File Name
            string name = App->resources->GetPathInfo(mesh_info->ourMesh->GetAssetFile()).name;
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", name.c_str());
            
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

            ImGui::TreePop();
        }
    }

    // -- MATERIAL INTO INSPECTOR -- //
    if (material_info != nullptr) {

        if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {

            //File Name
            ImGui::Text("Texture File: ");
            ImGui::SameLine();

            //File Name
            //string name; --> resourceMesh must return AssetPath
            ///ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", name.c_str());


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

            
            ImGui::TreePop();
        }
    }

    // -- CAMERA INTO INSPECTOR -- //
    if (camera_info != nullptr) {

        float near_distance = camera_info->GetNearDistance();
        float far_distance = camera_info->GetFarDistance();
        float fov = camera_info->GetFOV();

        ImGui::Text("Near Distance: ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Near Distance", &near_distance)) {
            camera_info->SetNearDistance(near_distance);
        }

        ImGui::Text("Far Distance: ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Far Distance", &far_distance)) {
            camera_info->SetFarDistance(far_distance);
        }

        ImGui::Text("Field Of View: ");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Field Of View", &fov)) {
            camera_info->SetFOV(fov);
        }
        LOG("%f", fov);
        LOG("%f", camera_info->camera_aspect_ratio);
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

    const char* items[] = { "Repeat", "Clamp", "Mirror", "Mirror Once", "Per-Axis" };
    static int item_current_idx = 0;
    const char* combo_label = items[item_current_idx];
    if (ImGui::BeginCombo("Wrapping", combo_label, 0)) {

        for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
            const bool is_selected = (item_current_idx == i);
            if (ImGui::Selectable(items[i], is_selected))
                item_current_idx = i;

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        LOG("Current selected %s", items[item_current_idx]);


        ImGui::EndCombo();
    }
    ImGui::Button("Import");
}

    //ImGui::ColorEdit4("Color", (float*)&current_color);
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
    /*ImGuizmo::Enable(true);

    ImGuiIO& io = ImGui::GetIO();

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    /*static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

    if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
       mCurrentGizmoOperation = ImGuizmo::ROTATE;

    if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)     
       mCurrentGizmoOperation = ImGuizmo::SCALE;
   

    if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
    ImGui::SameLine();
    if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;


   /*if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
       mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
   ImGui::SameLine();
   if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
       mCurrentGizmoOperation = ImGuizmo::ROTATE;
   ImGui::SameLine();
   if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
       mCurrentGizmoOperation = ImGuizmo::SCALE;

    math::float4x4 global_matrix_ = transform->GetGlobalMatrix();
    float4x4 view_matrix = App->camera->editor_camera_info->ViewMatrix().Transposed();
    float4x4 projection_matrix = App->camera->editor_camera_info->ProjectionMatrix().Transposed();

    float tempTransform[16];
    memcpy(tempTransform, global_matrix_.ptr(), 16 * sizeof(float));

    ImGuizmo::Manipulate(view_matrix.ptr(), projection_matrix.ptr(), mCurrentGizmoOperation, mCurrentGizmoMode, global_matrix_.ptr());

    /*if (ImGuizmo::IsUsing()) {

        global_matrix_.Transpose();

        if (mCurrentGizmoOperation == ImGuizmo::SCALE) {

            math::Quat des_rot;
            math::float3 des_pos;
            global_matrix_.Decompose(des_pos, des_rot, transform->scale);
            transform->UpdateGlobalMatrix();
        }
    }
   
    if (ImGui::IsKeyPressed(83))
       useSnap = !useSnap;

    ImGui::Checkbox("Snap", &useSnap);
    ImGui::Checkbox("Bound Sizing", &boundSizing);*/
    ImGuizmo::Enable(true);

    if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
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
        ImGuizmo::ViewManipulate(App->camera->editor_camera_info->ViewMatrix().ptr(), App->camera->editor_camera_info->GetFarDistance(), ImVec2((window_pos.x + window_width) - 100, window_pos.y + 20), ImVec2(100, 100), 0xFFFFFF);


    if (ImGuizmo::IsUsing())
    {
        float4x4 new_transform_matrix;
        new_transform_matrix.Set(temp_matrix);
        //LOG("%f %f %f  %f\n %f %f %f %f\n %f %f %f %f", temp_matrix[0][0], temp_matrix[0][1], temp_matrix[0][2], temp_matrix[0][3], temp_matrix[0][4])
        new_transform_matrix.Transpose();
        transform->SetTransformMatrix(new_transform_matrix);
    }
   

}
