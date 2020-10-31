# Zero Engine

## What is this?

Zero Engine is a Engine made by Germán Insua and Christian Piña from Design and Devolupment Videogames Degree at CITM. 

## Repository link

https://github.com/Germanins6/Zero-Engine

## Team members and responsabilities

### Germán Insua Perdomo

Github link: https://github.com/Germanins6

### Christian Piña López

Github link: https://github.com/christianpi4

## Controls

WASD: Move in Scene

Right Mouse Click: Orbit camera.

R: Move camera up vertically .

T: Move camera down vertically .

Shift: Augment camera velocity.

Mouse Wheel: Zoom In and Out.

While a GameObject is selected you can: 

F: Center Camera to GameObject.

Alt + Mouse Right Click: Orbit around object.

B: Orbit around object automatically.

## Drag and Drop
When you Drop a Mesh and in the Textures folder there are his correspondated texture, automatically is charged the mesh in scene with the texture.
You can drop any mesh and if you select it and drop a texture automatically is applicated to the mesh and showed at inspector.

Supported file extensions:
Meshes:
.fbx
.FBX
.obj
.OBJ
Textures:
.jpg
.JPG
.png
.PNG

## Windows

#File

Exit: Close Engine.

#GameObject

Create Empty GameObject: Creates an Empty GameObject that only haves Component Transform.

3D Objects: 
	-Cube: Create a Cube in Scene.
	-Pyramid: Create a Pyramid in Scene.
	-Sphere: Create a Sphere in Scene.
	-Cylinder: Create a Cylinder in Scene.

#Window

Examples: Open the Demo Window of Imgui.
Workspace Style: Change the Engine Style.
		-Dark
		-Classic
		-Light
		-Custom
		
Hierarchy: Show the Hierarchy Window where all the list of GameObjects.
Inspector: Show the Inspector Window where are all the Components of Selected GameObject.
	-Component Transform:
		-Position
		-Rotation
		-Scale
	-Component Mesh:
		-Active or Disable Mesh Draw
		-Mesh File
		-Vertex Normals
		-Index
		-Vertexs
		-UV Coords
		-Show Vertex Normals
		-Show Face Normals
	-Component Material:
		-Texture File
		-Width
		-Height
		-Show Checkers
		-Active or Disable Texture Draw
		-Preview of Checkers and Texture
		
Scene: Show the Scene Window.
Game: Show the Game Window.
Console: Show the console where all the Logs and initialitzations of librarys are showed.
Configuration: Show the Configuration window
	-Application:
		-App Name
		-Organization
		-Framerate
		-Frames Per Second Graph
		-Milliseconds Graph
		-Active VSYNC
	-Window:
		-Engine Icon
		-Brightness
		-Width
		-Height
		-Refresh Rate
		-FullScreen
		-Resizable
		-Borderless
		-Full Desktop
	-Hardware:
		-SDL Version
		-Cpus
		-System RAM
		-Caps
		-Gpu
		-Brand
	-Render:
		-Depth Test
		-Cull Face
		-Lightning
		-Color Material
		-Texture Draw
		-Wireframe Mode
	-Input:
		-Mouse Position
		
##Help

Documentation: Link to Zero Engine Documentation.
Download Latest: Link to Zero Engine Releases.
Report Issue: Link to Zero Engine Issues.
About: Show About window with some info about the Engine. 

## Libraries Used
* SDL v2.0.12
* Glew v2.1.0
* OpenGL v3.1.0
* ImGui v1.78
* MathGeoLib v1.5
* PhysFS v3.0.2
* DevIL v1.7.8
* Assimp v3.1.1

## License

MIT License

Copyright (c) 2020 Germán Insua & Christian Piña

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

##
![University Logo](https://www.citm.upc.edu/templates/new/img/logoCITM.png?1401879059)   
