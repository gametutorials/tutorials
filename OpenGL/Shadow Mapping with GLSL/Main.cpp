//*********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersShadowMapping							 //
//																		 //
//		$Description:	Shadow-mapped shadows using GLSL				 //
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
// Welcome to our first shadowing tutorial with shaders!  You have nothing to fear
// because shadowing with shaders is very easy.  Working off our previous tutorial
// on shadow mapping using fixed functionality, we add and take away just a few
// things to implement shadow mapping using GLSL.
//
// By reading this tutorial it is assumed that you understand the basics of shadow
// mapping, or you have gone over our previous tutorial on shadow mapping.  If
// any of the concepts in this tutorial seems fuzzy, please go back and review.
// Just to recap, let's describe what shadow mapping is.
//
// * Quick review on shadow mapping *
//
// Basically, shadow mapping is a way to calculate shadows using 2 passes,
// regardless of the world geometry's shapes or size.  What we do is render the
// light's view to a texture and just store the depth values.  We then project
// our current camera's view into the light's clip space and compare the depth
// values to see if there should be a shadow applied to each pixel or not.
//
// There are a couple ways to do shadow mapping in GLSL; however, we will use
// the GLSL function called shadow2DProj().  In order for this to work we still
// need to use the same ARB modes for our texture parameters like we did in the
// last tutorial.  These give shadow2DProj() which mode and function to use for
// shadow mapping.  We will go over this function in detail at when we get to
// the shader code at the bottom of this file.
//
// * What's different when using shaders? *
//
// The benefits of course are that you have more control.  However, in regards to
// the implementation, we do not use texture generation to compute the texture
// coordinates for the shadow map.  Instead, like we explained in the previous
// tutorial, we calculate the inverse of our camera matrix by ourselves.  Well, to
// be more specific, we calculate the inverse of the modelview matrix with the
// camera applied to that matrix.  So it's not just the camera's inverse, but the
// modelview matrix's inverse.  The camera needs to be applied to that matrix.  Other
// than loading and starting the shaders, nothing is different in our source and
// header files.  This is good news because we can then focus primarily on the
// shader implementation and not more code in our project.  You can thank us later :)
//
// * No lighting calculated? *
//
// This lesson works off of the previous shadow-mapping tutorial, however we don't
// calculate lighting in order to simplify the tutorial.  Everything about the scene
// is still the same.  We still have a spinning tube and a sphere in front of some 
// walls.  There is also the same light moving back and forth to show the changing 
// shadow, dependent on the light's position.  
//
// If you would like to check out a version of this tutorial that shows the view of
// the light in the top left corner, run the program ShadersShadowMapping_Ortho.exe.
// This is stored in the project's folder.  I just rendered the light's view to a
// normal RGB texture and used ortho mode to display it as a 2D bitmap.
//
// The controls for this tutorial are just the typical camera controls with the mouse
// and arrow keys.  Let's jump into the code.
//

// This is our global shader object that will load the shader files
CShader g_Shader;

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

// Below is our starting light position
float g_LightPosition[4] = {0.0f, 2.0f, 5.0f, 0.0f};

// We set the light's view position at the origin
float g_LightView[3] = {0.0f, 0.0f, 0.0f};

// This is the current direction and speed the light is moving
float g_Direction = -0.04f;

// These are the light's matrices that need to be stored
float g_mProjection[16] = {0};
float g_mModelView[16] = {0};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Since we are not using texture generation, we need to calculate the
// camera's inverse matrix to follow the projection equation.  This will
// store the inverse of the modelview matrix that has the camera position
// applied already to it.
float g_mCameraInverse[16] = {0};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Let's init our GLSL functions and make sure this computer can run the program.
	InitGLSL();		

	// Here we pass in our new vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("ShadowMapping.vert", "ShadowMapping.frag");

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// This function is the main function that needs to be changed from the
	// previous tutorial.  We needed to take out the texture generation code
	// so a couple things will change.  This means we will need to turn our
	// shadow on, as well as multiply the inverse of our camera matrix by
	// the light's projection and modelview matrix.  We still include the
	// bias matrix to convert the clip space to a 0 to 1 ratio instead of
	// a -1 to 1 ratio.

	// Let's turn our shaders on for doing shadow mapping on our world
	g_Shader.TurnOn();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Turn on our texture unit for shadow mapping and bind our depth texture
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, g_Texture[SHADOW_ID]);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// In order for the shader to have access to our light's texture of depth 
	// values, we need to tell our fragment shader the texture unit that our
	// texture is being bound to.  Since we use GL_TEXTURE0_ARB we pass in 0.
	GLuint uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "shadowMap"); 
	glUniform1iARB(uniform, 0);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

		// Instead of using texture generation functions that handle the inverse
		// camera matrix for us, we calculated this ourselves and need to multiply
		// it by the preceding matrices.  Remember the equation for projection:
		// 
		// mProjectedTexture = mLightProjection * mLightModelview * mCameraInverse;
		//
		// We need to use this because we are projecting the shadow map onto the
		// world from the light's position.  A bit tricky to wrap your head around?
		// Keep in mind, the camera's inverse matrix is really the modelview matrix
		// inverted with the camera already applied to it.

		// Following the above equation, multiply our camera's inverse by the matrices.
		glMultMatrixf(g_mCameraInverse);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	glMatrixMode(GL_MODELVIEW);			// Switch back to normal modelview mode

	RenderWorld();						// Render the world that needs to be shadowed

	// Now that the world is shadowed and we are done with the texture generation,
	// let's set everything back to normal by resetting the texture matrix.
	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	// Turn the first multi-texture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Light expected, we need to turn our shader off since we are done
	g_Shader.TurnOff();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// After we have switched backed to our camera's view, we need to
	// grab the inverse of the modelview's matrix.  If you are unfamiliar
	// with what an inverse of a matrix does, let's review.  If you
	// multiply a matrix by it's inverse, it basically zeros it out.
	// We need the inverse because in the vertex shader we will have a
	// vertex that is multiplied by the camera's view matrix (modelview),
	// and we need to bring that back down to world space and project
	// it into the light's clip space.  Since we are working in the light's
	// clip space and not the camera's clip space this needs to happen.

	// For this to work, we need to make sure we have called the Look()
	// function from the camera object before calling GetInverseMatrix().

	// Pass in the global matrix that will store our modelview inverse matrix.
	// This function grabs the current modelview matrix and returns the inverse.
	g_Camera.GetInverseMatrix(g_mCameraInverse);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
// As you can see, there isn't much to talk about in the way of our source and
// header files, but below we will go over both shader files to explain how
// our shadow-mapping technique works with GLSL.  The only other file you will
// want to look at is our Camera.cpp file, which has the GetInverseMatrix()
// function we created in this tutorial.  Let's explain the shader files now.
//
//
// * ShadowMapping.vert shader file explanation *

/*

varying vec4 projCoord;

void main()
{
	vec4 realPos = gl_ModelViewMatrix * gl_Vertex;
  
	projCoord = gl_TextureMatrix[0] * realPos;
	gl_FrontColor = gl_Color;

	gl_Position = ftransform();
}

*/

// So, what our vertex shader is doing is storing the projection coordinates
// that will be used for our shadowmap, converting the vertex into the
// light's clip space.  We also store the current color of the object and
// transform the current vertex by our projection and modelview matrix.
//
// Let's explain every line now:
//

/*

*** This variable is used in the vertex and fragment shader for
*** the projected coordinates that will be passed into our shadowmap.
varying vec4 projCoord;

*** Start the vertex shader program
void main()
{
	*** We need to put the passed in vertices into world space,
	*** since they need to be that way when they are projected into the
	*** light's clip space.  That way we can check for correct visibility.
	vec4 realPos = gl_ModelViewMatrix * gl_Vertex;
  
	*** Now that we have the light projection and modelview matrix loaded
	*** into our texture (also the camera inverse), we multiply the texture
	*** matrix by the current vertex in world space.  This gives us our
	*** projection coordinates that will be passed into shadow2DProj() in
	*** our fragment shader.
	projCoord = gl_TextureMatrix[0] * realPos;

	*** Store the current object's color
	gl_FrontColor = gl_Color;

	*** Assign the current vertice's position in camera's clip space
	gl_Position = ftransform();
}

*/

// That wasn't so bad, so let's jump into the fragment shader, which is
// probably even easier to understand.  First take a look at the file:
//
// * ShadowMapping.frag shader file explanation *

/*

uniform sampler2DShadow shadowMap;
varying vec4 projCoord;

void main ()
{
	const float kTransparency = 0.3;
	vec4 color = gl_Color;

	float rValue = shadow2DProj(shadowMap, projCoord).r + kTransparency;
	rValue = clamp(rValue, 0.0, 1.0);

	vec3 coordPos  = projCoord.xyz / projCoord.w;

	if(coordPos.x >= 0.0 && coordPos.y >= 0.0 && coordPos.x <= 1.0 && coordPos.y <= 1.0 )
   	{
		gl_FragColor = color * rValue;
	}
	else
	{
		gl_FragColor = color;
	}
}

*/

// What is going on in the fragment is that we are passing in our projection
// coordinates (that was calculated in the vertex shader) into shadow2DProj,
// which returns a one or a zero.  If it returns a zero, then there is a shadow
// for this pixel.  If it returns a one, then there is no shadow at the current
// pixel.  
// 
// Notice that we add a transparency value to the result.  The "r" value is
// the result value.  If we don't add a transparency value then the color of
// the shadow will always be pure black, since the value is multiplied by the
// color of the current object.  If it has a shadow (zero) then it will remain zero.
// We then do a clamp to make sure the added transparency doesn't intensify our 
// object's original color.  Dont stress about the clamp, you can delete it if you
// don't care if your color is a bit intensified.
//
// Next, we do what is called a perspective divide, which converts our 4D
// coordinates into 3D.  To do this, we just divide the XYZ values by the W value.
// We then can work in 3D, not 4D.  We usually wouldn't have to do this, but in
// order to get rid of artifacts of pixels outside of the shadowmap texture we
// want to only apply the shadow mapping value to our world if the current
// coordinates aren't negative and are less than or equal to 1.0.  In the
// previous fixed-functionality example of shadow mapping, you'll notice that
// if the light's frustum is smaller than the world's data then black artifacts
// appear on the outside.  To get rid of these we just need to make sure that we
// don't try and use shadow mapping if it isn't possible (i.e. outside the frustum).
// 
// If the current coordinates are in the bounds of the light's frustum then we
// multiply the shadow color by the current object's color.  Otherwise, let's
// just assign the object's color without any shadowing.
//
// Let's go through the fragment shader line-by-line and explain it:
//

/*

*** This is the shadow-mapping texture.  We want to use the special keyword
*** called sampler2DShadow, instead of sampler2D.  This tells GLSL that this
*** texture map is a depth map.
uniform sampler2DShadow shadowMap;

*** These shadow map projection coordinates were calculated in the vertex shader.
varying vec4 projCoord;

*** Start the fragment shader program
void main ()
{
	*** This is a constant float that has a simple transparency value.
	const float kTransparency = 0.3;

	*** This variable will hold our final color for the pixel
	vec4 color = gl_Color;

	*** Now comes the most important part.  We pass in our shadow map texture
	*** as the first parameter, then the 4D projection coordinates.  We then
	*** access the ".r" value of the function, which gives us a 1 or 0.  0 is
	*** a shadow, 1 is no shadow.  We add the transparency value to lighten it.
	float rValue = shadow2DProj(shadowMap, projCoord).r + kTransparency;

	*** We make sure that the rValue is between 0 and 1 since we added transparency.
	rValue = clamp(rValue, 0.0, 1.0);

	*** Here we do a perspective divide to convert the 4D coordinates into 3D.
	vec3 coordPos  = projCoord.xyz / projCoord.w;

	*** Now we make sure that the coordinates aren't outside the light's frustum.
	*** This would give us dark artifacts and unexpected results.
	if(coordPos.x >= 0.0 && coordPos.y >= 0.0 && coordPos.x <= 1.0 && coordPos.y <= 1.0 )
   	{
		*** Multiply the shadow value by our current object's color.
		gl_FragColor = color * rValue;
	}
	else
	{
		*** There coordinates are outside the light's frustum, so default to gl_Color.
		gl_FragColor = color;
	}
}

*/

// Pretty basic huh?  Shadow mapping is pretty simple to get going, but the next step
// is using a technique like PCF to "blur" the shadows so that they aren't so blocky
// and aliased.  The next tutorial covers basic "Percentage Closer Filtering" to increase
// the quality of the shadows.  It's a simple tutorial, but well worth viewing.
//
// Remember that when you use GL_LINEAR for your texture parameters you automatically
// get a basic blur for your shadows.  This of course, isn't good enough by most standards.
// Check out the next tutorial on PCF filtering bring your shadow quality up a notch.
//
// Be sure to check out the program ShadersShadowMapping_Ortho.exe.  This shows the light's
// view for the current shadow map.  Notice that the world leaves the light's frustum.  If
// you comment out the perspective divide code and the check if the coordinates are in the
// light's frustum, you can see the correlation of when the world leaves the light's
// frustum the black artifacts on the side appear.
//
//
// © 2000-2005 GameTutorials