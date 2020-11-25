#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ImGuizmo/ImGuizmo.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	Position = float3(10.0f, 10.0f, 0.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
	
	//Initializa the Components to acces fast
	editor_camera_info = new ComponentCamera(nullptr);
	editor_camera_transform = new ComponentTransform(nullptr);

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	//Set Initial Camera Propierties
	editor_camera_info->SetPos(Position);
	editor_camera_info->SetReference(Reference);
	editor_camera_info->LookAt(Reference);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");
	RELEASE(editor_camera_info);
	//RELEASE(scene_camera_info);

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!
	editor_camera_info->cull = false;
	editor_camera_info->draw = false;
	editor_camera_info->Update(dt);
	
	float3 newPos(0,0,0);
	float speed = 12.0f * dt;
	
	Move(newPos, speed, dt);
	Mouse(newPos, speed, dt);
	
	MousePicking();

	if (editor_camera_transform != nullptr) {
		
		Position += newPos;
		editor_camera_info->SetPos(Position);
		editor_camera_transform->position = Position;
		Reference += newPos;
		editor_camera_info->SetReference(Reference);

		/*editor_camera_info->LookAt(editor_camera_info->new_reference); 
		ComponentTransform* transform = dynamic_cast<ComponentTransform*>(camera->GetTransform());
		transform->position += newPos;
		editor_camera_info->new_reference += newPos;
		editor_camera_info->SetPos(transform->position);
		transform->SetPosition(transform->position.x, transform->position.y, transform->position.z);
		transform->UpdateGlobalMatrix();
		transform->UpdateNodeTransforms();*/
		
	}
	
	return UPDATE_CONTINUE;
}

// Camera Movement
void ModuleCamera3D::Move(float3& move, float speed, float dt) {

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = 24.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) move.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) move.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) move += editor_camera_info->frustum.front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) move -= editor_camera_info->frustum.front * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) move -= editor_camera_info->frustum.WorldRight() * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) move += editor_camera_info->frustum.WorldRight() * speed;

	if (App->editor->transform != nullptr) {

		//Focus
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
			if (App->editor->gameobject_selected != nullptr) {
				editor_camera_info->LookAt({ App->editor->transform->position.x, App->editor->transform->position.y, App->editor->transform->position.z });
			}
		}

		//Rotate Around Object
		if (App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT) {
			if (App->editor->gameobject_selected != nullptr) {
				editor_camera_info->LookAt({ App->editor->transform->position.x, App->editor->transform->position.y, App->editor->transform->position.z });
				move -= editor_camera_info->frustum.WorldRight();
			}
		}
	}

}

// Camera Orbit
void ModuleCamera3D::Mouse(float3& move, float speed, float dt) {

	// Mouse motion ----------------
	if (App->input->GetMouseZ() > 0) move -= editor_camera_info->frustum.front * speed * 2;
	if (App->input->GetMouseZ() < 0) move += editor_camera_info->frustum.front * speed * 2;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();
		float Sensitivity = 0.25f;

		//Orbital camera, if right click pressed and alt at same time we check if we have any viewport gameObject selected to get position and orbit camera around GO.
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
			if (App->editor->gameobject_selected != nullptr) {
				Reference = dynamic_cast<ComponentTransform*>(App->editor->gameobject_selected->GetTransform())->position;
			}
		}
		
		if (dx != 0.0f)
		{
			math::Quat rot_quatx = math::Quat::RotateY(dx * dt * Sensitivity);
			editor_camera_info->frustum.front = rot_quatx.Mul(editor_camera_info->frustum.front).Normalized();
			editor_camera_info->frustum.up = rot_quatx.Mul(editor_camera_info->frustum.up).Normalized();

		}

		if (dy != 0.0f)
		{
			math::Quat rot_quaty = math::Quat::RotateAxisAngle(editor_camera_info->frustum.WorldRight(), dy * dt * Sensitivity);
			math::float3 up = rot_quaty.Mul(editor_camera_info->frustum.up).Normalized();

			if (up.y > 0.0f) {

				editor_camera_info->frustum.up = up;
				editor_camera_info->frustum.front = rot_quaty.Mul(editor_camera_info->frustum.front).Normalized();

			}

		}

	}

}

//=============MOUSE PICKING==================//
void ModuleCamera3D::MousePicking() {

	float dx = ((float)App->input->GetMouseX() - ((float)App->editor->window_pos.x + (float)App->editor->tab_size.x)) / (float)App->editor->window_width;
	float dy = ((float)App->input->GetMouseY() - ((float)App->editor->window_pos.y + (float)App->editor->tab_size.y)) / (float)App->editor->window_height;

	dx = (dx - 0.5f) * 2.0f;
	dy = -(dy - 0.5f) * 2.0f;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && dx <= 1 && dx >= -1 && dy <= 1 && dy >= -1 && !ImGuizmo::IsOver()) {

		//LOG("X: %f  Y: %f", dx, dy);

		std::map<float, GameObject*> gameObject_hit_list;
		std::vector<GameObject*> gameObject_list = App->scene->gameobjects;

		LineSegment picking = editor_camera_info->frustum.UnProjectLineSegment(dx, dy);

		App->renderer3D->ray_cast = picking;

		//Look all gameObjects to see if one is hit
		for (size_t i = 0; i < gameObject_list.size(); i++)
		{
			if (gameObject_list[i]->GetMesh() != nullptr) {

				//Look if the raycast intersect with the Bounding Box of the GameObject
				bool hit = picking.Intersects(dynamic_cast<ComponentMesh*>(gameObject_list[i]->GetMesh())->mesh->GetAABB());

				//If we hit add the gameObject to the hit list
				if (hit) {
					float dNear, dFar;
					hit = picking.Intersects(dynamic_cast<ComponentMesh*>(gameObject_list[i]->GetMesh())->mesh->GetAABB(), dNear, dFar);
					gameObject_hit_list[dNear] = gameObject_list[i];
				}

			}

		}

		std::map<float, GameObject*>::iterator it = gameObject_hit_list.begin();

		while (it != gameObject_hit_list.end())
		{
			//Create Local Raycast
			LineSegment ray_local_space = picking;
			GameObject* gameObject_hit = it->second;

			//Components of the gameObject
			ComponentMesh* mesh_info = dynamic_cast<ComponentMesh*>(gameObject_hit->GetMesh());
			ComponentTransform* gameObject_hit_transform = dynamic_cast<ComponentTransform*>(gameObject_hit->GetTransform());

			//Transform once the ray into GameObject Space to test against all triangles
			ray_local_space.Transform(gameObject_hit_transform->GetGlobalMatrix().Transposed().Inverted());

			//Look all the triagnles of the mesh to look if we hit
			for (size_t i = 0; i < mesh_info->mesh->num_index; i += 3)
			{

				//Calculate Pos of Vertex of the Triangle
				float3 x = { mesh_info->mesh->vertex[mesh_info->mesh->index[i] * 3], mesh_info->mesh->vertex[mesh_info->mesh->index[i] * 3 + 1] , mesh_info->mesh->vertex[mesh_info->mesh->index[i] * 3 + 2] };
				float3 y = { mesh_info->mesh->vertex[mesh_info->mesh->index[i + 1] * 3], mesh_info->mesh->vertex[mesh_info->mesh->index[i + 1] * 3 + 1] , mesh_info->mesh->vertex[mesh_info->mesh->index[i + 1] * 3 + 2] };
				float3 z = { mesh_info->mesh->vertex[mesh_info->mesh->index[i + 2] * 3], mesh_info->mesh->vertex[mesh_info->mesh->index[i + 2] * 3 + 1] , mesh_info->mesh->vertex[mesh_info->mesh->index[i + 2] * 3 + 2] };

				//Assign Vertex to Triangle
				math::Triangle triangle = { x,y,z };

				//Look if ray hits with any of the triangles
				float distance;
				float3 hit_point;

				//Look if the raycast intersect with some of the triangles
				bool hit = ray_local_space.Intersects(triangle, &distance, &hit_point);

				//If its true the selected gameobject is the one we hit
				if (hit) {
					App->editor->gameobject_selected = gameObject_hit;
					return;
				}

			}

			it++;

		}

	}
}

// Get Camera View Matrix
float* ModuleCamera3D::GetViewMatrix() {
	return (float*)editor_camera_info->ViewMatrix().v;
}

// Get Camera Projection Matrix
float* ModuleCamera3D::GetProjectionMatrix() {
	return (float*)editor_camera_info->ProjectionMatrix().v;
}

