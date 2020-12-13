## Zero Engine
---

Zero Engine is a 3D Game Engine developed by "Germán Insua" and "Christian Piña" as a task for the subject 3D     
Engines of the Degree in Videogames Design and Development of "Universitat Politénica de Catalunya".
..

### Zero Engine Repository Link

https://github.com/Germanins6/Zero-Engine

### Team members
  -Germán Insua Perdomo
   
    [Github account]: https://github.com/Germanins6

 -Christian Piña López
    
    [Github account]: https://github.com/christianpi4

## How to use
---

#### **FUNCTIONALITIES**

#### [Changelog]
* v0.1: 
  
  -With this version the 3D Engine can just show geometries and display textures (*just in diffuse channel*).
  
  -Drag and drop files allowed for .obj and .fbx. **[Make sure .fbx encryption ASCII instead Binary]**.
  
  -Create Empty objects and primitives.
  
  -Drag and drop .png and .jpg files will load into geometry selected.
  
  -Show all gameobjects in scene and display its info.
  
  -Dock enabled UI Unity Like.
  
  
* v0.2: [Latest version]
  
  -This version does came with several changes.
  
  -Engine can import files and save their own custom files.
  
  -Scenes can be Saved/Loaded
  
  -Gameobjects can be Translated, Rotated and Scaled. Engine also does have interactable guizmos to work properly with gameobjects.
  
  -Mouse picking to select in scene gameobjects.
  
  -New component camera, that does have their own camera settings and can do Graphic optimizations as Frustum Culling using gameObjects bounding boxes.
  
  -Big changes about gameobject components adapted to be used as resources.
  
  -Resource manager, user can navigate through folders in engine execution. Folders and files can be modified, opened, deleted...
  
  -Time manager
   
#### **CONTROLS**

#### -Non-Selected GameObject controls.
---


**WASD** : Camera movement in scene.

**Right Mouse Click**: Orbit camera.

**E**: Translate gameobject with guizmos.

**R**: Rotate gameobject with guizmos.

**T**: Scale gameobject with guizmos.

**C**: Moves camera up vertically.

**V**: Moves camera down vertically.

**Shift**: Increase camera speed.

**Mouse Wheel**: Zoom In/Out.

#### -GameObject controls if selected.
---

**F**: Centers Camera to GameObject.

**Alt + Mouse Right Click**: Orbital camera around gameObject.

**B**: Turntable around gameobject automatically.


#### -Resource manager controls
---

**Left Click Twice to TextureFile while gameobject selected**: Change material diffuse resourceTexture to selected.

**Left Click Twice to Model**: Open model selected in scene.

**Left Click Twice to Scene**: Load saved scene selected.

**Left Click once in Model or Texture**: Open Importing options window properly for each resource.

**Left Click Delete Asset and Folder or File selected"**: Deletes item and their resources.


#### -Drag and Drop into application
---
~~~

Drop a mesh or texture and drop inside engine will create a duplicate file into Assets folder and resource info will be imported and structured to Library folder as resources.

~~~~
Supported file extensions:

###### **[CAPS DETECTION ENABLED]**

Meshes:

* .fbx

Textures:

* .jpg
* .png
* .tga

## Additional comments

**Important**: 

-If library folder deleted remember that old scenes from assets cannot be loaded with same information because resources UID changes. You are suposed to not delete lib folder as common user.

-Texture folder in resource manager generate one corrupted duplicate file(Left side one) known as "building 06_ c ". You cant place that texture into a channel diffuse.

-Delete gameobjects not implemented due several errors.

-If library folder deleted once( and properly ".metas" in asset folder) , first time you init the engine will reimport and generate once again library folder from 0, but materials couldnt load texture resources. Second time you open the engine after regenerate library will load textures and will be visible in your models.

-As We wrote before, keep in mind meta files when you delete library file to regenerate.

-If gameobject scaled rotations will reset but just if rotations are set as Global. Works properly in Local.

-Importing options generates devIL Yellow/Black textures to our resourceTexture.

-Scene Enviroment have been modified in maya to have Dummy001 a freezed group and deleted node targets for cameras and lights.

-Component Camera does have settings in inspector when selected but, main Viewport camera does have settings into Window->Configuration->Camera.

-Memory mismanagement at CleanUp() with resources. Destructor looks like calling map to unload each resource but Memory Dumps show different info.

-Primitive creation options doesnt work due to Refactor code about meshes working as resources.

## Windows
---

Brief description about what does each UI element.

### FILE

**Save**: Will save current scene into a Json file that will be stored into folder Assets/Scenes. You can change scene name in Hierarchy window.

**Exit**: Close Engine.

---

### GAMEOBJECT

**Create Empty GameObject**: Creates an Empty GameObject that only haves Component Transform.

**3D Objects**: Generate primitives
~~~
- Cube
- Pyramid
- Sphere
- Cylinder
~~~~
---
### WINDOW

**Examples**: Enable/Disable ImGui demo window.

**Workspace Style**: Change the Engine Style:
~~~
- Dark
- Classic
- Light
- Custom

~~~

**Hierarchy**: Enable/Disable the Hierarchy Window that contains all gameObjects in scene.

**Inspector**: Enable/Disable the Inspector Window and displays gameObject info if selected.

~~~
- Component Transform:
    - Position
    - Rotation
    - Scale

- Component Mesh:
    - Active or Disable Mesh Draw
    - Mesh File
    - Vertex Normals
    - Index
    - Vertexs
    - UV Coords
    - Show Vertex Normals
    - Show Face Normals

- Component Material:
    - Texture File
    - Width
    - Height
    - Show Checkers
    - Active or Disable Texture Draw
    - Preview of Checkers and Texture

~~~

**Scene**: Enable/Disable scene Window.

**Game**: Enable/Disable game Window.

**Console**: Enable/Disable the console where all the Logs and initializations of librarys are showed.

**Configuration**: Enable/Disable Configuration window

**Project**: Enable/Disable ResourceManager Window.

**Reference Count**: Enable/Disable Reference count Window.

**Importing options**: Enable/Disable Import options for mesh/textures

#### Application:
~~~
    - App Name
    - Organization
    - Framerate
    - Frames Per Second Graph
    - Milliseconds Graph
    - Active VSYNC
~~~
#### Window:
~~~
    - Brightness
    - Width
    - Height
    - Refresh Rate
    - FullScreen
    - Resizable
    - Borderless
    - Full Desktop
~~~
#### Hardware:
~~~
    - SDL Version
    - Cpus
    - System RAM
    - Caps
    - Gpu
    - Brand
~~~
#### Render:
~~~
    - Depth Test
    - Cull Face
    - Lightning
    - Color Material
    - Texture Draw
    - Wireframe Mode
~~~

#### Input:
~~~
	- Mouse Position
~~~
---
### HELP

**Documentation**: Redirects to Zero Engine Documentation.

**Download Latest**: Redirects to Zero Engine Releases.

**Report Issue**: Redirects to Zero Engine Issues.

**About**: Enable/Disbale About window with some info about the Engine.


## -Libraries Used
---
~~~
-SDL v2.0.12
-Glew v2.1.0
-OpenGL v3.1.0
-ImGui v1.78
-ImGuiGuizmo.
-MathGeoLib v1.5
-PhysFS v3.0.2
-DevIL v1.7.8
-Assimp v3.1.1
~~~

## License
---

~~~
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
~~~

##
![University Logo](https://www.citm.upc.edu/templates/new/img/logoCITM.png?1401879059)   
