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

ComponentMesh::ComponentMesh(GameObject* parent, Mesh* data) : Component(parent, ComponentType::MESH) {

	//Receive mesh information(vertex,index...) and generate buffers then in update renders.
	mesh = data;
	mesh->GenerateBufferPrimitives();

	draw_mesh = true;
	draw_vertexNormals = false;
	draw_faceNormals = false;

}

ComponentMesh::~ComponentMesh() {

	mesh = nullptr;
	path_info = nullptr;
}

bool ComponentMesh::Update(float dt) {

	//Just render, dont try to generate buffers each frame _)
	if (draw_mesh && this->owner != nullptr) {
		
		if (mesh->type == PrimitiveTypesGL::PrimitiveGL_NONE) {
			
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
		else if (mesh->type == PrimitiveGL_Cube || mesh->type == PrimitiveGL_Sphere || mesh->type == PrimitiveGL_Pyramid || mesh->type == PrimitiveGL_Cylinder) {
			mesh->RenderPrimitives();
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

	//-- Generate Texture_Buffers
	this->my_texture = 0;
	glGenBuffers(1, (GLuint*)&(this->my_texture));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * this->num_vertex, this->uv_coords, GL_STATIC_DRAW);

}

void Mesh::GenerateTextureInfo() {

	//-- Generate Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&(this->tex_info->id));
	glBindTexture(GL_TEXTURE_2D, this->tex_info->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)this->tex_info->GetWidth(), (int)this->tex_info->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)this->tex_info->data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, App->geometry->checkerImage);
}

void Mesh::GenerateBufferPrimitives() {

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

	/*//-- Generate Texture_Buffers
	this->my_texture = 0;
	glGenBuffers(1, (GLuint*)&(this->my_texture));
	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * this->num_vertex, this->uv_coords, GL_STATIC_DRAW);

	//-- Generate Texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&(this->tex_info->id));
	glBindTexture(GL_TEXTURE_2D, this->tex_info->id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)this->tex_info->GetWidth(), (int)this->tex_info->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLubyte*)this->tex_info->data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, App->geometry->checkerImage);
	*/
}

void Mesh::RenderGeometry() {

	//--Enable States--//
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	//-- Buffers--//
	glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if(draw_texture==true){
		if(this->tex_info != nullptr)glBindTexture(GL_TEXTURE_2D, this->tex_info->id);
	}

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

void Mesh::RenderPrimitives() {

	//--Enable States--//
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//-- Buffers--//
	//glBindBuffer(GL_ARRAY_BUFFER, this->my_texture);
	//glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, this->my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//glBindTexture(GL_TEXTURE_2D, this->tex_info->id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->my_indices);

	//glNormalPointer(GL_FLOAT, 0, NULL);

	//-- Draw --//
	glDrawElements(GL_TRIANGLES, this->num_index, GL_UNSIGNED_INT, NULL);

	//-- UnBind Buffers--//
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}