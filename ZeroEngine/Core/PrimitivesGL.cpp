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

bool Primitives::CubeGL(vec3 size) {

	bool ret = true;

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

	return ret;

}

bool Primitives::CubeDraw(float points[], uint cube_indices[]) {
	
	bool ret = false;
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

	return true;
}
