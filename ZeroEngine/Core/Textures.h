#pragma once

// -- DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#include "glew/include/glew.h"


struct Texture {

	Texture(ILuint my_id, ILint h, ILint w, ILint format, ILubyte* info) : id{ my_id }, height { h }, width{ w }, type{ format }, data{ info }{}
	~Texture() {
		glDeleteTextures(1, &this->id);
		ilDeleteImages(1, &this->id);
		data = nullptr;
	}
	ILuint id;
	ILint height, width, type;
	ILubyte* data;

	//Get size and type
	ILint GetWidth(){ return width; }
	ILint GetHeight() { return height; }
	ILubyte GetType() { return type; }

};