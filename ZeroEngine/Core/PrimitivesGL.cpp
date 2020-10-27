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

Primitives::Primitives()
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

}