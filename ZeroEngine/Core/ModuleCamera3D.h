#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Move(float3& Movement, float speed, float dt);
	void Mouse(float3& Movement, float speed, float dt);
	void MousePicking();

	float* GetViewMatrix();
	float* GetProjectionMatrix();

	void CameraInfo();

public:

	GameObject* camera;
	ComponentCamera* editor_camera_info;
	ComponentTransform* editor_camera_transform;
	ComponentCollider* editor_camera_collider;
	ComponentRigidDynamic* editor_camera_rigid;
	//ComponentCamera* scene_camera_info;
	math::float3 Position, Reference;
	bool camera_created;
	
	
private:
	
	//mat4x4 ViewMatrix, ViewMatrixInverse;
};