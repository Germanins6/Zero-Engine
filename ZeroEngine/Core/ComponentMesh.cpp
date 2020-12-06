#include "ComponentMesh.h"

// -- Tools
#include "SDL/include/SDL_opengl.h"
#include "glew/include/glew.h"
#include "Application.h"
#include "ResourceMesh.h"

//===== ComponentMesh =====//

ComponentMesh::ComponentMesh(GameObject* parent, ResourceMesh* resourceMesh) : Component(parent, ComponentType::MESH) {

	//Receive mesh information(vertex,index...) and generate buffers then in update renders.
	ourMesh = resourceMesh;
	//GenerateCheckers();

	//Generate geometry with resourceMesh info and generate bounding boxes
	GenerateBufferGeometry();
	GenerateAABB();

	draw_mesh = true;
	draw_vertexNormals = false;
}

ComponentMesh::~ComponentMesh() {

	glDeleteBuffers(1, (GLuint*)&(this->my_normals));
	glDeleteBuffers(1, (GLuint*)&(this->my_vertex));
	glDeleteBuffers(1, (GLuint*)&(this->my_indices));
	glDeleteBuffers(1, (GLuint*)&(this->my_texture));

	RELEASE_ARRAY(ourMesh->index);
	RELEASE_ARRAY(ourMesh->vertex);
	RELEASE_ARRAY(ourMesh->normals);
	RELEASE_ARRAY(ourMesh->uv_coords);
}

bool ComponentMesh::Update(float dt) {

	//Just render, dont try to generate buffers each frame _)
	if (draw_mesh && this->owner != nullptr) {
			
		RenderGeometry();
		
		// -- Vertex Normals Rendering -- //
		if (draw_vertexNormals) {

			glBegin(GL_LINES);
			glColor3f(1, 0, 1);

			DrawVertexNormals();

			glColor3f(1, 1, 1);
			glEnd();
		}
	}

	return true;
}

void ComponentMesh::GenerateBufferGeometry() {

	//-- Generate Index
	my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * ourMesh->num_index, ourMesh->index, GL_STATIC_DRAW);

	//-- Generate Vertex
	my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ourMesh->num_vertex * 3, ourMesh->vertex, GL_STATIC_DRAW);

	//-- Generate Normals
	my_normals = 0;
	glGenBuffers(1, (GLuint*)&(my_normals));
	glBindBuffer(GL_NORMAL_ARRAY, my_normals);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * ourMesh->num_vertex * 3, ourMesh->normals, GL_STATIC_DRAW);

	//-- Generate Texture_Buffers
	my_texture = 0;
	glGenBuffers(1, (GLuint*)&(my_texture));
	glBindBuffer(GL_ARRAY_BUFFER, my_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * ourMesh->num_vertex, ourMesh->uv_coords, GL_STATIC_DRAW);

}

void ComponentMesh::RenderGeometry() {

	//--Enable States--//
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	//-- Buffers--//
	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	/*if (draw_texture && this->tex_info != nullptr)
		glBindTexture(GL_TEXTURE_2D, this->tex_info->id);

	if (draw_checkers)
		glBindTexture(GL_TEXTURE_2D, textureID);*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);

	glBindBuffer(GL_NORMAL_ARRAY, this->my_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//Get matrix info for each mesh
	float4x4 globalTransform = dynamic_cast<ComponentTransform*>(owner->GetTransform())->GetGlobalMatrix();
	glPushMatrix();
	glMultMatrixf((float*)&globalTransform);

	//-- Draw --//
	glDrawElements(GL_TRIANGLES, ourMesh->num_index, GL_UNSIGNED_INT, NULL);

	glPopMatrix();

	//-- UnBind Buffers--//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


void ComponentMesh::DrawVertexNormals() {

	for (size_t i = 0; i < ourMesh->num_vertex; i++)
	{
		glVertex3f(ourMesh->vertex[i * 3], ourMesh->vertex[i * 3 + 1], ourMesh->vertex[i * 3 + 2]);
		glVertex3f(ourMesh->vertex[i * 3] + ourMesh->normals[i * 3] * 0.15, ourMesh->vertex[i * 3 + 1] + ourMesh->normals[i * 3 + 1] * 0.15, ourMesh->vertex[i * 3 + 2] + ourMesh->normals[i * 3 + 2] * 0.15);
	}
}

void ComponentMesh::GenerateAABB() {
	bbox.SetNegativeInfinity();
	bbox.Enclose((float3*)ourMesh->vertex, ourMesh->num_vertex);
}


//This should be in material(?)
/*
void Mesh::GenerateTextureInfo() {

	//-- Generate Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (draw_texture && this->tex_info != nullptr) {
		glGenTextures(1, (GLuint*)&(this->tex_info->id));
		glBindTexture(GL_TEXTURE_2D, this->tex_info->id);

	}

	if (draw_checkers && this->tex_info != nullptr) {
		glGenTextures(1, (GLuint*)&(this->textureID));
		glBindTexture(GL_TEXTURE_2D, this->textureID);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (draw_texture && this->tex_info != nullptr)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)this->tex_info->GetWidth(), (int)this->tex_info->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)this->tex_info->data);

	if (draw_checkers)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->checkerImage);

	//UnBind last
	glBindTexture(GL_TEXTURE_2D, 0);

}
*/


/*
void Mesh::GenerateCheckers() {
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}
}
*/

/*
glGenTextures(1, (GLuint*)&(this->textureID));
glBindTexture(GL_TEXTURE_2D, this->textureID);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->checkerImage);
*/