#include "ComponentMesh.h"

// -- Tools
#include "SDL/include/SDL_opengl.h"
#include "glew/include/glew.h"
#include "Application.h"
#include "ResourceMesh.h"

//===== ComponentMesh =====//

ComponentMesh::ComponentMesh(GameObject* parent, Resource* resourceMesh) : Component(parent, ComponentType::MESH) {

	//Receive mesh information(vertex,index...) and generate buffers then in update renders.
	ourMesh = dynamic_cast<ResourceMesh*>(resourceMesh);

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

	ourMesh = nullptr;
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

	ComponentMaterial* mat = dynamic_cast<ComponentMaterial*>(owner->GetMaterial());
	if (mat != nullptr) {

		glColor4f(mat->GetMaterial()->materialColor.r, mat->GetMaterial()->materialColor.g, mat->GetMaterial()->materialColor.b, mat->GetMaterial()->materialColor.a);

		if (mat->draw_texture && mat->GetMaterial()->diffuse != nullptr)
			glBindTexture(GL_TEXTURE_2D, mat->GetMaterial()->diffuse->gpu_id);

		if(mat->draw_checkers)
			glBindTexture(GL_TEXTURE_2D, mat->CheckersID);
	}


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
	//glBindTexture(GL_TEXTURE_2D, 0);

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