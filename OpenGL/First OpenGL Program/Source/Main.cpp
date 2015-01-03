//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		First OpenGL Program							 //
//																		 //
//		$Description:	Init OpenGL 4 and draw a triangle to the screen	 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application

/*
Welcome to the first tutorial on OpenGL.  OpenGL is an API for programming 3D, and is
used to develop applications from games to medical visualizations.  This tutorial shows 
you how to setup a basic OpenGL application and draw a triangle in 3D space.  This is the
first of the updated OpenGL tutorials that replaces the old OpenGL 2.0 tutorials with the
newer OpenGL 4.0.  You will need to make sure you have the latest drivers to run this
tutorial, which links can be found here:  https://www.opengl.org/wiki/Getting_Started.
You don't need to download OpenGL, but since OpenGL 1.1, you update your drivers and
that is how you get the latest versions, and the GLEW library will link the functions.

*WARNING*
For those who are fairly inexperienced in programming and think jumping into 3D development 
is going to be possible, I'd strongly caution you to go back and get a lot of experience 
building ASCII and then 2D games.  3D development is one of the hardest programming you can 
do as a developer and you will get frustrated if you don't already have a strong knowledge
of programming.

*Third-party libraries used*
I spent hours going back and forth trying to decide if I should eventually buckle and stop using
Win32 for the OpenGL tutorials so that Mac and Linux users could also benefit from the tutorials,
For a long time GLUT or SDL were the ones used but after researching third-party library option
I found one called GLFW that also allows cross-platform development, and has become quite popular.
I was impressed with the adoption and usability and decided to change all the OpenGL tutorials
that I make going forward to use GLFW.  In the effort to try and abstract the library used to 
created the window and handle input, I created the WindowManager abstract base class to take care 
of all the window and input code that GLFW does for us.  The GLFWManager then inherits this base 
class so that the GLApplication has no idea about what API is being used to handle the window and 
input.  By using CMake and following the directions on the GLFW website, you should be able to 
build this project on Mac OSX or Linux. More details and downloads here: http://www.glfw.org/.
I also built a Win32Manager as well for those who still want to work with Win32.

In order to be able to call any functions in OpenGL after version 1.1 you have to use function
pointers and query your graphics card drivers to see which extensions or latest functions the
card supports.  I manually did this in our older OpenGL tutorials, but since the newer OpenGL
requires these new functions to do anything now days, I also decided to use the GLEW library.
This library handles loading the supported extensions and assigning the functions for you.  
For example, glCreateShader() doesn't exist in the default gl.h header file, or any other
shader or new functions, so GLEW creates and initializes the function pointers for us.
It also has some cross-platform functionality that benefits Mac and Linux users as well.
So that this project compiles without the need for you to download and reference any externals,
the GLEW and GLFW libraries and include files are include in the Lib\ folder of this project.
More details on building or using GLEW can be found here: http://glew.sourceforge.net/ 

*C++ 11 and more Object Oriented Programming*
Any new updates to the tutorials will now try and utilize C++ 11 features and will attempt
to use more Object Oriented Programming implementations and less C-style development.

*3D programming introduction*
3D programming is the most enjoyable and exciting programming in the world to me.  I remember
making my first 3D program in OpenGL where it displayed a triangle and I could use the up and
down arrow keys on the keyboard to move the camera back and forth and see the triangle get closer
and farther away.  To this day I still remember the excitement I had when I got that working,
and though it seems silly that such a simple program would be so exciting, it was the realization
that I was opening the door to be able to create an awesome 3D game or application in the future. 
Also, as I started getting into collision detection and other 3D concepts, I gained a new
appreciation of the applications of math like linear algebra.  It's one thing to learn math out
of a book and never use the principles, but it's another to actually use the advanced math to
allow you to create amazing 3D worlds.

There is a lot to learn in 3D, but my hope is to get you started on the basics. Hopefully you have
had some basic experience with 2D so that you understand how an X and Y coordinate system work, which
has the y-axis vertical and the x-axis horizontal and an origin of (0,0) in the middle where x and y
intersect:
                        Y
						|
						|
			  -X________|________X
						|
						|
						|
					   -Y

The 3D space is similar to the 2D space, except that there is an extra z-axis that takes the space
from a flat system seen in Nintendo games to a more realistic space like we see in the real world
where there isn't just a left, right, up and down, but a third dimension that goes in and out
of the screen:
                  -Z \  Y
					  \ |
					   \|
			  -X________|________X
						|\
						| \
						|  \
					   -Y   \ Z

If you were to stick your finger straight on the screen and then push into the screen, if you were
to imagine your finger going into the screen, you would be going down the -z-axis, where the positive
z-axis would be coming out of the screen towards you.  If you have a hard time understanding these
basics of a 3D axis, please download a 3D application like Unity3D, Maya or 3DS Max and add a cube
in the scene around the axis and then move it around.  Using Adobe Photoshop to move images around
is the 2D experience where it's just up, down, left and right, where a 3D application allows that
extra axis like you would move things around in real life.

Objects in 3D are built from a bunch of 3D points which are rendered as points, lines or triangles.
The video card Graphics Processing Unit (GPU) natively renders in triangles, and QUADS (4 pointed polygons) 
have been removed as an option in OpenGL 3+ (at least for glDrawArrays()), and was not an option in 
OpenGL ES 2.0 (mobile version) for efficiency.

*3D to 2D projection*                                                                                                                                                                           
In 3D rendering, since our computer screens aren't a hologram and are 2D, the 3D scene must be
finally converted to a 2D image.  This is done through a projection, a 3D to 2D projection.  In order
for a 3D view to be projected, there is the transformation pipeline that the 3D points must go through:

Vertex (Object Coords) x [Model Matrix] = World Coords x [View Matrix] = Eye Coords x 
[Projection Matrix] = Clipping Coords x [Perspective Division] = Normalized Device Coords x 
[Viewport Transformation] = Screen Coords (2D Coords)

-Vertex-
The pipeline above is showing that first we start with a Vertex, which is an xyz point in 3D space.  That
could be from a model made in a 3D application like Maya or 3DS Max.

-Model Matrix-
The Vertex is then multiplied by a Model matrix which positions the point in World Coordinates.  This would be
any translation, rotation or scaling.  For instance, you want to position a couch in a room, the couch was
creating in Maya centered around the origin (0, 0, 0), but needs to be positioned in the room, this is the
now in the World Coordinates.

-View Matrix-
The Vertex is then multiplied by a View matrix which then turns the vertex into Eye Coordinates.  The view
matrix is created by calculating the inverse of a camera matrix. , which stores the position, target location 
and up vector.  A camera is described as a pinhole where it is a single point (the location
of the camera).

-Projection Matrix-
Next, the Vertex is multiplied by a Projection matrix to produce the Clipping Coordinates.  A Projection matrix  
is created by a field of view (FOV) and near and far planes.  A field of view is usually between 60-120 degrees
The FOV and near/far planes create what is called a frustum that determines what the camera can see.  Think of 
the frustum as a pyramid coming out of the camera with the top cut off.  Anything outside of the frustum is 
clipped and not rendered on the screen.  The frustum is used in space partitioning methods as well like a BSP tree, 
octree or quad tree so that you don't send objects to the GPU if you know they aren't even going to be seen.
The Clipping Coordinates are a box that is between -1 and 1 on the x, y, and z axis. You'll notice that since we 
don't use any matrices in this tutorial we needed to display a triangle within those bounds.  More about this later.

In the old OpenGL, the projection matrix was created by the gluPerspective(), but the GLU library was deprecated 
and removed so like all the other matrices, OpenGL expects you to create your own matrices.  In later tutorials 
(Matrices) we will go over creating this using the GLM library (OpenGL Mathematics).   

-Perspective Division-
This is where the x, y and z values are divided by the W to make the vertices pull towards the center the
farther the point is away from the camera.  The higher the W, the farther away the triangle will look since
it becomes smaller.  Think about looking at railroad tracks that go off into infinity.  Notice the rails on
the track slowly bend towards the center.  This effect is what the perspective division is creating to
simulate the reality of objects getting smaller and closer to or farther away from the center depending
on their distance from the camera.  This creates the Normalized Device Coordinates, which is the final
coordinate system before turning the points to the 2D Screen Coordinates.

-Viewport Transformation-
This is the final transformation that takes the specified viewport we give OpenGL which is
glViewport(startX, startY, width, height), and converts the Vertex to Screen Coordinates.

*What changed from the older OpenGL?*
For those who are familiar with the old way of doing things in OpenGL, I'll attempt to briefly go over what
has changed after OpenGL 3.0.  The old way of doing things in OpenGL was called a Fixed Function Pipeline,
which means that OpenGL exposed functions to do all of the 3D management from creating the matrices, setting
the lighting, etc.  Having this pipeline proved difficult to do many advanced rendering and eventually in
OpenGL 3.0 the old functions were deprecated, and then in 3.1 they were removed from the core.  You can still
use the old functions if you are using the older versions of OpenGL (Windows still uses 1.1).  The GLU library
was also removed, so this means that you have to calculate all your lighting and matrices to pass into shaders.
There is no more glBegin()/glEnd() functions to draw objects, but you need to use Vertex Buffer Objects and
Vertex Array Objects in conjunction with glDrawArrays().  No more passing in vertices one at a time, which was
slow anyway.  

Even shaders have changed from the older GLSL shaders which had properties like gl_ModelViewMatrix and 
gl_ProjectionMatrix that was using the old OpenGL pipeline to store your model, view and projection matrices.   
Since OpenGL 3.0+ expects you to now pass in your own matrices for everything, these older properties were
removed. This sounds annoying, but is really terrific because it now removes much of the dependency on OpenGL 
in your application's code and gives more power to your shaders.  This also helps makes GLSL easier to abstract
if you are trying to make a cross-platform renderer like the Unreal Engine.

-Shaders-
If you haven't ever worked with shaders, let's go over a brief overview of what they are and what they can do,
and then later you will want to check out the "Shaders\" folder to get a line-by-line instruction on each shader.
There are multiple types of shaders such as vertex shaders, fragment shaders (pixel shader), geometry, tessellation
and compute (CUDA) shaders.  In this tutorial we will just be going over the base shaders which are the vertex
and framework shaders.  Shaders allow us to run programming instructions directly on the GPU of your video card
which is optimized to process math, physics and rendering instructions incredibly fast.  You pass in shaders as
string into openGL's glShaderSource() function, and then call the glCompileShader() function to compile it at the
start of our application.  There are a few other steps, which we go over in the Shader.cpp file.

A vertex shader is where you calculate the final position for every vertex in Clipping Coordinates.  You will be 
setting the gl_Position parameter with an xyzw value for a vertex.  I chose to name the vertex shader as 
Shader.vertex, but you can name it whatever you want.  OpenGL just needs a string to create a shader so whatever 
method works to just load the string (or even hard code it in your application).  The vertex shader also passes 
variables to the fragment shader like color, texture coord and normal data for the fragment shader to use for 
it's per-pixel operations.  

A fragment shader handles the per-pixel operations and allows you to run calculations for every pixel on the
screen.  This sets the final RGBA color for a pixel, and is used extensively to calculate lighting and shadows.
One of the most popular uses for a fragment shader is to calculate beautiful lighting, especially when used with
a normal map, which will be explained in our Normal Mapping tutorial.  In a framework shader you give an output
variable that is the final RGBA value.  This is then passed to the OpenGL pipeline to process.  In this tutorial
we won't be passing in any color but hard coding the triangle to white.  The next tutorial will pass in colors.

-Vertex Buffer Object-
If you are used to doing glBegin()/glEnd() for your rendering, you can kiss those days goodbye.  You will need to 
use functions that will send buffers to OpenGL to pass to the GPU.  The is where Vertex Buffer Objects come into
the picture.  A Vertex Buffer Object (VBO) is used to pass in an array of your vertex, color, texture and normal
information to the GPU in order for them to be processed by OpenGL.  You can create a VBO for each one of those
individually, or do what we do in this tutorial and create a class (Vertex3) which has that information all packed
together so you can just pass in the whole thing into one buffer.  You then have to tell OpenGL what the size of
the buffer is and the size and where each different property is located in the class, known as the stride and offset.
This tutorial won't include anything but the vertices, but the next tutorial will add in the color to Vertex3.
The function we use for this are glGenBuffers(), glBindBuffer(), glBufferData() and glVertexAttribPointer(), which
more information and examples can be found in our Model.cpp file.

-Vertex Array Object-
A Vertex Array Object (VOA) goes together with the Vertex Buffer Objects (VBO) in that it saves the state of
the vertex information in the buffers, and for each vertex attribute passed in like color or texture coordinates,
you would call glEnableVertexAttribArray() and pass in the index associated with that property which which was
specified in the glVertexAttribPointer() call.  Refer to the Model.cpp file for an example of this.

-Project Layout-
In this project we created a few classes to make our OpenGL programming more organized and easier to swap out
window creation technologies like Win32, GLUT, SDL, etc..  This way if you want to change from GLFW to GLUT or
Win32, you can just inherit the WindowManager class to implement those specific window functions.  I would 
normally setup something for Dependency Injection but I didn't want to add too much complication to the tutorials.
Notice that our .cpp files go in the "Source Files\" folder, the .h files go in the "Header Files\" folder and 
the shader files have their own folder as well.  The tutorials will always start at the Main.cpp file so please 
look their first to start the tutorial.  This has a ton of things to go over so let's just jump in and take a look 
at the code. If you can get through this first tutorial, the rest should be much easier to swallow because we now
have a great base to just tweak going forward.
*/

Model g_Triangle;										// Our class to handle initializing and drawing our triangle


// This is our own main() function which abstracts the required main() function to run this application.
// This way if we want to run a Win32 application which requires a WinMain(), we can hide that in a Win32Manager,
// which our GLFWManager currently has a regular console application main() function that just immediately calls 
// this function after setting the WindowManager class. You can look to this function as the first thing that will 
// happen in our OpenGL applications that is outside the general one-time initialization for our window.
int GLApplication::GLMain()
{
	// This calls our Initialize() function below which sets up the creation of the window and initializes
	// the triangle vertices and associated shaders.
	Initialize();

	// This is our main game loop which will run until we close the window or hit Escape.
	GameLoop();

	// Once we hit Escape this will clean up the application's resources.  The same functions will be called
	// in the individual classes' deconstructors, but we do it anyway as a good practice in always cleaning up.
	Destroy();

	return 0;
}


// This function initializes the window, the shaders and the triangle vertex data.
void GLApplication::Initialize()
{
	// Make sure the window manager is initialzed prior to calling this and creates the OpenGL context.
	// This takes in a window width, height, title and a fullscreen boolean.
	if ( !WindowManager || WindowManager->Initialize(1024, 768, "GameTutorials - First OpenGL 4 Program", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}

	// Initialize the three XYZ points of the triangle.  This is our own class which stores the vertex
	// and later in other tutorials the associated color, texture coordinates and normals together.
	// We need 3 points to create a triangle in 3D.  We pass them in a clock-wise order.  The winding order 
	// of the vertices is incredibly important because it tells OpenGL what is the front and back side of
	// our triangle.  This comes into play when we turn on backface culling to only draw triangles facing
	// the camera.  This adds an optimization to your rendering if you know you aren't going inside objects.
	// Notice that the Z value is 0, so that means this will render right on the X,Y axis like it would in 2D.
	// The W column that the vertex shader's gl_Position needs is hard coded to 1 in the shader for now, and 
	// eventually is calculated for us when we do our matrix math in later tutorials (see the Matrices tutorial).
	Vertex3 vertices[] =
	{	//    X 	 Y	   Z   		
		{ -0.6f, -0.6f, 1.0 },		// Bottom-left vertex
		{  0.0f,  0.6f, 1.0 },		// Top vertex
		{  0.6f, -0.6f, 1.0 }, 		// Bottom-right vertex
	};

	// A note about the coordinates above, since we are not doing any matrix multiplication with a
	// model, view and projection matrix, we are just hard coding "Clipping Coordinates" that will be
	// passed into the gl_Position of our vertex shader.  This means that our coordinates can only
	// be between -1 to 1 along all the axises before being clipped because that's what the clipping 
	// coordinates are, a box made up of -1 to 1 on each axis. So if you wanted to make the triangle fill
	// the screen you would put the coordinates at (-1, -1, 0, 1), (0, 1, 0, 1), (1, -1, 0, 1).
	// Try and change the Z values to 1.01 and you will notice that it dissappears due to being clipped.
	// In a later tutorial we will go over matrices, which will then be taking the W value of the
	// vertices and dividing each the X,Y,Z by that W value to give the "Perspective Division".  I chose
	// to handle the W value in the shader and hard code it as 1.0 since we won't be using it in our code.
	// Notice that if you start changing the W values you can see the triangle get bigger and smaller.
	// This is kind of confusing, but just know that no one works in the Clipping Coordinate Space, we do
	// in this tutorial to just do the very basics to show a triangle, and later we will use matrices which
	// convert our model and view matrix to the clipping space for us.

	// Initialize the triangle with the vertex array and give the length of 3.  This will create our
	// Vertex Buffer Object and Vertex Array Object.
	g_Triangle.Initialize(vertices, 3, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
}


// This is our game loop where all the magic happens every frame
void GLApplication::GameLoop()
{
	// Loop until the user hits the Escape key or closes the window.  We created a ProcessInput function to
	// abstract the input from the main application flow so that we can make it easier for different 
	// environments.  We pass in true to always keep the loop running, but this could be replaced with a
	// custom boolean variable like bGameNotOver that could be set somewhere else like a menu system.
	while ( WindowManager->ProcessInput(true) )
	{
		// This clears the screen every frame to black (color can be changed with glClearColor).
		// Since our triangle isn't moving it doesn't really matter, but once you have moving objects
		// this is necessary otherwise the last frame will still have the old pixels and cause artifacts.
		glClear(GL_COLOR_BUFFER_BIT);
		
		// This renders our triangle to the back buffer (not the screen yet).
		g_Triangle.Render();

		// Now that we have told OpenGL to draw our white triangle, it isn't on screen yet until we swap
		// the back buffer with the front (visible) buffer.  This is so we don't see tearing artifacts
		// happening as we draw many objects to the screen during a single frame.  This is the same as drawing
		// objects in 2D, you don't swap the buffers until all the objects have been drawn on the screen.
		// Since each cross-platform framework has their own method for this, we abstract it in our own class.
		WindowManager->SwapTheBuffers();
	}
}


// This can be used to free all of our resources in the application.
void GLApplication::Destroy()
{
	// Free the vertex buffers and array objects
	g_Triangle.Destroy();

	// If we have a window manager still allocated then destroy and delete it
	if ( WindowManager )
	{
		WindowManager->Destroy();

		delete WindowManager;
		WindowManager = nullptr;
	}
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// So you've seen that the flow is to create our window and setup OpenGL,
// initialize our triangle with a vertex buffer and vertex array, load and compile
// the shaders, then render the triangle to a back buffer and swap buffers every 
// frame, then cleanup our resources once the user hits Escape or closes the window.
//
// Hopefully these are pretty straight forward concepts.  Be sure to look at the
// other .cpp files to see the implmentation of creating the window, shaders and
// model.  Each of the files have detailed explanations at the top.  Then I would
// look at the shader files in the Shaders\ folder to get more explanations on those.
// 
// The next tutorial will go over adding the ability to pass in colors to the shaders.
// 
//
// © 2000-2014 GameTutorials