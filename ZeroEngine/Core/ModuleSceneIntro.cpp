#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "glew/include/glew.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	draw = false;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	if(draw=true){
		
		Plane p(0, 1, 0, 0);
		p.axis = true;
		p.Render();

		//Cube box(1, 1, 1);
		//box.Render();
		/*float cube[108]{

			//FRONT
			0.f, 0.f, 0.f,
			1.f, 0.f, 0.f,
			1.f, 1.f, 0.f,

			0.f, 0.f, 0.f,
			1.f, 1.f, 0.f,
			0.f, 1.f, 0.f,

			//LEFT			 
			0.f, 0.f,-1.f,
			0.f, 0.f, 0.f,
			0.f, 1.f, 0.f,

			0.f, 0.f,-1.f,
			0.f, 1.f, 0.f,
			0.f, 1.f,-1.f,

			//RIGHT	 
			1.f, 0.f, 0.f,
			1.f, 0.f, -1.f,
			1.f, 1.f, -1.f,

			1.f, 0.f, 0.f,
			1.f, 1.f, -1.f,
			1.f, 1.f, 0.f,

			//BACK
			0.f, 0.f, -1.f,
			1.f, 1.f, -1.f,
			1.f, 0.f, -1.f,

			0.f, 0.f, -1.f,
			0.f, 1.f, -1.f,
			1.f, 1.f, -1.f,

			//DOWN
			0.f, 0.f, 0.f,
			0.f, 0.f, -1.f,
			1.f, 0.f, -1.f,

			0.f, 0.f, 0.f,
			1.f, 0.f, -1.f,
			1.f, 0.f, 0.f,

			//UP
			1.f, 1.f, -1.f,
			0.f, 1.f, -1.f,
			0.f, 1.f, 0.f,

			1.f, 1.f, 0.f,
			1.f, 1.f, -1.f,
			0.f, 1.f, 0.f,

		};

		uint my_id = 0;
		glGenBuffers(1, (GLuint*)&(my_id));
		glBindBuffer(GL_ARRAY_BUFFER, my_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, cube, GL_STATIC_DRAW);

		glLineWidth(2.0f);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, my_id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		//...bind and use other buffers
		glDrawArrays(GL_TRIANGLES, 0, 108);
		glDisableClientState(GL_VERTEX_ARRAY);
		*/
		
		uint points[24]{

			0.f, 0.f, 0.f,  //A
			1.f, 0.f, 0.f,  //B
			1.f, 1.f, 0.f,  //C
			0.f, 1.f, 0.f,  //D

			0.f, 0.f, -1.f, //E
			1.f, 0.f, -1.f, //F
			0.f, 1.f,-1.f,  //G
			1.f, 1.f, -1.f, //H

		};

		uint cube_indices[36]{
			
			//FRONT
			0, 1, 2,
			0, 2, 3,

			//RIGHT
			4, 0, 3,
			4, 3, 7,

			//LEFT
			1, 5, 6,
			1, 6, 2,

			//BACK
			5, 4, 7,
			5, 7, 6,

			//DOWN
			0, 4, 5,
			0, 5, 1,

			//UP
			3, 2, 6,
			3, 6, 7,

		};

		uint my_vertex = 0;
		glGenBuffers(1, (GLuint*)&(my_vertex));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_vertex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 24, points, GL_STATIC_DRAW); 

		uint my_indices = 0;
		glGenBuffers(1, (GLuint*)&(my_indices));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, cube_indices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
		glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, NULL);

		

			

			glLineWidth(1.0f);

	}
	return UPDATE_CONTINUE;
}

