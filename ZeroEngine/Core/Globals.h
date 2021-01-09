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
typedef unsigned int UID;


enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Zero Engine v0.3"
#define ORGANITZATION "CITM UPC 2020-2021"

#define MESH_PATH "Library/Meshes/"
#define TEXTURE_PATH "Library/Textures/"
#define MODEL_PATH "Library/Models/"
#define MATERIAL_PATH "Library/Materials/"
#define SCENE_PATH "Assets/Scenes/"

#define MESH_ICON "Assets/Icons/mesh_icon.png"
#define FOLDER_ICON "Assets/Icons/folder_icon.png"

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

enum class GeometryType {
    PLANE,
    BOX,
    SPHERE,
    CAPSULE,
    NONE
};