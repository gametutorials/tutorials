//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersIntro									 //
//																		 //
//		$Description:	Using GLSL we create a wavy sphere in wireframe	 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// Welcome to the introduction of shaders.  You will find out from now on, as you
// continue to use shaders it is going to be a bit different than the programming
// you are used to; different but more powerful.  We will be using the GLSL shader
// language.
//
// * What are shaders? *
//
// Shaders are text files that are passed to the video card's GPU (Graphics Processing
// Unit), which enable extremely fast operations to be performed on each vertex and/or 
// pixel on the screen.  Basically, shaders allow you to speed up computationally
// expensive operations like lighting, reflection, refraction, bump mapping, etc.
// It also allows you to morph your geometry in a quick way, like in this tutorial.
// Instead of having your computer's CPU get bogged down with these calculations,
// we want to use shaders to have the greatest amount of efficiency by using the GPU.
//
// * Who can run shaders? *
//
// Shaders are used with video cards that have a GPU.  Most of the video cards that
// you buy now will have a GPU.  Back in the day, NVidia's Geforce 3 was one of the
// first video cards to implement this technology.  You can get a capable video card
// for extremely cheap now days.
//
// * OpenGL Shader text files *
//
// In OpenGL there are 2 shader files.  There is a vertex shader and a fragment shader.
// Usually you will find the shaders labeled as .vert for vertex shaders and .frag
// for fragment shaders.  These are just text files, so you can call them what ever
// you want.  We create our own function for loading the text file into a char array.
//
// Right now, quickly open the .vert and .frag files to see what they look like.
// You will notice that it looks like a programming file.  It has a main() and
// a scope (the curly brackets).  The syntax for shader files is like the C++ language.
//
// Keep in mind that in Direct3D there isn't two files, but one .fx file.  You can check
// out our Direct3D shader tutorials for the differences between OpenGL and D3D.
//
// The vertex (.vert) file handles each vertex on the GPU, where the fragment file handles
// every pixel drawn by 3D objects, also known as a "pixel shader".
//
// * What this tutorial does *
//
// In this tutorial we draw a quadric sphere and apply a vertex and fragment shader to it.
// The vertex shader ("morph.vert") makes a wavy effect on each vertice using a cosine
// function, as well as using a fragment shader ("morph.frag") to color the sphere red.
//
// Now, this example doesn't make a huge increase in speed, but we do send some pretty
// intense computations for the function cos() to the GPU instead of calculating it on
// our CPU.
//
// *What is the difference between the GLSL and CG shader language? *
//
// The GLSL shader language is the recognized shader language by the OpenGL standards board,
// and works on both ATI and NVidia graphics cards.  The CG shader language was the first
// language to come out for OpenGL and just works on NVidia cards, but works with D3D
// and OpenGL.  HLSL (High Level Shader Language) is for DirectX and is the standard.
//
// It is assumed that by reading this tutorial you understand OpenGL/3D programming
// and you have read through the basic OpenGL tutorials on our CD.
//
// We created a CShader.cpp and CShader.h file to handle our shader code.  This CShader
// class included in the files is very basic.  In this file we just init our GLSL
// functions and then start using shaders!  Let's go to the code.  Be sure to look
// at the bottom of this file for explanations of each shader file.
//

// This is our global shader object.  We can create many shaders this way.
CShader g_Shader;

// This will be used to store a variable ID of "time" in our vertex shader file.
// We will change this to make the wavy effect.
GLint g_TimeLoc = 0;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the program window.
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Let's init our GLSL functions and make sure this computer can run the program.
	InitGLSL();

	// Here we pass in our vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("morph.vert", "morph.frag");

	// Now we want to get the variable "time" in the morph.vert file so we can update it.
	g_TimeLoc = g_Shader.GetVariable("time");

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


}


/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function clamps a specified frame rate for consistency
/////
/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop
/////
///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
			TranslateMessage(&msg);						// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 		
			if(AnimateNextFrame(60))					// Make sure we only animate 60 FPS
			{
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// * ///////

	// In our RenderScene() function we will now start using the shader.  We added support
	// to create quadrics so we don't have to load any models, which makes the tutorials easy.
	// What we do is create a sphere and then have our shader make it wave.  When we loaded
	// the shader files it turned ON the shader, so we don't need to turn it on every frame.
	// Notice that we pass in our g_TimeLoc variable that we got in the Init() function above
	// to change the "time" variable in the vertex shader.  We also pass in a new waveChange
	// value to vary the wave.

	// Create a static float to hold our current wave value, then update and change it each frame.
	static float waveChange = 0.0f;
	g_Shader.SetFloat(g_TimeLoc, waveChange);
	waveChange += 0.06f;

	GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack

	gluQuadricDrawStyle(pObj, GLU_LINE);				// Draw the sphere in wireframe

	glTranslatef(0.0f, 0.0f, -6.5f);					// Move the sphere backwards from the camera
	gluSphere(pObj, 2.5f, 75, 75);						// Draw a decent poly sphere with a radius of 2.5
	
	gluDeleteQuadric(pObj);								// Free the Quadric

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// * ///////


	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_KEYDOWN:
		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;
 
    case WM_DESTROY:									// If the window is destroyed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = (long)DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// That was simple to use our shader object huh?  Notice that we didn't need
// to turn "on" the shader every frame.  This is because once we call InitShaders()
// it is already turned on.  In the following tutorials we will show an example of
// turning them on and off in RenderScene().  You would just need to call TurnOn
// and TurnOff().
//
// So, when we run this program you will see a wavy wire-frame sphere.  The color
// red is from the fragment shader.  We just did a simple usage of the fragment
// shader for now.  The following tutorials will use more complex fragment shaders.
//
// Check out the bottom of CShader.cpp for complete details on the loading and 
// running of shader code.
//
// * Morph.vert shader file explanation *

/*
uniform float time;

void main()
{
	vec4 vertex = vec4(gl_Vertex);

	vertex.z = cos(4.0 * vertex.y + time) * 0.45;

	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
*/

// Above is the vertex shader, morph.vert.  Let's go over each line to explain
// what is going on.  Notice that the shader is similar to a normal program in C++.
//
// *** This line creates a variable called "time", of type "float".
// uniform float time;
//
// We can manipulate the variable to help make the wavy effect.
// uniform means that it is a made up variable that we can change/access.
//
// *** Just like in C++ we create a main with an opening scope {}
// void main(void)
// {
//
// *** Here store the current vertex in the GPU in a vec4 variable. 
// vec4 vertex = vec4(gl_Vertex);
//
// "gl_Vertex" is the preset variable for the current vertex that the shader
// is dealing with.  Notice that we need semi-colons at the end of statements
// like in C/C++.
//
// *** Now we actually create the wave effect by doing some simple math and cosine.
// vertex.z = cos(4.0 * vertex.y + time) * 0.45;
//
// GLSL has many built in functions that is understands like cos(), sin(), max(), etc.
// The variable "time" is passed into cos(), which will make the wave move throughout
// the sphere.  Comment out the increase of "time" in RenderScene() and notice it 
// doesn't move.  If you don't want it a flat wave just use: 
// vertex.z = vertex.z + cos(4.0 * vertex.y + time) * 0.45;
//
// *** Last of all, we set the new position of current vertex.
// gl_Position = gl_ModelViewProjectionMatrix * vertex;
//
// We don't just set the gl_Position variable to vertex because we need
// to multiply the vertex by OpenGL's current modelview and projection matrix.
// This positions the vertex in the world where it should be, and according to the camera.
// gl_Position and gl_ModelViewProjectionMatrix are both built in variables for GLSL.
// You also have gl_ModelViewMatrix and gl_ProjectionMatrix.  You could also say:
//
// gl_Position = gl_ModelViewMatrix * gl_ProjectionMatrix * vertex;
//
// Finally, we close the scope with a } at the end of the shader.
// Remember, that you can actually put comments in the shader files like in C++,
// but I chose not to so it didn't confuse anyone.  I think it's easier to look
// at shaders for the first time without comments so you know how simple they are.
// 
// Let's look really fast at the fragment shader, morph.frag:
//
// * Morph.frag shader file explanation *

/*

void main()
{	
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

*/

// This file is easy to explain.  gl_FragColor is a GLSL variable for
// storing the color of the current vertex.  In this case, we set it to red,
// with (R, G, B, A) being what we pass into vec4().
//
// That's pretty much it.  Just remember, .vert files are for transformations
// of the vertices, and fragment files are for texture and color, among other things.
// While the vertex shader handles each vertex being drawn, the fragment shader is
// a "pixel shader", which means it handles each pixel that is drawn on the screen from
// 3D objects.
//
// That's all there is to doing a basic shader!  Pretty powerful huh?  You will be
// further impressed at shader's power as we go through the next tutorials.
//
//
// © 2000-2005 GameTutorials