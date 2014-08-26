//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersLighting									 //
//																		 //
//		$Description:	Using GLSL we render basic vertex lighting		 //
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
// This is the second tutorial in our GLSL shader tutorial series.  In this
// lesson we will show you how to do VERY basic lighting with a shader.  This
// lighting is just using ambient and diffuse lighting.  We don't do the lighting
// in the fragment shader, but in the vertex shader.  The next tutorial will
// focus on specular lighting, which will be done in the fragment shader.
// 
// When you run the program you will see 2 spheres.  One sphere represents the
// moving light, and the larger sphere is what is being lit.  The code in this
// tutorial is pretty much the same as the previous tutorial.  If you look
// at the bottom of this file you will find a line-by-line description of
// the lighting shader.  It's really simple, so you can put down the knife :)
// 
// Keep in mind that we do not take into account the material properties
// of each vertex.  You can add that if you want, you just multiply the
// diffuse color by gl_FrontMaterial.diffuse in the shader.  We don't set
// any of those OpenGL properties in this tutorial so I didn't use it.
// 
//

// Below is our starting light position and moving X direction
float g_LightPosition[4] = {0.0f, 0.0f, 3.0f, 0.0f};
float g_Direction = 0.02f;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our global shader object.  We can create many shaders this way.
CShader g_Shader;


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

	// Let's init our GLSL functions and make sure this computer can run the program.
	InitGLSL();


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// To initialize the lights we just do what we normally do in OpenGL.
	// We just use ambient and diffuse for this tutorial because there is
	// no specular calculations in our shader.  The next tutorial handles
	// specular lighting, giving a spotlight effect.

	float ambience[4] = {0.1f, 0.1f, 0.1f, 1.0f};		// The color of the ambient light
	float diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0};			// The color of the diffuse light
	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambience );		// Set our ambience light values
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );		// Set our diffuse light color
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );	// This Sets our light position

	// This turns the background to a dark grey/black.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Here we pass in our new vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("lighting.vert", "lighting.frag");

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

	// In our render function we create a small sphere to show where the light is, as
	// well as a big sphere to show vertex lighting.  The small light moves back and
	// forth so we need to have a variable to keep track of the light position and 
	// direction; g_LightPosition and g_Direction.

	// If the light position goes out of the screen to the left, reset the direction and position.
	if(g_LightPosition[0] < -4.0f)
	{
		g_Direction = g_Direction * -1;
		g_LightPosition[0] = -4.0f; 
	}
	// If the light position goes out of the screen to the right, reset the direction and position.
	if(g_LightPosition[0] > 4.0f)
	{
		g_Direction = g_Direction * -1;
		g_LightPosition[0] = 4.0f; 
	}

	// Each frame we move the light by the current direction, then update the light with GL.
	g_LightPosition[0] += g_Direction;
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );


	GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack
	gluQuadricDrawStyle(pObj, GLU_FILL);				// Draw the sphere normally

	glTranslatef(0, 0, -10);							// Move the sphere back by 10
	gluSphere(pObj, 2.5f, 75, 75);						// Draw the sphere with a radius of 2.5
	
	// Here is where we turn OFF our shaders so that the small light sphere isn't
	// effected by the lighting.  That way it stays bright and you can see it move.
	g_Shader.TurnOff();

	// Now we create a small yellow sphere to represent the moving light.
	// We position the sphere at the g_LightPosition.
	glColor3f(1.0f, 1.0f, 0.5f);
	glTranslatef(g_LightPosition[0], g_LightPosition[1], g_LightPosition[2]);
	gluSphere(pObj, 0.05f, 25, 25);

	// Now that we already rendered the light we can turn shaders back on.
	g_Shader.TurnOn();
	
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
// Pretty cool effect huh?  This is really fast lighting when done with shaders.
// You might remember the days when if you had anything less than a Geforce2
// video card lighting was all faked in texture maps and was slow if you turned
// it on.
//
// Now let's go through the vertex shader line-by-line and explain the basic
// lighting equation we used.
//
// * Lighting.vert shader file explanation *

/*
void main()
{
	vec3 normal, lightDir;
	vec4 diffuseColor;
	float intensity;
	
	normal = normalize(gl_Normal);

	lightDir = normalize(vec3(gl_LightSource[0].position));
	intensity = max(dot(normal, lightDir), 0.0);

	diffuseColor = intensity * gl_LightSource[0].diffuse;
	gl_FrontColor = diffuseColor + gl_LightSource[0].ambient;

	gl_Position = ftransform();
}
*/

// Above is the vertex shader, lighting.vert.  Before we go over the
// code let's explain really fast how to calculate light.  It's quite
// simple actually.  We take the normal of the current vertex, which
// is passed in with the sphere by default (since we are using quadrics).
// Just to make sure, we normalize that normal, then use the dot product
// to project it onto the light's direction.  We take the max between
// 0 and the result because we don't light negative results.  This will
// make sure we get a value between 0 and 1.  Do you remember your
// math?  What does that mean if a dot product results in 0?  Let's review.
// That means that the vectors are perpendicular.  If we have a normal at 
// the top of the sphere pointing up, (0, 1, 0), and the light direction is
// pointing straight into the sphere at (0, 0, 1), the light wouldn't
// light up that point because there is no light going to it.  If we
// have a normal at (0, 0, 1) then the dot product would be 1.  This means
// that that the light is shinning directly on the area. 
//
// Basically, the result we get from our dot product is the intensity
// of light that we need too apply to that area.  If the dot product
// is negative that means that the light is shinning behind the area
// and would not be lit, which is why we do a max of 0 and the result.
// Simple huh?  Once we get the intensity of the light that should be
// applied we just multiply that intensity by our diffuse color, then
// we just add the ambient color to that result, giving us our final color.
//
// Let's explain every line now:
//
// Notice that we don't have any "uniform" variables that we need to
// set within our code.  We do, however, create 4 variables for our
// lighting computations.
/*
	*** These variables store the normalized normal, the normalized light 
	*** direction, the diffuse, as well as the intensity of the light.
	vec3 normal, lightDir;
	vec4 diffuseColor;
	float intensity;
	
	*** We normalize the normal of the vertex to make sure
	normal = normalize(gl_Normal);

	*** We also normalize the light direction to make sure
	lightDir = normalize(vec3(gl_LightSource[0].position));

	*** Now we get the dot product between the normal and the light direction.
	*** Remember we need to take the highest value, 0 or the result. We
	*** don't want negative numbers since that means there is no light at all.
	intensity = max(dot(normal, lightDir), 0.0);

	*** To get the correct diffuse color we multiply our diffuse by the light intensity.
	diffuseColor = intensity * gl_LightSource[0].diffuse;

	*** Finally, just add the ambient to the diffuse color.
	gl_FrontColor = diffuseColor + gl_LightSource[0].ambient;

	*** Since we don't change the position, we just use the standard ftransform().
	*** This does the same thing as "gl_ModelViewProjectionMatrix * gl_Vertex".
	gl_Position = ftransform();
*/
// 
// Let's look really fast at the fragment shader, Lighting.frag:
//
// * Lighting.frag shader file explanation *

/*

void main()
{	
	gl_FragColor = gl_Color;
}

*/

// We don't really use the fragment shader, since the work is
// done in the vertex shader, so here we just set the original
// color (gl_Color) to the fragment color variable, gl_FragColor.
//
// That's it!  Cool lighting effect huh?  The next tutorial shows
// us how to add specular lighting.  Check it out!
//
//
// © 2000-2005 GameTutorials