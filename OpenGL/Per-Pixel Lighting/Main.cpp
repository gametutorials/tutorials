//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersLightingII								 //
//																		 //
//		$Description:	Using GLSL we render specular per-pixel lighting //
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
// This is the next tutorial in our GLSL shader tutorial series.  Previously
// we learned how to create lighting with basic ambient and diffuse lighting.
// In this tutorial we will go the extra step and do per-pixel lighting with
// the added specular lighting.  The code is 99% the same as the previous
// tutorial, except for the shaders.  Last time we did vertex lighting in
// our vertex shader, but now we will go over using the fragment shader that
// will produce a per-pixel effect.
//
// * What is per-pixel lighting? *
//
// This might come as a surprise to you, but per-pixel lighting is lighting
// every geometry-rendered pixel on the screen :)  Of course, we don't light 
// the background, but all the pixels associated with rendered primitives like
// objects, triangles, quads, etc.  If you want to see the difference of vertex
// and per-pixel lighting I suggest you look at our per-pixel lighting tutorial
// in the Direct3D section, which shows a side-by-side example of the difference
// between vertex and per-pixel lighting.  Another way to see the difference is
// to go into the last tutorial and set the quadric sphere detail to (5,5) instead 
// of (75,75).  You will notice the banding that originates from each vertex.
// Then do the same thing for this tutorial and notice that the specular lighting
// is not effected from the detail of the sphere, but has obvious per-pixel
// lighting.
//
// * What is specular lighting? *
//
// Just as a refresher, specular lighting is the brightest light that shows a
// reflection of the light source, usually white.  Ambient lighting is the color
// of the absence of direct light on an object.  Diffuse light is the normal color
// of the light source.  Specular lighting is a great effect because it makes the
// lighting look more realistic.
// 
// When you run the program you will see 2 spheres.  One sphere represents the
// moving light, and the larger sphere is what is being lit.  If you look
// at the bottom of this file you will find a line-by-line description of
// the lighting shader.  The shaders in this lesson are a bit more complicated
// than previous tutorials, but still moderately simple.
// 
// Keep in mind that we do not take into account the material properties
// of each vertex.  You can add that if you want, you just multiply the
// diffuse color by gl_FrontMaterial.diffuse in the shader.  We don't set
// any of those OpenGL properties in this tutorial so I didn't use it.
// 
// If you have already gone through the previous lighting tutorial, then
// you don't need to look through much of the code, since we just add 2
// lines of code to input our specular light color to OpenGL.  You can
// pretty much just jump right down to the bottom of this file to look
// through the explanations on the vertex and fragment shader.
//
//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our global shader object.  We can create many shaders this way.
CShader g_Shader;

// Below is our starting light position and moving X direction
float g_LightPosition[4] = {0.0f, 0.0f, 3.0f, 0.0f};
float g_Direction = 0.02f;


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

	float ambience[4] = {0.1f, 0.1f, 0.1f, 1.0f};		// The color of the ambient light
	float diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0};			// The color of the diffuse light


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	float specular[4] = {1.0f, 1.0f, 1.0f, 1.0};		// Set the specular to pure white
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		// Input our specular to OpenGL

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambience );		// Set our ambience light values
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );		// Set our diffuse light color
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );	// This sets our light position

	// This turns the background to a dark grey/black.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we pass in our new vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("lightingII.vert", "lightingII.frag");

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
	
	// Turn off the shader so that it doesn't effect our sphere representing our light
	g_Shader.TurnOff();

	// Now we create a small yellow sphere to represent the moving light.
	// We position the sphere at the g_LightPosition.
	glColor3f(1.0f, 1.0f, 0.5f);
	glTranslatef(g_LightPosition[0], g_LightPosition[1], g_LightPosition[2]);
	gluSphere(pObj, 0.05f, 25, 25);

	// Now that we already rendered the light we can turn shaders back on.
	g_Shader.TurnOn();
	
	gluDeleteQuadric(pObj);								// Free the Quadric

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
// Can you tell the difference between specular and no specular lighting?
// I'll give a quick review of what this tutorial did.  Like the last lighting
// tutorial, we have 2 spheres, one is the small moving light, and the larger
// sphere is the sphere that receives and demonstrates the lighting.  We moved
// away from the vertex lighting and added per-pixel lighting in the fragment
// shader.
//
// Now let's go through the vertex shader line-by-line and explain the lighting
// equations we used.
//
// * LightingII.vert shader file explanation *

/*

varying vec3 normal, lightDir, halfVector;
	
void main()
{	
	vec4 eyePos;

	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	worldPos = gl_ModelViewMatrix * gl_Vertex;
	lightDir = normalize(vec3(gl_LightSource[0].position - worldPos));
	
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	gl_Position = ftransform();
} 

*/

// Above is the vertex shader, lightingII.vert.  Before we go over the
// code let's explain really fast how to calculate specular light.  As
// you remember, to calculate the basics of light we take the dot product
// of the light direction/position vector with the normal of the surface.
// We can then know the intensity of the light.  If the value is less than
// 0 then there is no light; however, if it's above 0 then the intensity
// is from 0 to 1.  If the value is one then we have 100% light intensity
// for that area, with 0 or less being no light.
//
// Now the next step is to add more realism by adding specular.  Specular
// is dependent on the eye position of the camera.  Notice how specular
// lighting moves as either the camera, light or model moves.  To calculate
// this we use something called a half vector.  This vector is the vector
// that is halfway between the camera eye vector and the light direction
// vector.  Let's do a quick diagram:
//
//                    L		H	  E 
//   L - Light Dir	   \    |    /
//	 H - Half vector	\   |   /
//	 E - Eye Vector		 \  |  /
//						  \ | /
//						   \|/
//                    -------------
//					    (Surface)
//
// In the diagram above we see 3 vectors, the light vector on the surface,
// the eye vector (the direction the camera is facing) and the H vector
// in the middle represents the half vector.  To calculate the half vector
// you just add E + L.  So H = L + E.  We don't need to do that because
// OpenGL already calculates this for us.  The remaining necessary data
// needed is the normal, the specular light color and the shine factor.  
// We get the normal from the vertex shader, and we have the specular
// light colors we inputted from OpenGL, but what is the shine factor?
// Basically, the higher the shine factor the less the size of our
// specular light.  You will probably use values like 8, 64 and 128.
// With this data we plug it into the specular lighting equation:
//
// Specular = (Normal dot HalfVector)^Shine * LightSpecular;
//
// We dot the normal and the half vector, then raise that to the
// power of our shine value, then multiply that by the light's specular.
// Of course, if you want certain surfaces to have different
// specular levels you will want to also multiply that result
// by the material specular color, but we don't do that in this lesson.
// With the result we can just add it to our diffuse and ambient colors
// to produce our final color for the current pixel.
//
// Let's explain every line now:
//

/*
*** These variables are varying because they will be set and used in
*** the fragment shader.  We need to have these varying variables defined
*** in our vertex and fragment shader.
varying vec3 normal, lightDir, halfVector;

*** This starts our vertex shader program
void main()
{	
	*** This will be used to calculate our world position of the vertex 
	vec4 worldPos;

	*** Just to make sure we want to make sure our normals are updated correctly
	*** and that the normal is normalized.  OpenGL has a normal matrix to fix
	*** any normals that were rotated, translated or scaled.  As long as we use
	*** glLookAt() and don't use glScalef() we probably don't need this line,
	*** but we do it just to make sure for future programs.
	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	*** Multiply the current vertex by the model-view matrix to get the
	*** vertex in world space.  Then we get a normalized vector for our
	*** light direction by subtracting the worldPos from the light's position.
	worldPos = gl_ModelViewMatrix * gl_Vertex;
	lightDir = normalize(vec3(gl_LightSource[0].position - worldPos));
	
	*** Store the normalized half vector that OpenGL computes for us.
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	*** Update the current vertice according to the view and projection matrices.
	gl_Position = ftransform();
} 


*/
// 
// Let's look now at the fragment shader, LightingII.frag:
//
// * LightingII.frag shader file explanation *

/*

varying vec3 normal, lightDir, halfVector;
	
void main()
{
	vec4 diffuseLight, specularLight;	
	vec4 color = gl_LightSource[0].ambient;
	float kShine = 64.0;
	vec3 n;
		
	n = normalize(normal);

	float lightIntensity = max(dot(n, normalize(lightDir)), 0.0);

	diffuseLight = lightIntensity * gl_LightSource[0].diffuse;
	
	specularLight = pow(max(dot(n, normalize(halfVector)), 0.0), kShine) * gl_LightSource[0].specular;

	if(lightIntensity > 0.0) 
	{
		color = diffuseLight + specularLight + gl_LightSource[0].ambient;
	}

	gl_FragColor = color;
}

*/

// This is the first tutorial that we really use the fragment shader.
// If we want per-pixel lighting, this is where the magic needs to
// happen.  Let's go over each line and give a basic explanation:
//

/*
*** Notice that we have the same varying variables in the vertex shader.
*** By the time we get here we will have the variables set correctly.
varying vec3 normal, lightDir, halfVector;
	
*** Start the fragment shader
void main()
{
	*** These variables store values to make things cleaner.
	*** Notice that our shine is set at 64.  If we made the shine
	*** 8 the effect would generate a HUGE specular light on the model.
	vec4 diffuseLight, specularLight;	
	vec4 color = gl_LightSource[0].ambient;
	float kShine = 64.0;
	vec3 n;
		
	*** Just to make sure, let's normalize our normal.
	n = normalize(normal);

	*** Since we will use this value twice, let's store it in a variable.
	*** This is the light intensity value between 0 and 1.
	float lightIntensity = max(dot(n, normalize(lightDir)), 0.0);

	*** Like normal, we calculate our diffuse by multiplying our light's diffuse
	*** by the intensity of the light that is found by dotting the normal and lightDir.
	diffuseLight = lightIntensity * gl_LightSource[0].diffuse;
	
	*** Using our equation for specular explained above, multiply our light's specular
	*** by half vector and the normals intensity.  This is exactly like what we do
	*** with the above diffuse lighting, except instead of the light direction we
	*** use the half vector and raise the result to the kShine value to produce more
	*** or less attenuation of the specular lighting.
	specularLight = pow(max(dot(n, normalize(halfVector)), 0.0), kShine) * gl_LightSource[0].specular;

	*** Here we check to see if there is any light that needs to be applied.
	*** If we have over 0% intensity of light we add the ambient + diffuse + specular lighting.
	if(lightIntensity > 0.0) 
	{
		*** Calculate the final light color value for this current pixel.
		color = diffuseLight + specularLight + gl_LightSource[0].ambient;
	}

	*** Set the current pixel's color value to the light we calculated.  If there
	*** was no light then we just use the default ambient light value.
	gl_FragColor = color;
}

*/

// That's it!  Not so bad was it?  It's pretty intuitive if you go through each
// like and see what is going on; nothing really complicated about it.  Even
// the mediocre mathematician can wrap their head around it :)
//
// Be sure to check out the next tutorial on toon-shading lighting.  Enjoy!
//
//
// © 2000-2005 GameTutorials