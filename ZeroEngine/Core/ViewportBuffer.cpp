#include "Application.h"
#include "ViewportBuffer.h"
#include "Globals.h"

#include <string>
#include "ImGui/imgui_internal.h"
#include <gl/GL.h>

//quemepario
ViewportBuffer::ViewportBuffer(Application* app, bool start_enabled) : Module(app, start_enabled){
	show_viewport_window = true;
}

//Lapuitra
ViewportBuffer::~ViewportBuffer()
{}

bool  ViewportBuffer::Init() {
	return true;
}

bool ViewportBuffer::Start() {

	bool ret = false;

	//Creating framebuffer
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		ret = true;
		LOG("Succesfull Buffer loaded");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &frameBuffer);

	//Creating destiny texture to render
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//glViewport instead before render buffer to render all window

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Attach texture to frameBuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	return true;
}

update_status ViewportBuffer::PreUpdate(float dt) {
	
	return UPDATE_CONTINUE;
}

update_status ViewportBuffer::Update(float dt) {
	return UPDATE_CONTINUE;
}

update_status ViewportBuffer::PostUpdate(float dt) {
	return UPDATE_CONTINUE;
}

bool ViewportBuffer::CleanUp() {
	return true;
}