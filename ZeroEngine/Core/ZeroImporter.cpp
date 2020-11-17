#include "ZeroImporter.h"

#include "ComponentMesh.h"

//-- Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

//Devil Libs loading
#pragma comment(lib, "Core/DevIL/libx86/DevIL.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILU.lib")
#pragma comment(lib, "Core/DevIL/libx86/ILUT.lib")

void MeshImporter::Import(const aiMesh* aiMesh, Mesh* ourMesh){

	//Vertex
	ourMesh->num_vertex = aiMesh->mNumVertices;
	ourMesh->vertex = new float[ourMesh->num_vertex * 3];
	memcpy(ourMesh->vertex, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertex * 3);
	LOG("New mesh with %d vertices", ourMesh->num_vertex);

	//Index
	if (aiMesh->HasFaces()) {
		ourMesh->num_index = aiMesh->mNumFaces * 3;
		ourMesh->index = new uint[ourMesh->num_index];

		for (size_t i = 0; i < aiMesh->mNumFaces; i++)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
				LOG("WARNING, geometry face with != 3 indices!")
			else
				memcpy(&ourMesh->index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}

	//Normals
	if (aiMesh->HasNormals()) {

		//Initialize size
		ourMesh->normals = new float[aiMesh->mNumVertices * 3];

		//Calculate Normals of Vertex
		for (size_t i = 0; i < aiMesh->mNumVertices; i++) {

			//Calculate Normals of Vertex
			ourMesh->normals[i * 3] = aiMesh->mNormals[i].x;
			ourMesh->normals[i * 3 + 1] = aiMesh->mNormals[i].y;
			ourMesh->normals[i * 3 + 2] = aiMesh->mNormals[i].z;
		}
	}

	//Uvs
	ourMesh->uv_coords = new float[ourMesh->num_vertex * 2];
	for (size_t i = 0; i < aiMesh->mNumVertices; i++) {

		if (aiMesh->mTextureCoords[0]) {
			ourMesh->uv_coords[i * 2] = aiMesh->mTextureCoords[0][i].x;
			ourMesh->uv_coords[i * 2 + 1] = aiMesh->mTextureCoords[0][i].y;
		}
	}

}

uint64 MeshImporter::Save(const Mesh* ourMesh, char** fileBuffer) {

}