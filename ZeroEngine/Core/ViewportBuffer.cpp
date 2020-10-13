#include "Application.h"
#include "ViewportBuffer.h"
#include "Globals.h"

#include <string>
#include "ImGui/imgui_internal.h"
#include <gl/GL.h>

//quemepario
ViewportBuffer::ViewportBuffer(Application* app, bool start_enabled) : Module(app, start_enabled){
	frameBuffer = NULL;
	renderBufferoutput = NULL;
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


	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->width, App->window->height ,0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//Render Buffers
	glGenRenderbuffers(1, &renderBufferoutput);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferoutput);

	//Bind tex data with render buffers
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->width, App->window->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferoutput);

	//After binding tex data, we must unbind renderbuffer and framebuffer not usefull anymore
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	return true;
}

update_status ViewportBuffer::PreUpdate(float dt) {

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}


update_status ViewportBuffer::PostUpdate(float dt) {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

bool ViewportBuffer::CleanUp() {

	//WE SHOULD CLEANUP SOMETHING. REMINDER

	return true;
}