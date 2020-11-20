#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	Position = float3(5.0f, 5.0f, 5.0f);
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

	if (editor_camera_transform != nullptr) {
		Position += newPos;
		editor_camera_info->SetPos(Position);
		editor_camera_transform->position = Position;
		Reference += newPos;
		editor_camera_info->SetReference(Reference);

		//editor_camera_info->LookAt(editor_camera_info->new_reference); 
		/*ComponentTransform* transform = dynamic_cast<ComponentTransform*>(camera->GetTransform());
		transform->position += newPos;
		editor_camera_info->new_reference += newPos;
		editor_camera_info->SetPos(transform->position);
		transform->SetPosition(transform->position.x, transform->position.y, transform->position.z);
		transform->UpdateGlobalMatrix();
		transform->UpdateNodeTransforms();*/
		
	}
	/*newPos = dynamic_cast<ComponentTransform*>(camera->GetTransform())->position + newPos;
	new_reference += newPos;
	editor_camera_info->frustum.pos = newPos + editor_camera_info->frustum.pos;
	LOG("%f %f %f", new_reference.x, new_reference.y, new_reference.z);
	//editor_camera_info->LookAt(new_reference);*/
	return UPDATE_CONTINUE;
}

// Camera Movement
void ModuleCamera3D::Move(float3& move, float speed, float dt) {

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = 24.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) move.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) move.y -= speed;

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

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) move += editor_camera_info->frustum.front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) move -= editor_camera_info->frustum.front * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) move -= editor_camera_info->frustum.WorldRight() * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) move += editor_camera_info->frustum.WorldRight() * speed;

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
			math::Quat rot_quat = math::Quat::RotateY(dx * dt * Sensitivity);
			editor_camera_info->frustum.front = rot_quat.Mul(editor_camera_info->frustum.front).Normalized();
			editor_camera_info->frustum.up = rot_quat.Mul(editor_camera_info->frustum.up).Normalized();
		}

		if (dy != 0.0f)
		{
			math::Quat rot_quat = math::Quat::RotateAxisAngle(editor_camera_info->frustum.WorldRight(), dy * dt * Sensitivity);
			math::float3 up = rot_quat.Mul(editor_camera_info->frustum.up).Normalized();

			if (up.y > 0.0f) {

				editor_camera_info->frustum.up = up;
				editor_camera_info->frustum.front = rot_quat.Mul(editor_camera_info->frustum.front).Normalized();

			}
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

