#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;


enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Zero Engine v0.1"
#define ORGANITZATION "CITM UPC 2020-2021"

#define MESH_PATH "Library/Meshes/"
#define TEXTURE_PATH "Library/Textures/"
#define MODEL_PATH "Library/Models/"
#define SCENE_PATH "Library/Scenes/"
#define MATERIAL_PATH "Library/Materials/"

#define MESH_ICON "Assets/Icons/mesh_icon.png"
#define FOLDER_ICON "Assets/Icons/folder_icon.png"