//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Matrices										 //
//																		 //
//		$Description:	Shows a basic introduction to OpenGL Matrices.	 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
This tutorial goes over the modern OpenGL method of rendering your 3D data and having to handle
all the matrix math yourself.  Up until this point we have been drawing a triangle in Clipping
Coordinates, but now we will show how to create and use the Projection, View and Model matrices
to render a triangle in 3 different positions.  It is important to note that OpenGL 3.0+ no 
longer let's you do the following:

1) No more glTranslatef(), glRotatef() to move objects around.
2) No more glPushMatrix()/glPopMatrix() to limit movements within a scope.
3) No more glMatrixMode(GL_PROJECTION)/glMatrixMode(GL_MODELVIEW) to set the projection, model
   and view matrices.  In fact, OpenGL doesn't know anything about the matrices and just cares
   about the final Clipping Coordinates that we pass into gl_Position of a vertex shader. 
4) No more gluPerspective for setting our projection matrix with OpenGL.
5) No more using gl_ModelViewProjectionMatrix in your shaders, you need to pass your own in.
   
As you can see, this means we are responsible for 100% of the creation and calculation of the
matrices in the modern OpenGL API, and coming up with the final Clip Coordinates to give to
OpenGL.  At first this seems a bit intimidating, where we were comfortable with OpenGL
doing all this math and calculations for us and we just focused on the high-level fun stuff.
The good news is, this forces us to truly understand the 3D pipeline, gives us more power
and standards within shaders, and thankfully there is a library that makes this work easier 
for us called the GLM library (OpenGL Mathematics).  

*What are Matrices*
A Matrix is a rectangular array of numbers layed out in rows and columns, and are used in 3D
to transform points in 3D space, as well as projecting the points to 2D so they can be
displayed on the screen.  In 3D we generally represent matrices in a 4x4 matrix like so:

[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]

The above matrix is called the Identity Matrix.  The Identity Matrix is special in that if you
multiply it by a vector (like an (x,y,z) vertex) it results in the same vector:

[ m1 m5 m9  m13 ]	[x]   [ m1*x + m5*x + m9*x  + m13*x ]
[ m2 m6 m10 m14 ] * [y] = [ m2*y + m6*y + m10*y + m14*y ]
[ m3 m7 m11 m15 ]   [z]	  [ m3*z + m7*z + m11*z + m15*z ]
[ m4 m8 m12 m16 ]   [1]	  [ m4*1 + m8*1 + m12*1 + m16*1 ]

Example:

[ 1 0 0 0 ] [1]   [ 1*1 + 0*1 + 0*1 + 0*1 ]   [1 + 0 + 0 + 0]   [1]
[ 0 1 0 0 ] [2] = [ 0*2 + 1*2 + 0*2 + 0*2 ] = [0 + 2 + 0 + 0] = [2]
[ 0 0 1 0 ] [3]	  [ 0*3 + 0*3 + 1*3 + 0*3 ]	  [0 + 0 + 3 + 0]	[3]
[ 0 0 0 1 ] [1]	  [ 0*1 + 0*1 + 0*1 + 1*1 ]	  [0 + 0 + 0 + 1]	[1]

In OpenGL matrices use the column-major format, which as you can see in the example above,
that means that the order goes from top to bottom in each column starting in the top left. 
This is represented in a multi-dimensional array like matrixArray[4][4]. The point of matrices 
is that you can store transformation information like translations, rotations and scale and 
then multiply that matrix by a vertex or combine them by multiplying 2 matrices together.  Let 
us examine the inner details of a transform matrix.

A translation in a matrix for x, y and z are stored in these slots:

[ 1 0 0 x ]
[ 0 1 0 y ]
[ 0 0 1 z ]
[ 0 0 0 1 ]

The scaling slots are along the diagonal (scaling of x, y and z):

[ x 0 0 0 ]
[ 0 y 0 0 ]
[ 0 0 z 0 ]
[ 0 0 0 1 ]

If we put them all together we have 1 matrix that does scaling and translation where the
x,y and z are the scale value, and x2, y2 and z2 are the translation values:

[ x 0 0 x2 ]
[ 0 y 0 y2 ]
[ 0 0 z z2 ]
[ 0 0 0 1  ]

The rotation part is a bit more complicated since it uses cosine and sine functions, and it 
changes depending on the axis:

	 X-Axis Rotation				  Y-Axis Rotation			      Z-Axis Rotation
[ 1  0       0       0 ]		[ cos(θ)  0  -sin(θ)   0 ]		[  cos(θ)  sin(θ)  0   0 ]
[ 0  cos(θ)  sin(θ)  0 ]		[ 0       1   0        0 ]		[ -sin(θ)  cos(θ)  0   0 ]
[ 0 -sin(θ)  cos(θ)  0 ]		[ sin(θ)  0   cos(θ)   0 ]		[  0       0       1   0 ]
[ 0  0       0       1 ]		[ 0       0   0        1 ]		[  0       0       0   1 ]

The rotation matrices can also be combined with the scale and translation matrix to create the
final Model Matrix.

*GLM (OpenGL Mathematics Library)*
GLM is a header only library, which means you don't need to compile any DLL's or libraries, 
and it is platform independent.  In this tutorial we added the GLM library in our Lib\ folder, 
and made sure our project added the Lib\ folder as an "include" directory.  For users not using 
Visual Studio to build these tutorials you will need to add this in your own compiler 
settings/projects.  GLM mimics the GLSL shader language in terms of variables and functions,
so the nice thing is if you know GLSL, you know GLM.  The variables like vec3 and mat4 for
vertices and matrices are used in GLM as they are in GLSL.  If you look in the Lib\glm\ folder 
you will see it has dozens of files that allow us to do every core 3D mathematics that we need.

*Using Matrices in 3D*
We went through this in the First OpenGL Program tutorial, but we will cover it again because
it is important to understand why we do what we do with matrices to turn a 3D (x,y,z) point 
into a 2D (x,y) position on the screen (or off the screen if not in view).  Below is the 
process that a 3D point (a vertex), must go through in order to get to the final screen 
coordinates:

Vertex (Object Coords) x [Model Matrix] = World Coords x [View Matrix] = Eye Coords x
[Projection Matrix] = Clipping Coords x [Perspective Division] = Normalized Device Coords x
[Viewport Transformation] = Screen Coords (2D Coords)

-Vertex-
The pipeline above is showing that first we start with a Vertex, which is an xyz point in 3D 
space. This could be from a model made in a 3D application like Maya / 3DS Max, or our triangle 
points.

-Model Matrix-
The Vertex is then multiplied by a Model matrix which positions the point in World Coordinates.  
This would be any translation, rotation or scaling.  For instance, you want to position a 
couch in a room, the couch was creating in Maya centered around the origin (0, 0, 0), but 
needs to be positioned in the room, this is the now in the World Coordinates.  We use GLM to 
create our Model matrix by calling the "translate()" function and passing in an Identity Matrix 
the model's Position vector.  This generates a 4x4 translation matrix for us with the x,y,z 
translation we gave it.  Note that in GLM mat4(1.0) returns an identity matrix.

						   Starting Matrix     Translation
mat4 modelMatrix = translate(mat4(1.0f),       Position);

-View Matrix-
The Vertex is then multiplied by a View matrix which then turns the vertex into Eye Coordinates.  
The view matrix is created by calculating the inverse of a camera matrix. , which stores the 
position, target location and up vector.  A camera is described as a pinhole where it is a 
single point (the location of the camera).  We use GLM to create our view matrix since 
gluLookAt() is deprecated:

							  Camera Position			  Target				Up Vector
mat4 viewMatrix = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

-Projection Matrix-
Next, the Vertex is multiplied by a Projection matrix to produce the Clipping Coordinates.  A 
Projection matrix is created by a field of view (FOV) and near and far planes.  A field of 
view is usually between 60-120 degrees. The FOV and near/far planes create what is called a 
frustum that determines what the camera can see.  Think of the frustum as a pyramid coming out 
of the camera with the top cut off.  Anything outside of the frustum is clipped and not rendered 
on the screen.  The frustum is used in space partitioning methods as well like a BSP tree, 
octree or quad tree so that you don't send objects to the GPU if you know they aren't even 
going to be seen.  The Clipping Coordinates are a box that is between -1 and 1 on the x, y, 
and z axis. In our vertex shader we use our matrices to calculate each point in Clipping 
Coordinates, and it is passed to the GLSL property, gl_Position.

In the old OpenGL, the projection matrix was created by the gluPerspective(), but the GLU 
library was deprecated and removed.  We use GLM to create the perspective matrix for us like so:

									 FOV		  Aspect Ratio		   Near   Far Planes
mat4 projectionMatrix = perspective(60.0f, ScreenWidth / ScreenHeight, 0.5f, 150.f);

-Perspective Division-
This is where the x, y and z values are divided by the W to make the vertices pull towards the 
center the farther the point is away from the camera.  The higher the W, the farther away the 
triangle will look since it becomes smaller.  Think about looking at railroad tracks that go 
off into infinity.  Notice the rails on the track slowly bend towards the center.  This effect 
is what the perspective division is creating to simulate the reality of objects getting smaller 
and closer to or farther away from the center depending on their distance from the camera.  This 
creates the Normalized Device Coordinates, which is the final coordinate system before turning 
the points to the 2D Screen Coordinates.  We don't do this calculation, and OpenGL does this
once it receives our Clipping Coordinates in gl_Position in the vertex shader.

-Viewport Transformation-
This is the final transformation that takes the specified viewport we give OpenGL which is
glViewport(startX, startY, width, height), and converts the Vertex to Screen Coordinates.

*Passing Matrices into Vertex Shaders*
Now that we have a way to calculate our model, view and projection matrices, we need to pass
them into our Vertex Shader to calculate the vertex position.  We do this by adding the
matrices as "uniform" parameters in the Vertex Shader, which are variables that stay the
same during the entire rendering call and can't be changed within the shader:

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

We then do the matrix multiplication by going from right to left since order is important:

gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;

It is important to note that matrix multiplication is not generally commutative which means
A * B  does not necessarily equal B * A, so the order is important.  Matrix multiplication is 
associative though, which means A * (B * C) = (A * B) * C.

Finally, in order to pass in a matrix to a Vertex Shader, you need to use glUniformMatrix4fv:

glUniformMatrix4fv(shaderId, count (which is just 1), GL_FALSE (to not transpose it), matrix);
*/

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


Model g_Triangle;										// Our class to handle initializing and drawing our triangle


// This is our own main() function which abstracts the required main() function to run this application.
int GLApplication::GLMain()
{
	// This calls our Initialize() function below which creates the window and triangle
	Initialize();

	// This is our main game loop which will run until we close the window or hit Escape.
	GameLoop();

	// Once we hit Escape this will clean up the application's resources.
	Destroy();

	return 0;
}


// This function initializes the window, the shaders and the triangle vertex data.
void GLApplication::Initialize()
{
	// Make sure the window manager is initialized prior to calling this and creates the OpenGL context
	if ( !WindowManager || WindowManager->Initialize(ScreenWidth, ScreenHeight, "GameTutorials - Matrices", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Tell OpenGL that we want a 3D viewport the same size as the window
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	
	// This tells OpenGL that we want depth testing so it renders the order correctly.  If
	// we didn't do this it would render the back triangles in front of the front one.
	glEnable(GL_DEPTH_TEST);

	// Create the projection matrix with GLM's perspective function, replacing gluPerspective.
	//												  FOV		    Aspect Ratio				Near / Far Planes
	mat4 projectionMatrix = perspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.5f, 150.f);

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Initialize the three XYZ points of the triangle as their associated Green, Red and Blue
	// colors.  We give an alpha value of 1.0, which gives the colors an opacity of 100%.
	Vertex3 vertices[] =
	{	//    X 	  Y	    Z      	   R	  G	   B	 A
		{ { -0.6f,  -0.6f, 0.0f },  { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.0f,   0.6f, 0.0f },  { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.6f,  -0.6f, 0.0f },  { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	// Initialize the triangle with the vertex array and give the length of 3.  This will create our
	// Vertex Buffer Object and Vertex Array Object.
	g_Triangle.Initialize(vertices, 3, "Shaders/Shader.vertex", "Shaders/Shader.fragment");


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Pass in the projection matrix to our model so we can pass it to the vertex shader
	g_Triangle.SetProjectionMatrix(projectionMatrix);

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


// This is our game loop where all the magic happens every frame
void GLApplication::GameLoop()
{
	// Loop until the user hits the Escape key or closes the window
	while ( WindowManager->ProcessInput(true) )
	{


		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Create the view matrix with the GLM lookAt function, which is created with our camera's 
		// position, target that it is looking at, and the update vector which orients the camera.
		//							  Camera Position			  Target				Up Vector
		mat4 viewMatrix = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

		// This clears the screen every frame to black (color can be changed with glClearColor).
		// Since we turned on Depth Testing, we need to clear the color and depth buffer every frame.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Pass in the view matrix to the model so it can be used in the vertex shader
		g_Triangle.SetViewMatrix(viewMatrix);

		// Set the position of the first triangle to be at the origin
		g_Triangle.SetPosition(vec3(0, 0, 0));

		// Render the first triangle
		g_Triangle.Render();

		// Move the position to the right and back one
		g_Triangle.SetPosition(vec3(1, 0, -1));

		// Render the second triangle with the new position
		g_Triangle.Render();

		// Move the position to the left and back one
		g_Triangle.SetPosition(vec3(-1, 0, -1));
		
		// Render the third triangle with a new position
		g_Triangle.Render();

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Swap the buffers to display the final rendered image on screen
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
// This tutorial shows you how to create and use matrices to calculate the 3D
// positions from a model, view and projection matrix.  We use the GLM library
// to do the matrix math for us.  Feel free to look at the code to find out how
// to calculate the matrices yourself.  Below is how to calculate the matrices.
//
// Model Matrix:
//
// mat4 modelMatrix = translate(mat4(1.0f),       Position);
//
// View Matrix:
//
// mat4 viewMatrix = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
// 
// Projection Matrix:
//
// mat4 projectionMatrix = perspective(60.0f, ScreenWidth / ScreenHeight, 0.5f, 150.f);
//
// Putting it all together in the Vertex shader:
//
// gl_Position = projectionMatrix * viewMatrix * modelMatrix * in_Position;
//
// We also went over that matrix multiplication order matters since A * B doesn't always equal
// B * A (it's not commutative but associative).  OpenGL uses the column-row order for
// matrices.  To pass in a matrix to a Vertex Shader we use the glUniformMatrix4fv function.
//
// glUniformMatrix4fv(shaderId, 1, GL_FALSE, matrix);
//
//
// © 2000-2014 GameTutorials