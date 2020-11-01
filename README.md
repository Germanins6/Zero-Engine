## Zero Engine
---

Zero Engine is a 3D Game Engine developed by "Germán Insua" and "Christian Piña" as a task for the subject 3D     
Engines of the Degree in Videogames Design and Development of "Universitat Politénica de Catalunya".


### Zero Engine Repository Link

https://github.com/Germanins6/Zero-Engine

### -Team members
  -Germán Insua Perdomo
   
    [Github account]: https://github.com/Germanins6

 -Christian Piña López
    
    [Github account]: https://github.com/christianpi4

## -How to use
---

#### **FUNCTIONALITIES**

#### [Changelog]
* v0.1: [Latest version]
  
  With this version the 3D Engine can just show geometries and display textures (*just in diffuse channel*).
  
  Drag and drop files allowed for .obj and .fbx. **[Make sure .fbx encryption ASCII instead Binary]**.
  Create Empty objects and primitives.
  
  Drag and drop .png and .jpg files will load into geometry selected.
  
  Show all gameobjects in scene and display its info.
  
  Dock enabled UI Unity Like.
  
  ---
 
#### **CONTROLS**

#### -Non-Selected GameObject controls.
---


**WASD** : Camera movement in scene.

**Right Mouse Click**: Orbit camera.

**R**: Moves camera up vertically .

**T**: Moves camera down vertically .

**Shift**: Increase camera speed.

**Mouse Wheel**: Zoom In/Out.

#### -GameObject controls if selected.
---

**F**: Centers Camera to GameObject.

**Alt + Mouse Right Click**: Orbital camera around gameObject.

**B**: Turntable around gameobject automatically.

#### -Drag and Drop into application
---
~~~

Drop a mesh will load its info, at the same time if your loaded mesh does have any material info and you have
the propper resources into textures folder allows the app creating the material and load the texture into the mesh.

Individually you can also drag and drop into a loaded mesh a texture file and this will be applied automatically

**Important**: Texture loading generates memory leaks in engine runtime. Still no cleaning
 properly in execution. Keep in mind this will be fixed in our next versions. Also keep in mind that if you 
 use larger files this bug will be accentuated.

~~~~
Supported file extensions:

###### **[CAPS DETECTION ENABLED]**

* .fbx
* .obj

Textures:

* .jpg
* .png

## Additional functionalty

-Hierarchy window does have a button that can clean the gameObjects currently placed in the scene.

-Drag and drop into selected gameObject with a Non-material mesh will create the component and update the gameObject.

-FrameBuffer created , window render all the stuff into scene window.

## Additional comments

**Important**: 
Texture loading generates memory leaks in engine runtime. Still no cleaning

properly in execution. Keep in mind this will be fixed in our next versions. 

Also keep in mind that if you use larger files this bug will be accentuated.


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
~~~
-SDL v2.0.12
-Glew v2.1.0
-OpenGL v3.1.0
-ImGui v1.78
-MathGeoLib v1.5
-PhysFS v3.0.2
-DevIL v1.7.8
-Assimp v3.1.1
~~~

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
