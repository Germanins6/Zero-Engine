#include "Globals.h"
#include "Application.h"
#include "PrimitivesGL.h"
#include "ModuleWindow.h"
#include "ModuleEditor.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <math.h>
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Core/glew/libx86/glew32.lib")

Primitives::Primitives(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Destructor
Primitives::~Primitives()
{}

// Called before render is available
bool Primitives::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;


	return ret;
}

// Called before quitting
bool Primitives::CleanUp()
{
	LOG("Destroying Primitives");

	return true;
}

					//CUBE//
//================================================//
CubeGL::CubeGL(vec3 size) {

	float points_cube[24]{
			   
		size.x * 0.f, size.y * 0.f, size.z *  0.f,  //A
		size.x * 1.f, size.y * 0.f, size.z *  0.f,  //B
		size.x * 1.f, size.y * 1.f, size.z *  0.f,  //C
		size.x * 0.f, size.y * 1.f, size.z *  0.f,  //D
		
		size.x * 0.f, size.y * 0.f, size.z * -1.f,  //E
		size.x * 1.f, size.y * 0.f, size.z * -1.f,  //F
		size.x * 0.f, size.y * 1.f, size.z * -1.f,  //G
		size.x * 1.f, size.y * 1.f, size.z * -1.f,  //H
					
	};

	uint cube_indices[36]{

		//FRONT
		0, 1, 2,
		0, 2, 3,

		//RIGHT
		4, 0, 3,
		4, 3, 6,

		//LEFT
		1, 5, 7,
		1, 7, 2,

		//BACK
		5, 4, 6,
		5, 6, 7,

		//DOWN
		0, 4, 5,
		0, 5, 1,

		//UP
		3, 2, 7,
		3, 7, 6,

	};
	
	CubeBuffers();
}

CubeGL::~CubeGL() {}

void CubeGL::CubeBuffers() {

	this->my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(this->my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, this->points_cube, GL_STATIC_DRAW);

	this->my_indices = 0;
	glGenBuffers(1, (GLuint*)&(this->my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, this->cube_indices, GL_STATIC_DRAW);

}

void CubeGL::CubeDraw(vec3 pos) {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}
//================================================//
					//SPHERE//
//================================================//
/*
void SphereGL::Sphere(uint rings, uint sectors, float radius) {

	vector<float> vertices;
	vector<short> indices;

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);

	vector<float>::iterator v = vertices.begin();

	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		
		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

	}

	indices.resize(rings * sectors * 4);
	
	vector<short>::iterator i = indices.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}

	vertices_amount = vertices.size();
	vertices_ = new float[vertices_amount];
	indices_amount = indices.size();
	indices_ = new short[indices_amount];

	for (size_t i = 0; i < vertices_amount; i++)
	{
		vertices_[i] = vertices[i];
	}
	
	for (size_t i = 0; i < indices_amount; i++)
	{
		indices_[i] = indices[i];
	}

	vertices.clear();
	indices.clear();

	SphereBuffers();

}

void SphereGL::SphereBuffers() {

	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount, vertices_, GL_STATIC_DRAW);

	my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * indices_amount, indices_, GL_STATIC_DRAW);

}

void SphereGL::SphereDraw(vec3 pos) {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_QUADS, indices_amount, GL_UNSIGNED_SHORT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

}

//================================================//
					//PYRAMID//
//================================================//

void PyramidGL::Pyramid(uint faces, vec3 size, vec3 pos, float height, float face_lenght) {
	
	vector<vec3> points;
	vector<int> indices;
	size.x = size.x * SIZE_INITIAL;
	size.z = size.z * SIZE_INITIAL;
	float base_lenght = (face_lenght / faces);
	float base_angle_int = 360 / faces;
	//float base_angle_ext = (180*(faces-2))/faces;
	float radiant_angle_int = (base_angle_int * M_PI )/ 180;
	//float radiant_angle_ext = (base_angle_ext * M_PI) / 180;
	float apotema_base = base_lenght / (2 * tan(radiant_angle_int / 2));
	float angle = radiant_angle_int / 2;

	vec3 central_point = { base_lenght / 2, 0.f, -apotema_base };
	vec3 height_point = { 0.f, (central_point.y + height * size.y), 0.f };
	vec3 central_point_2 = { 0.f, 0.f, 0.f };
	vec3 apotema_point = { (base_lenght / 2), central_point.y, 0.f };
	vec3 vect_to_center = apotema_point - central_point;

	//LOG("X:%f Y:%f Z:%f ", height_point.x, height_point.y, height_point.z); 
	
	//Create Vector Of Vertexs
	for (size_t i = 0; i < faces; i++)
	{
		
		float cosr = cos(angle);
		float sinr = sin(angle);
		mat3x3 roty = mat3x3(

			cosr, 0.f, sinr,
			0.f,  1.f, 0.f, 
		   -sinr, 0.f, cosr

		);
		vec3 vec_rot = (roty * vect_to_center) * size;
		//LOG("X:%f Y:%f Z:%f ANGLE: %f", vec_rot.x, vec_rot.y, vec_rot.z, angle); //Vertex
		points.push_back(vec_rot); //Add Vertex
		angle = (radiant_angle_int) + angle;
		
	}
	
	points.push_back(central_point_2); //Add Central Point to create Base
	points.push_back(height_point); //Add Height Point to make it 3D
	//LOG("X: %f Y: %f  Z: %f", central_point.x, central_point.y, central_point.z);

	//Create Vector of Indices
	for (size_t i = 0; i <= faces; i++)
	{
		//Create Base
		if (i==faces) {
			indices.push_back(i-1);
			indices.push_back(0);
			indices.push_back(points.size() - 2);

			indices.push_back(0);
			indices.push_back(i - 1);
			indices.push_back(points.size() - 1);

		}
		else {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(points.size() - 2);

			indices.push_back(i + 1);
			indices.push_back(i);
			indices.push_back(points.size() - 1);
		}

		

	}

	vertices_amount = points.size();
	vertices_ = new float[vertices_amount];
	indices_amount = indices.size();
	indices_ = new short[indices_amount];

	for (size_t i = 0; i < vertices_amount; i++)
	{
		vertices_[i * 3] = points[i].x;
		vertices_[i * 3 + 1] = points[i].y;
		vertices_[i * 3 + 2] = points[i].z;
	}

	for (size_t i = 0; i < indices_amount; i++)
	{
		indices_[i] = indices[i];
	}

	PyramidBuffers();
	
}

void PyramidGL::PyramidBuffers() {

	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount, vertices_, GL_STATIC_DRAW);

	my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_amount, indices_, GL_STATIC_DRAW);

}

void PyramidGL::PyramidDraw(vec3 pos) {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, vertices_);
	glDrawElements(GL_TRIANGLES, indices_amount, GL_UNSIGNED_INT, indices_);

	glPopMatrix();

}
*/
//================================================//
					//AXIS/GRID//
//================================================//
void Primitives::AxisGL(int size) {

	glLineWidth(2.0f);

	glBegin(GL_LINES);
	glColor3f(0.75f, 0.75f, 0.75f);
	for (int i = -size; i <= size; i++)
	{
		glVertex3f(i, 0, -size);
		glVertex3f(i, 0, size);
		glVertex3f(-size, 0, i);
		glVertex3f(size, 0, i);
	}

	glEnd();

	glLineWidth(1.0f);

	// Draw Axis
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

	glEnd();

	glLineWidth(1.0f);

}