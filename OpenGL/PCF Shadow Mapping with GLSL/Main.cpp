//*********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersShadowMappingPCF							 //
//																		 //
//		$Description:	Creating higher quality shadowmaps with GLSL	 //
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
float g_DT;												// This stores the elapsed time between the current and last frame


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// Welcome to the next tutorial on shadow mapping.  In this tutorial, we take the
// previous code from the GLSL shadow-mapping tutorial and add PCF to our shadows.
// It is assumed that you have looked at both the first shadow mapping tutorial
// and the GLSL version, since it's a series.
//
// * What is PCF? *
//
// PCF stands for "Percentage Closer Filtering".  Basically, it is taking some
// surrounding depth values in a shadow mapping and averaging them together to
// give a less-aliased version of the shadow map.  It performs a type of blur.
// Since you can't just do bilinear filtering on a shadowmap because it is
// depth values, you have to use an approach like PCF.
//
// In this tutorial we will do PCF with a 3x3 kernel, meaning that we take all the
// eight surrounding depth values of each depth value and average them together,
// including the middle one.  Think of it as a three-by-three grid.  There are
// eight pixels surrounding the middle one.  We will add each one of the values
// up, then divide by 9.  This will give us an average for that current pixel that
// we are applying a shadow too.  This gives a little blur effect around the edge
// of the shadows.  Though a 3x3 grid isn't going to make a huge difference, it
// will be enough to make decent looking shadows, especially when the shadows
// are rendered over an object with a detailed texture map.
//
// Open up the two images that came with this tutorial.  Notice the self-shadowing
// that shadow maps create.  ShadowsOnTextures.jpg shows the shadow mapping being
// applied to the character, where NoShadows.jpg just has it being applied to the
// walls.  Notice that the character's shield blocks the light, giving a realistic
// shadow on his body.  Since the texture map of the character is detailed and not
// a pure color like the background walls, it is very hard to tell if the shadow
// is aliased or not.  However, when looking at the pure pink wall behind the
// character the aliasing around the edges of the shadowmap are more obvious.
// I show you this to have you see that using this technique in a detailed world
// isn't going to look as aliased as in our simple demo of pure-colored walls.
//
// * Nothing changed in code *
//
// In this tutorial we didn't change any of the code in our project, except the
// name of the shader and fragment files.  The tutorial is just showing the PCF
// technique, so ShadowMappingPCF.frag is where all the new code lies.  At the
// bottom of this file we will go over ever line and explain the technique.
//
// The controls for this tutorial are just the typical camera controls with the mouse
// and arrow keys.  Go to the bottom of this file to see the PCF shader explanation.
//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// These store our width and height for the shadow texture
#define SHADOW_WIDTH 512
#define SHADOW_HEIGHT 512

// The max textures we will use in our array
#define MAX_TEXTURES 1000								

// This is the index into the g_Texture array that will hold our depth texture
#define SHADOW_ID	0

// This is used to set the mode with glTexParameteri() for comparing depth values.
// We use GL_COMPARE_R_TO_TEXTURE_ARB as our mode.  R is used to represent the depth value.
#define GL_TEXTURE_COMPARE_MODE_ARB       0x884C

// This is used to set the function with glTexParameteri() to tell OpenGL how we
// will compare the depth values (we use GL_LEQUAL (less than or equal)).
#define GL_TEXTURE_COMPARE_FUNC_ARB       0x884D

// This mode is what will compare our depth values for shadow mapping
#define GL_COMPARE_R_TO_TEXTURE_ARB       0x884E

// The texture array where we store our image data
UINT g_Texture[MAX_TEXTURES];	

// Our camera object so we can move around the world.
CCamera g_Camera;

// This is our global shader object that will load the shader files
CShader g_Shader;

// Below is our starting light position
float g_LightPosition[4] = {0.0f, 2.0f, 5.0f, 0.0f};

// We set the light's view position at the origin
float g_LightView[3] = {0.0f, 0.0f, 0.0f};

// This is the current direction and speed the light is moving
float g_Direction = -0.04f;

// These are the light's matrices that need to be stored
float g_mProjection[16] = {0};
float g_mModelView[16] = {0};
float g_mCameraInverse[16] = {0};


///////////////////////////////// STORE LIGHT MATRICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function positions our view from the light for shadow mapping
/////
///////////////////////////////// STORE LIGHT MATRICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void StoreLightMatrices()
{	
	// Reset our current light matrices
	memset(g_mModelView, 0, sizeof(float)*16);
	memset(g_mProjection, 0, sizeof(float)*16);

	// Let's push on a new matrix so we don't change the rest of the world
	glPushMatrix();

		// Push on a new matrix to keep our view changes isolated
		glPushMatrix();

			// Reset the current modelview matrix
			glLoadIdentity();		

			// This is where we set the light's position and view.
			gluLookAt(g_LightPosition[0],  g_LightPosition[1],  g_LightPosition[2], 
					  g_LightView[0],	   g_LightView[1],	    g_LightView[2],		0, 1, 0);

			// Now that we have the light's view, let's save the current modelview matrix.
			glGetFloatv(GL_MODELVIEW_MATRIX, g_mModelView);

		// Now pop off the current light view's matrix
		glPopMatrix();

		// Reset the current matrix
		glLoadIdentity();

		// Set our FOV, aspect ratio, then near and far planes for the light's view
		gluPerspective(60.0f, 1.0f, 0.5f, 25.0f);

		// Grab the current matrix that will be used for the light's projection matrix
		glGetFloatv(GL_MODELVIEW_MATRIX, g_mProjection);

	// Go back to the original matrix
	glPopMatrix();
}


/////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a blank texture to render to
/////
/////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	
void CreateRenderTexture(UINT textureArray[], int sizeX, int sizeY, int channels, int type, int textureID)										
{
	// Create a pointer to store the blank image data
	unsigned int *pTexture = NULL;											

	// Store the current channels to be allocated by default
	int channelsTrue = channels;

	// If the channels are greater than 4 there must be a special flag like
	// GL_DEPTH_COMPONENT, so make sure we only allocate 1 bit per pixel.
	if(channels > 4)
		channelsTrue = 1;

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new unsigned int [sizeX * sizeY * channelsTrue];
	memset(pTexture, 0, sizeX * sizeY * channelsTrue * sizeof(unsigned int));	

	// Register the texture with OpenGL and bind it to the texture ID
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					
	
	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, sizeX, sizeY, 0, type, GL_UNSIGNED_INT, pTexture);						
	
	// Set the texture quality
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}


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

	// Here we initialize our multi-texturing functions
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	// Make sure our multi-texturing extensions were loaded correctly
	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your current setup does not support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// Tell OpenGL our light's position
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );

	// This turns the background to a dark grey/black.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Turn on our light and enable color along with the light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	// Here we allocate memory for our depth texture that will store our light's view
	CreateRenderTexture(g_Texture, SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, SHADOW_ID);

	// Set the camera:		Position		View		 Up Vector
	g_Camera.PositionCamera(0, 9, 12,     0, 2.5, -2,     0, 1, 0);

	// Let's init our GLSL functions and make sure this computer can run the program.
	InitGLSL();		


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we pass in our new vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("ShadowMappingPCF.vert", "ShadowMappingPCF.frag");

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
		g_DT = desiredFPS;
		elapsedTime -= desiredFPS; // Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function Handles the main program loop
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
		else											// If there wasn't a message
		{ 
			if(AnimateNextFrame(60))					// Render the scene only at 60 FPS
			{
				g_Camera.Update();						// Handle our camera movement
				RenderScene();							// Render the scene every frame 
			}
        } 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


//////////////////////////////// DRAW WALLS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws the walls behind our objects
/////
//////////////////////////////// DRAW WALLS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawWalls()
{
	// This is the size of our walls for each side of the axis
	float size = 5.0f;

	// Start rendering the walls as QUADS
	glBegin(GL_QUADS);

		// Back
		glNormal3f(0, 0, 1);				// Store the FORWARD normal
		glColor4f(1.0f, 0.0f, 1.0f, 1.0);	// Make this wall pink
		glVertex3f(-size, size, 0);			// The top-left vertex
		glVertex3f(-size, -size, 0);		// The bottom-left vertex
		glVertex3f(size, -size, 0);			// The bottom-right vertex
		glVertex3f(size, size, 0);			// The top-right vertex

		// Left
		glNormal3f(1, 0, 0);				// Store the RIGHT normal
		glColor4f(1.0f, 0.0f, 0.0f, 1.0);	// Make this wall red
		glVertex3f(-size, size, size);		// The top-left vertex
		glVertex3f(-size, -size, size);		// The bottom-left vertex
		glVertex3f(-size, -size, 0);		// The bottom-right vertex
		glVertex3f(-size, size, 0);			// The top-right vertex

		// Right
		glNormal3f(-1, 0, 0);				// Store the LEFT normal
		glColor4f(0.0f, 0.0f, 1.0f, 1.0);	// Make this wall blue
		glVertex3f(size, size, 0);			// The top-left vertex
		glVertex3f(size, -size, 0);			// The bottom-left vertex
		glVertex3f(size, -size, size);		// The bottom-right vertex
		glVertex3f(size, size, size);		// The top-right vertex

		// Bottom
		glNormal3f(0, 1, 0);				// Store the UP normal
		glColor4f(0.0f, 1.0f, 0.0f, 1.0);	// Make this wall green
		glVertex3f(size, -size, size);		// The front-right vertex
		glVertex3f(size, -size, 0);			// The back-right vertex
		glVertex3f(-size, -size, 0);		// The back-left vertex
		glVertex3f(-size, -size, size);		// The front-left vertex

	// Stop drawing QUADS
	glEnd();
}


//////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire world that needs to be shadowed
/////
//////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderWorld()
{
	// Draw our walls and move them back into the screen by 15
	glPushMatrix();
		glTranslatef(0, 0, -15);
		DrawWalls();
	glPopMatrix();

	// Color the tube green
	glColor4f(0.0f, 1.0f, 0.1f, 1.0);

	// Create a new quadric and render it normally
	GLUquadricObj *pObj = gluNewQuadric();				
	gluQuadricDrawStyle(pObj, GLU_FILL);				

	// Create an empty tube that spins around
	glPushMatrix();
		static float rotY = 0;
		glTranslatef(-2, 0, -10);
		glRotatef(rotY++, 0, 1, 0);
		gluCylinder(pObj, 0.5f, 0.5f, 2.0f, 25, 25);
	glPopMatrix();

	// Color the sphere yellow
	glColor4f(1.0f, 1.0f, 0.1f, 1.0);

	// Create the sphere and move it back and to the right of the tube
	glPushMatrix();
		glTranslatef(2, 0, -10);							
		gluSphere(pObj, 0.5f, 25, 25);	
	glPopMatrix();
	
	// Free the quadric object
	gluDeleteQuadric(pObj);								
}


//////////////////////////////// APPLY SHADOW MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function applies the shadow map to our world data
/////
//////////////////////////////// APPLY SHADOW MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ApplyShadowMap()
{	
	// Let's turn our shaders on for doing shadow mapping on our world
	g_Shader.TurnOn();

	// Turn on our texture unit for shadow mapping and bind our depth texture
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, g_Texture[SHADOW_ID]);

	// Give GLSL our texture unit that holds the shadow map
	GLuint uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "shadowMap"); 
	glUniform1iARB(uniform, 0);

	// Here is where we set the mode and function for shadow mapping with shadow2DProj().
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	// Create our bias matrix to have a 0 to 1 ratio after clip space
	const float mBias[] = {0.5, 0.0, 0.0, 0.0, 
						   0.0, 0.5, 0.0, 0.0,
						   0.0, 0.0, 0.5, 0.0,
						   0.5, 0.5, 0.5, 1.0};

	glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(mBias);			// The bias matrix to convert to a 0 to 1 ratio
		glMultMatrixf(g_mProjection);	// The light's projection matrix
		glMultMatrixf(g_mModelView);	// The light's modelview matrix
		glMultMatrixf(g_mCameraInverse);// The inverse modelview matrix
	glMatrixMode(GL_MODELVIEW);			// Switch back to normal modelview mode

	RenderWorld();						// Render the world that needs to be shadowed

	// Reset the texture matrix
	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	// Turn the first multi-texture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);

	// Light expected, we need to turn our shader off since we are done
	g_Shader.TurnOff();
}


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{	
	// Update the light matrices for it's current position
	StoreLightMatrices();
	
	// Clear the screen and depth buffer so we can render from the light's view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Set the current viewport to our texture size
	glViewport(0, 0, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT);

	// Now we just need to set the matrices for the light before we render
	glMatrixMode(GL_PROJECTION);
	
	// Push on a matrix to make sure we can restore to the old matrix easily
	glPushMatrix();

		// Set the current projection matrix to our light's projection matrix
		glLoadMatrixf(g_mProjection);

		// Load modelview mode to set our light's modelview matrix
		glMatrixMode(GL_MODELVIEW);

		// Load the light's modelview matrix before we render to a texture
		glLoadMatrixf(g_mModelView);

		// Since we don't care about color when rendering the depth values to
		// the shadow-map texture, we disable color writing to increase speed.
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 

		// Eliminate artifacts caused by shadow mapping
		glPolygonOffset(8.0f, 4.0f);
	
		// This turns of the polygon offset functionality to fix artifacts.
		// Comment this out and run the program to see what artifacts I mean.
		glEnable(GL_POLYGON_OFFSET_FILL);

		// Render the world according to the light's view
		RenderWorld();
		
		// Now that the world is rendered, save the depth values to a texture
		glBindTexture(GL_TEXTURE_2D, g_Texture[SHADOW_ID]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT);

		// We can turn color writing back on since we already stored the depth values
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 

		// Turn off polygon offsetting
		glDisable(GL_POLYGON_OFFSET_FILL);

		// Restore our normal viewport size to our screen width and height
		glViewport(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

		// Go back to the projection mode and restore the original matrix
		glMatrixMode(GL_PROJECTION);

	// Restore the original projection matrix
	glPopMatrix();

	// Go back to modelview model to start drawing like normal
	glMatrixMode(GL_MODELVIEW);

	// Clear the color and depth bits and start over from the camera's view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();										
	g_Camera.Look();	

	// Pass in the global matrix that will store our modelview inverse matrix.
	g_Camera.GetInverseMatrix(g_mCameraInverse);

	// Render the world and apply the shadow map texture to it
	ApplyShadowMap();

	// The rest of the code below just draws and moves the light.  You don't need it.

	GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack
	gluQuadricDrawStyle(pObj, GLU_FILL);				// Draw the sphere normally

	// Draw the light at the light's position
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glTranslatef(g_LightPosition[0], g_LightPosition[1], g_LightPosition[2]);
	gluSphere(pObj, 0.05f, 25, 25);
	
	gluDeleteQuadric(pObj);								// Free the Quadric

	SwapBuffers(g_hDC);									// Swap the back buffers to the foreground

	// If the light position goes out of the screen to the left, reset the direction and position.
	if(g_LightPosition[0] < -2.0f)
	{
		g_Direction = g_Direction * -1;
		g_LightPosition[0] = -2.0f; 
	}
	// If the light position goes out of the screen to the right, reset the direction and position.
	if(g_LightPosition[0] > 2.0f)
	{
		g_Direction = g_Direction * -1;
		g_LightPosition[0] = 2.0f; 
	}
	
	// Each frame we move the light by the current direction, then update the light with GL.
	g_LightPosition[0] += g_Direction;
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );
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
        lRet = (long)DefWindowProc(hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Since the only code that really changed was in ShadowMappingPCF.frag, we will
// only explain that file.  If you would like an explanation on the vertex shader,
// please look at the previous tutorial on shadow mapping for a line-by-line
// explanation.  Take a look first at the fragment file below:
//
// * ShadowMappingPCF.frag shader file explanation *

/*

uniform sampler2DShadow shadowMap;
varying vec4 projCoord;

void main ()
{
	const float kTransparency = 0.3;
	vec4 color = gl_Color;

	vec3 shadowUV = projCoord.xyz / projCoord.q;
	float mapScale = 1.0 / 512.0;

	vec4 shadowColor = shadow2D(shadowMap, shadowUV);

	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale, -mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale,  	      0, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale, -mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale,  	      0, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(        0,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(        0, -mapScale, 0));

	shadowColor = shadowColor / 9.0;

	shadowColor += kTransparency;
	shadowColor = clamp(shadowColor, 0.0, 1.0);

	if(shadowUV.x >= 0.0 && shadowUV.y >= 0.0 && shadowUV.x <= 1.0 && shadowUV.y <= 1.0 )
	{
		gl_FragColor = color * shadowColor;
	}
	else
	{
		gl_FragColor = color;
	}
}

*/

// Can you see that we aren't using the shadow2DProj() function anymore?
// This is because we don't want to be handling PCF in 4D coordinates.
// To make it easier, as well as show new functions in GLSL to do shadow
// mapping, we use the shadow2D() function instead.
//
// The implementation is very similar to what we did in the previous tutorial,
// except we do the perspective divide first to convert to 3D coordinates,
// which are passed into shadow2D().  This function returns a color value,
// unlike shadow2DProj().r giving us just a float value.
//
// We then start creating offsets into the shadow map and then add up all the
// surrounding pixels before dividing by 9 to get our average shadow intensity.
// In order to know how far the surrounding pixels are, we must first determine
// this by dividing one by the shadowmap texture size.  Since we are using 512
// we just do this:  1.0 / 512.0.
// 
// Just like the last tutorial we don't want the shadows to be so dark, so we
// add a transparency value, then clamp the final value from 0 to 1.  Then we
// also make sure that the shadow coordinates are within the light's frustum.
// If not, we don't apply any shadow value to that pixel, just the lit color.
//
// Let's go through the fragment shader line-by-line and explain it:
//

/*

*** This is our shadow map texture variable.
uniform sampler2DShadow shadowMap;

*** This stores our 4D shadow coordinates.
varying vec4 projCoord;

*** Start the fragment program
void main ()
{
	*** Create a const variable for our transparency.
	const float kTransparency = 0.3;

	*** This value will store our final color value.
	vec4 color = gl_Color;

	*** Do the perspective divide to convert our coords to 3D from 4D.
	vec3 shadowUV = projCoord.xyz / projCoord.q;

	*** This tells us the distance between each depth value in the texture map.
	float mapScale = 1.0 / 512.0;

	*** We first want to store the first (middle) depth value
	vec4 shadowColor = shadow2D(shadowMap, shadowUV);

	*** Now we just create some surrounding offsets for the middle depth value.
	*** We just add a XYZ offset to our XYZ current shadow coordinate.  The order
	*** in which we add them up obviously isn't important.
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale, -mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3( mapScale,  	      0, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale, -mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(-mapScale,  	      0, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(        0,  mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowUV.xyz + vec3(        0, -mapScale, 0));

	*** Since we added up 9 values, we just divide by 9 to get the average shadow pixel.
	shadowColor = shadowColor / 9.0;

	*** Just like the last tutorial, we add a transparency and clamp the result.
	shadowColor += kTransparency;
	shadowColor = clamp(shadowColor, 0.0, 1.0);

	*** Make sure that the shadow coordinates are in the light's frustrum.
	*** If they aren't we just use the color, otherwise we apply the shadow color.
	if(shadowUV.x >= 0.0 && shadowUV.y >= 0.0 && shadowUV.x <= 1.0 && shadowUV.y <= 1.0 )
	{
		gl_FragColor = color * shadowColor;
	}
	else
	{
		gl_FragColor = color;
	}
}

*/

// Not very complicated at all was it?  Still, it's good to just focus on the
// technique and not be bogged down by other complicated code and new techniques.
// That's it.  If you want more blurred shadows you will have to calculate more
// offsets to blur.  For nice PCF shadows it takes generally about 32 samples
// of surrounding depth values.  Good luck!
//
//
// © 2000-2005 GameTutorials