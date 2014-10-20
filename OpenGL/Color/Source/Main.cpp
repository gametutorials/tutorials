//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Color											 //
//																		 //
//		$Description:	Draws a colored triangle in OpenGL 4			 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
This is the next tutorial in the modern OpenGL series which builds upon the previous tutorial
where we displayed a white triangle on the screen with OpenGL 4.  In this tutorial we will be
passing in colors to the GPU in order to color the triangle.  Now that we have a great base
for our modern OpenGL application, adding colors is relatively simple.

1) First we add a rgba property to our Vertex3 class, so that we can pass in a Red, Green,
   Blue and Alpha value along with each vertex of our triangle.

2) We then need to add the color attributes to our Vertex Array Object and Vertex Buffer Object.
   We do this by adding the size and location of the rgba property via glVertexAttribPointer():
   
   glVertexAttribPointer(kColorIndex, 4, GL_FLOAT, GL_FALSE, sizeof(Vertices[0]), (GLvoid*)sizeof(Vertices[0].xyz));
  
   This takes an index to be referenced in the vertex shader (kColorIndex = 1), the number of
   floats (4 for r,g,b,a), the type which is a GL_FLOAT, we don't want it normalized so GL_FALSE,
   the size of a single Vertex3 object, and then a void pointer to the start of the location
   within the Vertex3 class to the rgba property, which is right after the xyz variable.  By
   giving a pointer to the size of the xyz variable, we give the offset in bytes into the class.

3) Now that we have a new index attribute for color, we need to enable it with the kVertexIndex
   by calling glEnableVertexAttribArray(kColorIndex), and then disabling it with a call to
   glDisableVertexAttribArray(kColorIndex).

4) Finally, now that we are passing in colors to the shaders, we need to create a new in_Color
   property that will then be passed over to the fragment shader for that vertex:

   layout(location=1) in vec4 in_Color;

   The location=1 maps to the same location index that we used to pass in the color attributes
   with kColorIndex.  We then create and set an out variable within the vertex shader to pass
   the fragment shader, since we have to pass any variables through the vertex shader and can't
   pass them directly to a fragment shader.
   
   out vec4 frag_Color;

   void main(void)
   {
	   ...
	   frag_Color = in_Color;
   }

5) Finally, we need take the frag_Color that was sent over to the fragment shader and instead
   of hard coding our color like last time, we set the out_Color to our frag_Color.

   in vec4 frag_Color;
   layout(location=0) out vec4 out_Color;

   void main(void)
   {
	   out_Color = frag_Color;
   }

These are the only changes (other than a window title) that have changed since the later
tutorial, which can be found with NEW blocks in Main.cpp, Model.cpp, Model.h and the shaders.
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
	// Make sure the window manager is initialzed prior to calling this and creates the OpenGL context
	if ( !WindowManager || WindowManager->Initialize(1024, 768, "GameTutorials - Color", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}

	
	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Initialize the three XYZ points of the triangle as their associated Green, Red and Blue
	// colors.  We give an alpha value of 1.0, which gives the colors an opacity of 100%.
	Vertex3 vertices[] =
	{	//    X 	 Y	   Z      	 R	   G	 B	   A
		{ { -0.6f, -0.6f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.0f, 0.6f, 0.0f },   { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.6f, -0.6f, 0.0f },  { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Initialize the triangle with the vertex array and give the length of 3.  This will create our
	// Vertex Buffer Object and Vertex Array Object.
	g_Triangle.Initialize(vertices, 3, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
}


// This is our game loop where all the magic happens every frame
void GLApplication::GameLoop()
{
	// Loop until the user hits the Escape key or closes the window
	while ( WindowManager->ProcessInput(true) )
	{
		// This clears the screen every frame to black (color can be changed with glClearColor).
		glClear(GL_COLOR_BUFFER_BIT);
		
		// This renders our triangle to the back buffer (not the screen yet).
		g_Triangle.Render();

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
// In this tutorial we add colors to our Vertex3 class and pass them to our shaders.
// The next tutorial will go over the more complicated topic of matrixes, which
// will allow us to move our objects around and calculate the final projections
// instead of working in Clipping Coordinates like done here.
// 
//
// © 2000-2014 GameTutorials