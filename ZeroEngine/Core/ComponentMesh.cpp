#include "ComponentMesh.h"

// -- Tools
#include "SDL/include/SDL_opengl.h"
#include "glew/include/glew.h"
#include "Application.h"
#include "ModuleGeometry.h"

ComponentMesh::ComponentMesh(GameObject* parent, Mesh* data, const char* path) : Component(parent, ComponentType::MESH) {

	//Receive mesh information(vertex,index...) and generate buffers then in update renders.
	mesh = data;
	mesh->GenerateBufferGeometry();

	path_info = path;

	draw_vertexNormals = false;
	draw_faceNormals = false;
	draw_mesh = true;

	
}


ComponentMesh::~ComponentMesh() {

	mesh = nullptr;
	path_info = nullptr;
}

bool ComponentMesh::Update(float dt) {

	//Just render, dont try to generate buffers each frame _)
	if (draw_mesh && this->owner != nullptr) {

		mesh->RenderGeometry();

		// -- Vertex Normals Rendering -- //
		if (draw_vertexNormals) {

			glBegin(GL_LINES);
			glColor3f(1, 0, 1);

			for (size_t i = 0; i < mesh->num_vertex; i++)
			{
				glVertex3f(mesh->vertex[i * 3], mesh->vertex[i * 3 + 1], mesh->vertex[i * 3 + 2]);
				glVertex3f(mesh->vertex[i * 3] + mesh->normals[i * 3] * 0.15, mesh->vertex[i * 3 + 1] + mesh->normals[i * 3 + 1] * 0.15, mesh->vertex[i * 3 + 2] + mesh->normals[i * 3 + 2] * 0.15);
			}

			glColor3f(1, 1, 1);
			glEnd();
		}

		// -- Face Normals Rendering -- //
		if (draw_faceNormals) {

			glBegin(GL_LINES);
			glColor3f(1, 0, 1);

			for (size_t i = 0; i < mesh->num_normal_faces; i++)
			{
				glVertex3f(mesh->normal_faces[i * 3], mesh->normal_faces[i * 3 + 1], mesh->normal_faces[i * 3 + 2]);
				glVertex3f(
					mesh->normal_faces[i * 3] + mesh->normal_face_vector_direction[i * 3] * 0.15,
					mesh->normal_faces[i * 3 + 1] + mesh->normal_face_vector_direction[i * 3 + 1] * 0.15,
					mesh->normal_faces[i * 3 + 2] + mesh->normal_face_vector_direction[i * 3 + 2] * 0.15
				);
			}

			glColor3f(1, 1, 1);
			glEnd();
		}
	}

	return true;

}

void Mesh::GenerateBufferGeometry() {

	//-- Generate Normals
	this->my_normals = 0;
	glGenBuffers(1, (GLuint*)&(this->my_normals));
	glBindBuffer(GL_NORMAL_ARRAY, this->my_normals);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * this->num_normals * 3, this->normals, GL_STATIC_DRAW);

	//-- Generate Vertex
	this->my_vertex = 0;
	glGenBuffers(1, (GLuint*)&(this->my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->num_vertex * 3, this->vertex, GL_STATIC_DRAW);

	//-- Generate Index
	this->my_indices = 0;
	glGenBuffers(1, (GLuint*)&(this->my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->num_index, this->index, GL_STATIC_DRAW);

}

void Mesh::RenderGeometry() {


	//--Enable States--//
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	//-- Buffers--//
	
	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);

	glBindBuffer(GL_NORMAL_ARRAY, this->my_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//-- Draw --//
	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);

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