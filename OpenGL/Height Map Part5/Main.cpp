//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		HeightMap5										 //
//																		 //
//		$Description:	Shows how to add reflections and basic water.	 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"								// This includes our main header file
#include "camera.h"								// This includes our camera's header file

bool  g_bFullScreen = true;						// Set full screen as default
HWND  g_hWnd;									// This is the handle for the window
RECT  g_rRect;									// This holds the window dimensions
HDC   g_hDC;									// General HDC - (handle to device context)
HGLRC g_hRC;									// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;							// This holds the global hInstance for UnregisterClass() in DeInit()


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This is the first attempt at basic water for our OpenGL tutorials.  In this
// tutorial we will add upon the previous heightmap tutorial and create some
// reflective water.  We will use OpenGL's stencil buffer functions to draw the
// reflection of the terrain into the water.  We will also use clipping planes
// to make sure the reflection stays in our designated flat plane (the water).
// Though this tutorial uses basic water code, the following tutorials use vertex
// and pixel shaders to create realistic refracted/reflected water.  This lesson
// is just to get your juices flowing for later :)
//
// * How does reflection work? *
//
// Well, reflection is usually done with a simple technique of rendering the
// reflected geometry upside down, then using a clipping plane to only show the
// reflection above the surface; like our water surface.  This means that you
// will see the terrain reflected into the water, but when you go below the
// water it will not show the reflection anymore, just the bottom of the terrain.
// We tell the clipping plane to cut the geometry below our water.  This makes a
// realistic effect of the reflection.  If we didn't use clipping planes we would
// go underwater and see our upside down terrain, which would look silly.
//
// Another way to do reflections of this kind is to render the top part of the
// terrain to a texture, then flip that texture upside down and bind it to our
// surface (the water).  We would use a clipping plane in this case too, when
// rendering it to a texture.  We use this technique in our next heightmap 
// tutorials on doing refraction/reflection with shaders.
//
// Of course, there are other types of reflection like enviroment mapping and
// such, which we will cover later in our enviroment-mapped-car tutorial.
//
// This tutorial assumes that you have already gone through all of the heightmap
// tutorials and are familiar with the code.  We only add a little bit of code
// to this version for the water, so you only need too look in this file (main.cpp)
// to see the changes and new code.
//
// You can use the same controls as the previous heightmap tutorials, +, -, space,
// and left/right click.  Notice that we included fog from the last tutorial in
// this one, but we color it blue and make it look like water's fog.
//

// This stores our water height on the terrain
float g_WaterHeight = 30.0f;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our fog extension function pointer to set a vertice's depth
PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;

// This stores the desired depth that we want to fog
float g_FogDepth = 50.0f;

// Our function pointers for the ARB multitexturing functions
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;

// This controls if we have detail texturing on or off
bool g_bDetail = true;

// This handles the current scale for the texture matrix for the detail texture
int g_DetailScale = 16;

// This is our global camera object
CCamera g_Camera;								

// This holds the height map data
BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];			

// This tells us if we want lines or fill mode
bool  g_bRenderMode = true;						

// This holds the texture info by an ID
UINT g_Texture[MAX_TEXTURES] = {0};

// This holds our global delta time
float g_DT = 0.0f;

#define BACK_ID		11		// The texture ID for the back side of the cube
#define FRONT_ID	12		// The texture ID for the front side of the cube
#define BOTTOM_ID	13		// The texture ID for the bottom side of the cube
#define TOP_ID		14		// The texture ID for the top side of the cube
#define LEFT_ID		15		// The texture ID for the left side of the cube
#define RIGHT_ID	16		// The texture ID for the right side of the cube
#define WATER_ID	17		// The texture ID for the water bitmap (* NEW *)


///////////////////////////////// CREATE SKY BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a sky box centered around X Y Z with a width, height and length
/////
///////////////////////////////// CREATE SKY BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	// Turn on texture mapping if it's not already
	glEnable(GL_TEXTURE_2D);

	// Bind the BACK texture of the sky map to the BACK side of the cube
	glBindTexture(GL_TEXTURE_2D, g_Texture[BACK_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// This centers the sky box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the BACK Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// Bind the FRONT texture of the sky map to the FRONT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[FRONT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);	
	
		// Assign the texture coordinates and vertices for the FRONT Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[BOTTOM_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
	
		// Assign the texture coordinates and vertices for the BOTTOM Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// Bind the TOP texture of the sky map to the TOP side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[TOP_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the TOP Side
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// Bind the LEFT texture of the sky map to the LEFT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[LEFT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the LEFT Side
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[RIGHT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

		// Assign the texture coordinates and vertices for the RIGHT Side
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the application
/////
///////////////////////////////// INIT  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Here we initialize our multitexturing functions
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	// Check to see if our multi-texture extensions were loaded
	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your current setup does not support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// Find the correct function pointer that houses the fog coordinate function
	glFogCoordfEXT	= (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");

	// Before trying to use this function pointer, we need to make sure it was
	// given a valid address.  If not, then we need to quit because something is wrong.
	if(!glFogCoordfEXT)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your current setup does not support volumetric fog", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// Pick a tan color for our fog with a full alpha
	float fogColor[4] = {0.2f, 0.2f, 0.9f, 1.0f};

	glEnable(GL_FOG);						// Turn on fog
	glFogi(GL_FOG_MODE, GL_LINEAR);			// Set the fog mode to LINEAR (Important)
	glFogfv(GL_FOG_COLOR, fogColor);		// Give OpenGL our fog color
	glFogf(GL_FOG_START, 0.0);				// Set the start position for the depth at 0
	glFogf(GL_FOG_END, 50.0);				// Set the end position for the detph at 50

	// Now we tell OpenGL that we are using our fog extension for every vertex
	glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

	// Here we read in the height map from the .raw file and put it in our
	// g_HeightMap array.  We also pass in the size of the .raw file (1024).
	LoadRawFile("Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);	

	glEnable(GL_DEPTH_TEST);							// Enables depth testing
	glEnable(GL_TEXTURE_2D);							// Enable texture mapping
	glEnable(GL_CULL_FACE);								// Turn on back face culling

	CreateTexture(g_Texture[0],			"Terrain.bmp"); // Load the terrain texture
	CreateTexture(g_Texture[1],			"Detail.bmp");  // Load the detail-terrain texture
	CreateTexture(g_Texture[BACK_ID],	"Back.bmp");	// Load the Sky box Back texture
	CreateTexture(g_Texture[FRONT_ID],	"Front.bmp");	// Load the Sky box Front texture
	CreateTexture(g_Texture[BOTTOM_ID], "Bottom.bmp");	// Load the Sky box Bottom texture
	CreateTexture(g_Texture[TOP_ID],	"Top.bmp");		// Load the Sky box Top texture
	CreateTexture(g_Texture[LEFT_ID],	"Left.bmp");	// Load the Sky box Left texture
	CreateTexture(g_Texture[RIGHT_ID],	"Right.bmp");	// Load the Sky box Right texture


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	CreateTexture(g_Texture[WATER_ID],	"Water.bmp");	// Load the water texture
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Give our camera a decent starting point in the world
	g_Camera.PositionCamera( 280, 40, 225,  281, 40, 225,  0, 1, 0);
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function keeps our rendering within a specified frame rate
/////
///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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
		g_DT = desiredFPS;
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// DRAW WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws our moving water QUAD
/////
///////////////////////////////// DRAW WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawWater(float waterHeight)
{
	// This is our function that draws the water over the terrain.
	// We create a simple variable to slowly move the water texture.
	// If you want to change the UV coordinates for the water, just
	// chance the waterUV variable.
	static float move = 0.0f;
	float waterUV = 1.0f;

	// Slowly increase our water texture across the surface
	move += 0.00005f;

	// Turn on texture mapping and bind the water texture
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_Texture[WATER_ID]);

    //Render the water surface at the waterHeight 
    glBegin(GL_QUADS);
		glTexCoord2f(0.0f, waterUV + move);
		glVertex3f(0.0f, waterHeight, 0.0f);
		glTexCoord2f(0.0f, 0.0f + move);
		glVertex3f(0.0f, waterHeight, 1000.0f);
		glTexCoord2f(waterUV, 0.0f + move);
		glVertex3f(1000.0f, waterHeight, 1000.0f);
		glTexCoord2f(waterUV, waterUV + move);
		glVertex3f(1000.0f, waterHeight, 0.0f);
    glEnd();

	// Turn off texturing
    glDisable(GL_TEXTURE_2D);

	// Restore the color of our world to white with full alpha
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
		
			// We added a way to clamp our frame rate so the water moves smoothly
			if(AnimateNextFrame(60))					// Make sure we only render 60 frames per second
			{
				g_Camera.Update();						// Update the camera data
				RenderScene();							// Render the scene every frame
			}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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

	// Get the current position of the camera
	CVector3 vPos		= g_Camera.Position();
	CVector3 vNewPos    = vPos;

	// Check if the camera is below the height of the terrain at x and z,
	// but we add 10 to make it so the camera isn't on the floor.
	if(vPos.y < Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10)
	{
		// Set the new position of the camera so it's above the terrain + 10
		vNewPos.y = (float)Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10;

		// Get the difference of the y that the camera was pushed back up
		float temp = vNewPos.y - vPos.y;

		//  Get the current view and increase it by the different the position was moved
		CVector3 vView = g_Camera.View();
		vView.y += temp;

		// Set the new camera position.
		g_Camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
								vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	// Give OpenGL our camera position
	g_Camera.Look();


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here comes the most technical part of our tutorial--clipping planes.
	// We turn the terrain upside-down, so we don't want the bottom sticking
	// out of the water, so we need to create a clipping plane that will
	// clip anything that is above it.  In this case, (0, -1, 0) is the normal
	// of the plane to clip, and we finally give it out distance from the
	// origin--g_WaterHeight.
	double reflectPlane[] = {0.0f, -1.0f, 0.0f, g_WaterHeight};			

	glEnable(GL_CLIP_PLANE0);							// Turn on clipping for plane 0

	// This is where we select the clipping plane we want to use, and then the
	// plane's data is passed in as an array of floats (ABC and D).  You can have
	// many clipping planes going, which is why we need to specify which one we want.
	glClipPlane(GL_CLIP_PLANE0, reflectPlane);			

	// Now comes the reflection!
	// Pop on a new matrix so our translation/scaling doesn't effect other data
	glPushMatrix();

		// We first need to translate the terrain to the water height multiplied by 2, 
		// since we are reflecting it upside down at that height.
		glTranslatef(0.0f, g_WaterHeight*2.0f, 0.0f);

		// We then use glScale() to flip the terrain upside-down (-1 flips it).
		glScalef(1.0, -1.0, 1.0);	

		// Since the terrain is upside-down we need to do front-face culling.
		glCullFace(GL_FRONT);

		// Now we render the terrain and the skybx upside-down for the reflection.
		RenderHeightMap(g_HeightMap);						
		CreateSkyBox(500, 0, 500, 2000, 2000, 2000);

		// Now we can restore the app to back-face culling
		glCullFace(GL_BACK);
	
	// Leave the previous matrix and go back to the original matrix
	glPopMatrix();

	// Since the reflection is already drawn, let's turn off clipping.
	glDisable(GL_CLIP_PLANE0);							
	
	// Now we actually draw the water blended over the reflection, which
	// creates the effect that the reflection is in the water.  We blend
	// with a simple alpha to one minus alpha blending function.  The color
	// is set to a blue, with the alpha value at 50% (0.5).  We don't want
	// the alpha too high or else we wouldn't see the reflection.
	glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f, 0.5f, 0.9f, 0.5f);
		DrawWater(g_WaterHeight);
	glDisable(GL_BLEND);

	// Render the height map normally, right-side-up.
	RenderHeightMap(g_HeightMap);						

	// Create the sky box and center it around the terrain
	CreateSkyBox(500, 0, 500, 2000, 2000, 2000);

	// This block of code right here is for an added effect of
	// what is draw when you are under the water.  This isn't necessary,
	// but I thought I would add it to improve the tutorial's realism.
	// When we go underwater we want to be able to look up out of the
	// water.  To do that we just render the water again, but change
	// the culling to front-face, then change our water color.  Nothing special.
	// We could just do a check to see if we are under or over the water,
	// then only render the water once, but I think you can do that yourself.
	glCullFace(GL_FRONT);
	glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.0f, 0.0f, 0.9f, 0.5f);
		DrawWater(g_WaterHeight);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);									
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;


	case WM_LBUTTONDOWN:
	
		g_bDetail = !g_bDetail;
		break;

	case WM_RBUTTONDOWN:								// If the left mouse button was clicked
		
		g_bRenderMode = !g_bRenderMode;

		// Change the rendering mode to and from lines or triangles
		if(g_bRenderMode) 				
		{
			// Render the triangles in fill mode		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
		else 
		{
			// Render the triangles in wire frame mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		}
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_SPACE:								// Check if we hit the SPACE bar

				// To get a few different ideas of what a detail texture
				// could look like, we want to change it's scale value.

				// Times the current scale value by 2 and loop when it gets to 128
				g_DetailScale = (g_DetailScale * 2) % 128;

				// If the scale value is 0, set it to 1 again
				if(g_DetailScale == 0)
					g_DetailScale = 1;

				break;

			case VK_ADD:								// Check if we hit the + key

				g_FogDepth += 1;						// Increase the fog height

				if(g_FogDepth > 200)					// Make sure we don't go past 200
					g_FogDepth = 200;

				break;

			case VK_SUBTRACT:							// Check if we hit the - key

				g_FogDepth -= 1;						// Decrease the fog height

				if(g_FogDepth < 0)						// Make sure we don't go below 0
					g_FogDepth = 0;

				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Tell windows we want to quit
        break; 
    } 
 
    return DefWindowProc (hWnd, uMsg, wParam, lParam); 	// Return the default
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This wasn't so bad was it?  Especially for such a cool effect.  You need to
// check out the water tutorial with shaders.  The effect is so much more realistic.
// Just to sum up what happen here, we just created a reflection in the water of
// our terrain and skybox.  This is done by rendering the terrain and skybox two
// times, but with one of those times upside-down.  We then throw a simple-blended
// QUAD that resembles the water over the reflection.  We use a clipping plane to
// make sure that the reflected terrain stays under the way and not sticking out.
//
// To speed this up we would want to put our terrain in a display list, which would
// cut down our loops and rendering time.  You are now ready to go to the next
// tutorial that goes over using shaders to create reflection and water.
//
//
// © 2000-2005 GameTutorials