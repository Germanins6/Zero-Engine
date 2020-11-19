#include "Application.h"

#include "ComponentCamera.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent, ComponentType::CAMERA){

	frustum.type = math::PerspectiveFrustum;

	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 1.0f;
	frustum.farPlaneDistance = 10.0f;
	frustum.verticalFov = 60 * DEGTORAD;
	camera_aspect_ratio = App->window->window_aspect_ratio;
	frustum.horizontalFov = 2 * atanf(tanf(frustum.verticalFov * 0.5) * camera_aspect_ratio);
	cull = true;
}

ComponentCamera::~ComponentCamera() {

}

bool ComponentCamera::Update(float dt) {
	
	ComponentTransform* transform = dynamic_cast<ComponentTransform*>(owner->GetTransform());

	if (transform != nullptr) {
		frustum.pos = transform->GetGlobalMatrix().Transposed().TranslatePart();
		frustum.front = transform->GetGlobalMatrix().Transposed().WorldZ().Normalized();
		frustum.up = frustum.front.Cross(-frustum.WorldRight()).Normalized();

		Draw();
	}

	if(cull){
		CameraCullGameObjects();
	}

	return true;
}

void ComponentCamera::Draw() {

	glBegin(GL_LINES);
	glLineWidth(50.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	for (uint i = 0; i < frustum.NumEdges(); i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}
	glEnd();

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

}

//=============MATRIX==================//
math::float4x4 ComponentCamera::ViewMatrix() {

	math::float4x4 ViewMatrix = frustum.ViewMatrix();
	ViewMatrix.Transposed();
	return ViewMatrix;

}
math::float4x4 ComponentCamera::ProjectionMatrix() {

	math::float4x4 ProjectionMatrix = frustum.ProjectionMatrix();
	ProjectionMatrix.Transposed();
	return ProjectionMatrix;

}

//=============ASPECT RATIO==================//
float ComponentCamera::GetAspectRatio() {
	return frustum.AspectRatio();
}
void ComponentCamera::SetAspectRatio(float aspect_ratio) {
	camera_aspect_ratio = aspect_ratio;
	frustum.horizontalFov = 2 * atanf(tanf(frustum.verticalFov * 0.5) * camera_aspect_ratio);
}

//=============NEAR DISTANCE==================//
float ComponentCamera::GetNearDistance() {
	return frustum.nearPlaneDistance;
}
void ComponentCamera::SetNearDistance(float near_distance) {
	frustum.nearPlaneDistance = near_distance;
}

//=============FAR DISTANCE==================//
float ComponentCamera::GetFarDistance() {
	return frustum.farPlaneDistance;
}
void ComponentCamera::SetFarDistance(float far_distance) {
	frustum.farPlaneDistance = far_distance;
}

//=============FIELD OF VIEW==================//
float ComponentCamera::GetFOV() {
	return frustum.horizontalFov * RADTODEG;
}
void ComponentCamera::SetFOV(float fov) {
	frustum.horizontalFov = fov * DEGTORAD;
	frustum.verticalFov = 2 * atanf(tanf(frustum.horizontalFov * 0.5) * camera_aspect_ratio);
}

//=============CAMERA CULLING==================//
bool ComponentCamera::Cull(math::AABB bbox) {

	math::float3 vCorner[8];
	int iTotalIn = 0;
	bbox.GetCornerPoints(vCorner); // get the corners of the box into the vCorner array
	// test all 8 corners against the 6 sides
	// if all points are behind 1 specific plane, we are out
	// if we are in with all points, then we are fully in
	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			// test this point against the planes
			if (frustum.GetPlane(p).IsOnPositiveSide(vCorner[i])) { //<-- “IsOnPositiveSide” from MathGeoLib
				iPtIn = 0;
				--iInCount;
			}
		}
		// were all the points outside of plane p?
		if (iInCount == 0) {
			return(false);
		}
			
		// check if they were all on the right side of the plane
		iTotalIn += iPtIn;
	}
	// so if iTotalIn is 6, then all are inside the view
	if (iTotalIn == 6) {
		return(true);
	}
		
	// we must be partly in then otherwise

	return(true);
}

void ComponentCamera::CameraCullGameObjects() {
		
	for (size_t i = 0; i < App->scene->gameobjects.size(); i++)
	{
		ComponentMesh* mesh_info = dynamic_cast<ComponentMesh*>(App->scene->gameobjects[i]->GetMesh());

		if (mesh_info != nullptr) {
			
			if (mesh_info->mesh != nullptr)
			{
				if (Cull(App->scene->gameobjects[i]->bbox))
					App->scene->gameobjects[i]->active = false;
				else
					App->scene->gameobjects[i]->active = true;
			}
		}
	}
		
	
}
