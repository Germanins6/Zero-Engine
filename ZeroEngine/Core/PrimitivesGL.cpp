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

Primitives::Primitives() : type(PrimitiveTypesGL::PrimitiveGL_NONE)
{
	my_vertex = 0;
	my_indices = 0;
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

CubeGL::CubeGL():Primitives(){
	type = PrimitiveTypesGL::PrimitiveGL_Cube;
}
CubeGL::CubeGL(vec3 size) : Primitives() {

	float points_cube[24]{

		size.x * 0.f, size.y * 0.f, size.z * 0.f,  //A
		size.x * 1.f, size.y * 0.f, size.z * 0.f,  //B
		size.x * 1.f, size.y * 1.f, size.z * 0.f,  //C
		size.x * 0.f, size.y * 1.f, size.z * 0.f,  //D

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

	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, points_cube, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, cube_indices, GL_STATIC_DRAW);
}
void CubeGL::InnerRender(vec3 position, vec4 rotation) const {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

}

SphereGL::SphereGL() :Primitives() {
	type = PrimitiveTypesGL::PrimitiveGL_Sphere;
}
SphereGL::SphereGL(uint rings, uint sectors, float radius) {

	vector<float> vertices, normals, texCoords;
	vector<short> indices;

	// clear memory of prev arrays
	std::vector<float>().swap(vertices);
	std::vector<float>().swap(normals);
	std::vector<float>().swap(texCoords);

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectors;
	float stackStep = PI / rings;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= rings; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectors; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.push_back(nx);
			normals.push_back(ny);
			normals.push_back(nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectors;
			t = (float)i / rings;
			texCoords.push_back(s);
			texCoords.push_back(t);
		}
	}

	// generate CCW index list of sphere triangles

	int k1, k2;
	for (int i = 0; i < rings; ++i)
	{
		k1 = i * (sectors + 1);     // beginning of current stack
		k2 = k1 + sectors + 1;      // beginning of next stack

		for (int j = 0; j < sectors; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (rings - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
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

	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount, vertices_, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * indices_amount, indices_, GL_STATIC_DRAW);

}
void SphereGL::InnerRender(vec3 position, vec4 rotation) const{

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, indices_amount, GL_UNSIGNED_SHORT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

}

PyramidGL::PyramidGL() :Primitives() {
	type = PrimitiveTypesGL::PrimitiveGL_Pyramid;
}
PyramidGL::PyramidGL(uint faces, vec3 size, float height, float face_lenght) : Primitives(){
	
	float points[15]{
	
	   size.x * 0.f, size.y * 0.f, size.z * 0.f, //A
	   size.x * 1.f, size.y * 0.f, size.z * 0.f, //B
	  
	   size.x * 0.f, size.y * 0.f, size.z * -1.f, //C
	   size.x * 1.f, size.y * 0.f, size.z * -1.f, //D
	   
	   size.x * 0.5f, size.y * 1.f, size.z * -0.5f, //E
	   
	   
	};

	short indices[18]{ 
		
		0,1,4, //Front
		2,0,4, //Right
		1,3,4, //Left
		3,2,4, //Back
		
		//Down
		0,3,1,
		0,2,3,

	};
	
	/*size.x = size.x * SIZE_INITIAL;
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

		

	}*/

	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 15, points, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 18, indices, GL_STATIC_DRAW);
	
}
void PyramidGL::InnerRender(vec3 position, vec4 rotation) const {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, NULL);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix(); 
}

CylinderGL::CylinderGL() :Primitives() {
	type = PrimitiveTypesGL::PrimitiveGL_Cylinder;
}
CylinderGL::CylinderGL(float sectorCount, float radius, float height) : Primitives(){

	std::vector<float> vertices, normals, texCoords;
	std::vector<short>	indices;

	float sectorStep = 2 * PI / sectorCount;
	float sectorAngle;  // radian

	std::vector<float> unitCircleVertices;
	for (int i = 0; i <= sectorCount; ++i)
	{
		sectorAngle = i * sectorStep;
		unitCircleVertices.push_back(cos(sectorAngle)); // x
		unitCircleVertices.push_back(sin(sectorAngle)); // y
		unitCircleVertices.push_back(0);                // z
	}

	// clear memory of prev arrays
	std::vector<float>().swap(vertices);

	// get unit circle vectors on XY-plane
	std::vector<float> unitVertices = unitCircleVertices;

	// put side vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float t = 1.0f - i;                              // vertical tex coord; 1 to 0

		for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			float uz = unitVertices[k + 2];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(uy * radius);             // vy
			vertices.push_back(h);                       // vz
			// normal vector
			normals.push_back(ux);                       // nx
			normals.push_back(uy);                       // ny
			normals.push_back(uz);                       // nz
			// texture coordinate
			texCoords.push_back((float)j / sectorCount); // s
			texCoords.push_back(t);                      // t

		}
	}

	// the starting index for the base/top surface
	//NOTE: it is used for generating indices later
	int baseCenterIndex = (int)vertices.size() / 3;
	int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

	// put base and top vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
		float nz = -1 + i * 2;                           // z value of normal; -1 to 1

		// center point
		vertices.push_back(0);     vertices.push_back(0);     vertices.push_back(h);
		normals.push_back(0);      normals.push_back(0);      normals.push_back(nz);
		texCoords.push_back(0.5f); texCoords.push_back(0.5f);

		for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
		{
			float ux = unitVertices[k];
			float uy = unitVertices[k + 1];
			// position vector
			vertices.push_back(ux * radius);             // vx
			vertices.push_back(uy * radius);             // vy
			vertices.push_back(h);                       // vz
			// normal vector
			normals.push_back(0);                        // nx
			normals.push_back(0);                        // ny
			normals.push_back(nz);                       // nz
			// texture coordinate
			texCoords.push_back(-ux * 0.5f + 0.5f);      // s
			texCoords.push_back(-uy * 0.5f + 0.5f);      // t
		}
	}

	int k1 = 0;                         // 1st vertex index at base
	int k2 = sectorCount + 1;           // 1st vertex index at top

	// indices for the side surface
	for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
	{
		// 2 triangles per sector
		// k1 => k1+1 => k2
		indices.push_back(k1);
		indices.push_back(k1 + 1);
		indices.push_back(k2);

		// k2 => k1+1 => k2+1
		indices.push_back(k2);
		indices.push_back(k1 + 1);
		indices.push_back(k2 + 1);
	}

	// indices for the base surface
	//NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
	//      please see the previous code snippet
	for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(k + 1);
			indices.push_back(k);
		}
		else // last triangle
		{
			indices.push_back(baseCenterIndex);
			indices.push_back(baseCenterIndex + 1);
			indices.push_back(k);
		}
	}

	// indices for the top surface
	for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
	{
		if (i < sectorCount - 1)
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(k + 1);
		}
		else // last triangle
		{
			indices.push_back(topCenterIndex);
			indices.push_back(k);
			indices.push_back(topCenterIndex + 1);
		}
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

	glGenBuffers(1, (GLuint*)&(my_vertex));
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_amount, vertices_, GL_STATIC_DRAW);

	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * indices_amount, indices_, GL_STATIC_DRAW);

}
void CylinderGL::InnerRender(vec3 position, vec4 rotation) const {
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(270, 1, 0, 0);
	glColor4f(App->editor->current_color.x, App->editor->current_color.y, App->editor->current_color.z, App->editor->current_color.w);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, indices_amount, GL_UNSIGNED_SHORT, NULL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glPopMatrix();

}