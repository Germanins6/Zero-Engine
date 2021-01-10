#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "glew/include/glew.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void VSYNC_();
	void DrawingModes(bool currentState, int glMode);
	void DrawRayCast(LineSegment ray_cast);

	void DrawBoxCollider(const float4x4& transform, const float3& half_size);
	void DrawSphereCollider(const float4x4& transform, float radius);
	void DrawCapsuleCollider(const float4x4& transform, const float half_height, const float radius);
	void RenderThrowSpheres(physx::PxRigidBody* sphere_vector);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool depth_test;
	bool cull_face;
	bool lighting;
	bool mat_color;
	bool texture;
	bool wireframe_mode;
	bool vsync_active;

	LineSegment ray_cast;

};