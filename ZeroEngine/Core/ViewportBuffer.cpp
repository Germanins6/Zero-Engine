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