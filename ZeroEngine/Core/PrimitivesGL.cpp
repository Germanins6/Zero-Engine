#include "Globals.h"
#include "Application.h"
#include "PrimitivesGL.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

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

void Primitives::CubeGL(vec3 size) {

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
	
	CubeDraw(points_cube, cube_indices);
}

void Primitives::CubeDraw(float points[], uint cube_indices[]) {
	
	uint my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, points, GL_STATIC_DRAW);

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, cube_indices, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void Primitives::SphereGL(uint rings, uint sectors, float radius) {


	vector<GLfloat> vertices;
	vector<GLfloat> normals;
	vector<GLfloat> texcoords;
	vector<GLushort> indices;

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	texcoords.resize(rings * sectors * 2);

	vector<GLfloat>::iterator v = vertices.begin();
	vector<GLfloat>::iterator n = normals.begin();
	vector<GLfloat>::iterator t = texcoords.begin();

	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		float const y = sin(-M_PI_2 + M_PI * r * R);
		float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
		float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

		*t++ = s * S;
		*t++ = r * R;

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;

		*n++ = x;
		*n++ = y;
		*n++ = z;
	}

	indices.resize(rings * sectors * 4);
	
	vector<GLushort>::iterator i = indices.begin();
	for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}


	SphereDraw(vertices, normals, texcoords, indices, { 0,0,0 });
}

void Primitives::SphereDraw(vector<GLfloat> vertices, vector<GLfloat> normals, vector<GLfloat> texcoords, vector<GLushort> indices, vec3 pos) {
	
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
		glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
		glPopMatrix();

}