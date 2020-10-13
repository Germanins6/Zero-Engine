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

void Primitives::CubeGL(vec3 size, vec3 pos) {

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
	
	CubeDraw(points_cube, cube_indices, pos);
}

void Primitives::CubeDraw(float points[], uint cube_indices[], vec3 pos) {
	
	
	uint my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, points, GL_STATIC_DRAW);

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, cube_indices, GL_STATIC_DRAW);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();
}

void Primitives::SphereGL(uint rings, uint sectors, float radius, vec3 pos) {

	vector<GLfloat> vertices;
	vector<GLushort> indices;

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);

	vector<GLfloat>::iterator v = vertices.begin();

	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		
		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

	}

	indices.resize(rings * sectors * 4);
	
	vector<GLushort>::iterator i = indices.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}


	SphereDraw(vertices, indices, pos);
}

void Primitives::SphereDraw(vector<GLfloat> vertex, vector<GLushort> index, vec3 pos) {
	
	/*uint my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex.size(), vertex, GL_STATIC_DRAW);

	uint my_index = 0;
	glGenBuffers(1, (GLuint*)&(my_index));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, index, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(4, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_index);
	glDrawElements(GL_QUADS, 10000, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);*/

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
	glDrawElements(GL_QUADS, index.size(), GL_UNSIGNED_SHORT, &index[0]);
	
	glPopMatrix();
		

}

void Primitives::PyramidGL(uint faces, vec3 size, vec3 pos) {
	
	/*float points[15]{
	
	   size.x * 0.f, size.y * 0.f, size.z * 0.f, //A
	   size.x * 1.f, size.y * 0.f, size.z * 0.f, //B
	  
	   size.x * 0.f, size.y * 0.f, size.z * -1.f, //C
	   size.x * 1.f, size.y * 0.f, size.z * -1.f, //D
	   
	   size.x * 0.5f, size.y * 1.f, size.z * -0.5f, //E
	   
	   
	};

	uint indices[18]{ 
		
		0,1,4, //Front
		2,0,4, //Right
		1,3,4, //Left
		3,2,4, //Back
		
		//Down
		0,3,1,
		0,2,3,

	};*/
	
	vector<vec3> points;
	vector<int> indices;

	float base_lenght = faces * size.x;
	float base_angle_int = 360 / faces;
	float base_angle_ext = (180*(faces-2))/faces;
	float radiant_angle_int = (base_angle_int * M_PI )/ 180;
	float radiant_angle_ext = (base_angle_ext * M_PI) / 180;
	float apotema_base = size.x / (2 * tan(radiant_angle_int / 2));
	float angle = radiant_angle_int / 2;

	vec3 central_point = { size.x / 2, pos.y, -(static_cast<float>(size.z / (2 * tan(radiant_angle_int /2)))) };
	vec3 height_point = { 0.f, (central_point.y + size.y), 0.f };
	vec3 central_point_2 = { 0.f, 0.f, 0.f };
	vec3 apotema_point = { size.z / 2 + pos.x, pos.y, pos.z };
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
		vec3 vec_rot = roty * vect_to_center;
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


	PyramidDraw(points, indices, pos);
	
}

void Primitives::PyramidDraw(vector<vec3> points, vector<int> indices, vec3 pos) {

	/*uint my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points, GL_STATIC_DRAW);

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices, GL_STATIC_DRAW);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();*/

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, &points[0]);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]);

	glPopMatrix();

}
