//**********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadowMapping									 //
//																		 //
//		$Description:	Shadow-mapped shadows using fixed functionality	 //
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
float g_DT;												// Global float that stores the elapsed time between the current and last frame



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// In this tutorial we use a technique called "shadow mapping" to cast shadows
// on our world.  This version of shadow mapping will be done using fixed
// functionality, which means we will not be using shaders, just normal OpenGL
// calls.  The next tutorial on shadows uses GLSL to implement GPU shadow mapping.
//
// * What is shadow mapping? *
//
// Shadow mapping, developed by Lance Williams in 1978, is a technique that renders 
// the world to a texture from a light's position, storing the data as a depth 
// texture (storing the z-buffer values).  The world is then rendered in the camera's
// position.  When rendering the world we test the light view's depth texture values 
// against the current camera view's depth values to see if there is a shadow.  
//
// The two depths values are tested in the light's clip space.  If the camera 
// view's depth value is greater than the light view's depth value, then obviously 
// there is something in front of the light, which means the light can't see what 
// the camera is seeing from the light's position.  This is where we would give that 
// pixel a shadow.  This test is done for every pixel that we see in the camera's view.
//
// * How do we test the depth values per pixel? *
//
// OpenGL has some nice nice glTexParameteri() options to do the shadow-mapping checks
// for us.  We will go over these options for glTexParameteri() later in the tutorial.
// We use texture space (GL_TEXTURE_MATRIX) to load the light's projection and 
// modelview matrix, where the shadow-mapping calculations can be done.
// 
// * How are the shadow texture coordinates generated? *
//
// We use OpenGL's texture generation functionality (S, T, R, Q) to project the shadows 
// onto the world.  The calculations are done using the texture matrix, and then shadow
// map is projected onto our world for the current texture unit we are using (0).
//
// * Benefit from shadow mapping *
//
// The greatest benefit that shadow mapping offers is that it takes only two passes to
// give a shadow for the world, even self-shadowing of objects in the world.  If objects
// are in front of each other the shadows are generated appropriately.  Pixar's animation
// studio has even used shadow mapping in their movies.  Shadow mapping is fast.  It also
// doesn't matter how complex your geometry is, it still handles the shadows the same.
// This means curves, flat surfaces, etc.
//
// * Downside of shadow mapping *
//
// Unlike other shadow methods, shadow mapping gives very aliased results.  The quality
// of the shadows depend on your light's perspective and near/far plane.  Since the shadow
// is created from a depth map, the size of the texture that the light's view is rendered
// to also determines the quality.  We use 512x512, which gives decent quality.  The higher
// the texture size the better.  Unless you only want your program to get good frame rates
// on a GeForce 6800 video card or higher, you will need to use power-of-two texture sizes.
// This tutorial only does a directional light, but if you want a point light you will need
// to implement a more complicated version of shadow mapping called "Parabolic Shadow Maps".
// To increase the quality of the shadow there is also some filtering options available. 
//
// * PCF Filtering *
//
// PCF filtering stands for "Percentage Closer Filtering", which is basically a way to take
// samples around the current pixel in the shadow map and perform a nice blurred effect
// around the edges of the shadow.  To get a really good visual effect you will need at
// least 16 to 32 samples for every pixel, which is obviously a hit to your performance.
// Since you can't just blur the depth values of our depth map, you must blur after you
// calculate if the current pixel has a shadow or not.  Check out our GLSL tutorial that 
// shows a basic PCF example.  Be sure to check out the GLSL shadow mapping tutorial first.
//
// * Automatic PCF filtering *
//
// Just by setting our texture to GL_LINEAR for the GL_TEXTURE_MAG_FILTER we get basic
// 2x2 PFC filtering automatically.  Notice the slight blurring around the shadow's edges
// when you get up close to the walls.  If you have GL_NEAREST you will see that there
// is no PFC filtering applied to the shadow, which gives a strong aliased shadow.
//
//
// This lesson works off of the previous planar shadows tutorial.  We have a spinning
// tube and a sphere in front of some walls.  There is also a light moving back and forth
// to show the changing shadow, dependent on the light's position.  This lesson uses a
// directional light instead of a point lights since a point light is a lot more
// complicated to use with shadow mapping.  
//
// The controls for this tutorial are just the typical camera controls with the mouse
// and arrow keys.  Let's jump into the code.
//

// These store our width and height for the shadow texture.  The higher the
// texture size the better quality shadow.  Must be power of two for most video cards.
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

// This mode is what will compare our depth values for shadow mapping.
// R refers to the depth value in the texture that will be compared.
// Notice that we don't need to render the world to a depth texture
// because this mode will already handle the comparisons between the
// depth texture and the world's z-buffer values (depth values) for us.
#define GL_COMPARE_R_TO_TEXTURE_ARB       0x884E

// The texture array where we store our image data
UINT g_Texture[MAX_TEXTURES];	

// Our camera object so we can move around the world.
CCamera g_Camera;

// Our function pointers for the ARB multi-texturing functions
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;

// Below is our starting light position
float g_LightPosition[4] = {0.0f, 2.0f, 5.0f, 0.0f};

// We set the light's view position at the origin
float g_LightView[3] = {0.0f, 0.0f, 0.0f};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is the current direction and speed the light is moving
float g_Direction = -0.04f;

// These arrays will store our 4x4 matrices for the light's
// project and modelview matrix.  These will then be loaded
// into the texture matrix for the shadow mapping.
float g_mProjection[16] = {0};
float g_mModelView[16] = {0};


///////////////////////////////// STORE LIGHT MATRICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function positions our view from the light for shadow mapping
/////
///////////////////////////////// STORE LIGHT MATRICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void StoreLightMatrices()
{
	// In this function we just set our camera position to the light's position
	// and then store the current modelview matrix.  Lastly, we set our light's
	// frustum (perspective) to set our depth precision.  Ideally, the smaller
	// the frustum is, the more precision our depth map is, which gives a better
	// quality shadow.  Notice that we change the perspective to a 60 degree field
	// of view.  This allows us to view more of the world from our light's position.
	// That way we can have closer near and far planes, giving better depth values.
	
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

		// We could go to prospective mode and change the perspective,
		// then have to restore our camera's perspective and go back to modelview
		// mode.  However, if we just start with a new modelview matrix, then change
		// the perspective and grab the current matrix we save some steps and restorations.

		// Reset the current matrix
		glLoadIdentity();

		// Change our light's perspective to 60 degrees FOV, aspect ratio 1, with a near
		// and far plane of 0.5 and 25.0.  The far plane is only 25 because our world isn't
		// larger than 25.  Depending on how large your world is you will need to increase
		// the far plane and/or add additional lights.
		gluPerspective(60.0f, 1.0f, 0.5f, 25.0f);

		// Even though we want the perspective matrix, we can still grab the
		// current modelview matrix and use that as our projection matrix because
		// we used gluPerspective() on a new matrix, which is the projection matrix.
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
	// This function is a modified version of what we have used in previous tutorials.
	// We just allowed the option of doing a depth texture.  When doing a depth
	// texture we need to pass in GL_DEPTH_COMPONENT for the type AND the channels.
	// To check for this we just see if the channels are greater than 4.  If so, we
	// allocate a texture for just one bit per pixel, then pass in the channel value
	// for glTexImage2D().

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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

	// Here we allocate memory for our depth texture that will store our light's view.
	// We must set the channels and type for the texture as GL_DEPTH_COMPONENT.
	CreateRenderTexture(g_Texture, SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, SHADOW_ID);

	// Set the camera:		Position		View		 Up Vector
	g_Camera.PositionCamera(0, 9, 12,     0, 2.5, -2,     0, 1, 0);

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
	// This function will draw our entire scene except for the light.
	// We could speed up our application by not using quadrics and using
	// display lists, but just to make things simple I chose to not add extras.

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
	// Now comes the juice of this tutorial.  What this function will do is:
	//
	// 1) Turn on texture mapping 
	// 2) Bind our shadow texture that holds the depth values
	// 3) Set our texture mode and function for shadow mapping
	// 4) Turn on and set OpenGL texture generation by GL_EYE_LINEAR
	// 5) Load the light's matrices into the texture matrix
	// 6) Render the world that will be shadowed
	// 7) Restore and turn everything off again
	//

	// Turn on our texture unit for shadow mapping and bind our depth texture
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D, g_Texture[SHADOW_ID]);

	// Here is where we set the mode and function for shadow-mapping in fixed functionality.
	// The mode we use is GL_TEXTURE_COMPARE_MODE_ARB, with GL_COMPARE_R_TO_TEXTURE_ARB, which
	// tells OpenGL that we want to compare the depth value in our world to the current
	// texture's depth value.  We then set the compare function to GL_LEQUAL, which says that
	// we will not shadow the current pixel if the depth value is less than or equal to
	// the texture's depth value.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	// Let's explain really fast about why we need a "bias" matrix.  Well, when we multiply 
	// our world data by the projection and modelview matrix it converts it to clip space, 
	// which is a box that is measured in -1 to 1.  Then this is later converted to screen 
	// coordinates.  Now, our texture coordinates are clamped from 0 to 1 right?  We are
	// working in the light's clip space and we want to convert the -1 to 1 ratio to a
	// 0 to 1 ratio, which will measure up with our texture coordinates.  To do this we
	// need a "bias" matrix that will convert from -1 to 1 to a 0 to 1 ratio.  We first
	// load the bias matrix into our texture matrix and then multiply the projection and
	// modelview matrix by the bias matrix.  The bias matrix that will do this is below.
	// It's the same thing as doing this to every coordinate: newCoord = (oldCoord + 1) / 2;
	const float mBias[] = {0.5, 0.0, 0.0, 0.0, 
						  0.0, 0.5, 0.0, 0.0,
						  0.0, 0.0, 0.5, 0.0,
						  0.5, 0.5, 0.5, 1.0};

	// Now comes the complicated part to grasp at first.  If you have never used the
	// texture generation functionality in OpenGL for sphere mapping or something else,
	// I will give a basic explanation of what it does.  So that we don't have to
	// do the work ourselves of calculating every UV coordinate, we can have the UV 
	// coordinates generated by OpenGL for us, given a bunch of different inputs to work from.
	// We can use OpenGL's texture generation functionality to project our camera depth values
	// into the light's clip space.  Since our matrices are 4x4, we need to do our
	// projection in 4D.  Thus, we use texture coordinates S, T, R and Q.

	// We first create planes for each axis (x, y, z, w).  This will be used
	// with our projection of our camera's view to the light's clip space and
	// make sure the texture coordinates don't move when our camera moves, but
	// are the same no matter where our camera is looking.  The equation for
	// doing projected texturing is:  
	//
	// mProjectedTexture = mLightProjection * mLightModelview * mCameraInverse;
	//
	// We say the "camera inverse", but really it is the modelview matrix's inverse
	// that has the camera's view applied to it.  So this is what we mean when we say that.
	// Usually we will need to multiply the inverse of our camera matrix by the projection 
	// and modelview matrix of the light; however, when using these planes for our camera
	// OpenGL will calculate the inverse of our camera for us.  In the GLSL version of
	// shadow mapping we calculate the camera inverse ourselves.

	// Create the eye planes for generating texture coordinates
	const double xPlane[] = {1.0, 0.0, 0.0, 0.0};
	const double yPlane[] = {0.0, 1.0, 0.0, 0.0};
	const double zPlane[] = {0.0, 0.0, 1.0, 0.0};
	const double wPlane[] = {0.0, 0.0, 0.0, 1.0};

	// Enable texture generation for S, T, R, and Q
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	// Set each texture coordinate's texture gen mode to use the camera.
	// This will have OpenGL incorporate the camera's inverse for us.
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

	// Set our planes for each coordinate in order to project the texture appropriatly.
	glTexGendv(GL_S, GL_EYE_PLANE, xPlane );
	glTexGendv(GL_T, GL_EYE_PLANE, yPlane );
	glTexGendv(GL_R, GL_EYE_PLANE, zPlane );
	glTexGendv(GL_Q, GL_EYE_PLANE, wPlane );

	// Now we actually do the matrix multiplication.  First we switch to texture
	// mode.  That way we aren't effecting the modelview matrix.  Then we load in 
	// the bias matrix, multiply that by the light's projection matrix, then multiply
	// that result by the light's modelview matrix.  This, in conjunction with our
	// camera inverse matrix calculated by OpenGL for us, gives us the right matrix
	// for projecting everything that needs to be projected.  That is, the camera's
	// depth values into the light's clip space, then the generated shadow map onto
	// the geometry of the world.
	glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(mBias);			// The bias matrix to convert to a 0 to 1 ratio
		glMultMatrixf(g_mProjection);	// The light's projection matrix
		glMultMatrixf(g_mModelView);	// The light's modelview matrix
	glMatrixMode(GL_MODELVIEW);			// Switch back to normal modelview mode

	RenderWorld();						// Render the world that needs to be shadowed

	// Now that the world is shadowed and we are done with the texture generation,
	// let's set everything back to normal by resetting the texture matrix.
	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	// Turn off texture generation for our S, T, R and Q coordinates
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	// Turn the first multi-texture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);
}


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{	
	// This is the last function that we need explaining for this tutorial.
	// We have done all the shadow mapping work above this, but we still haven't
	// gone over actually rendering the depth values of the light's view to a 
	// texture.  Let's go over each line and see how this is done.  It's simple.

	// Since our light is moving every frame we will need to update the light's
	// matrices.  We really only need to update the modelview matrix every frame,
	// but we do the projection matrix too just to make it a simple function call.
	// If your light never moves, you only need to call this function once.
	StoreLightMatrices();
	
	// Clear the screen and depth buffer so we can render from the light's view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Since our texture map that will store the depth values is a different size
	// than our normal viewport, we need to change the viewport to make it the same
	// size as our texture.
	glViewport(0, 0, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT);

	// Now we just need to set the matrices for the light before we render.
	// We switch to projection mode and input our light's projection matrix.
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

		// This special function fixes artifacts that lead to small errors when
		// calculating the shadow map.  You will need to adjust the scale depending
		// on your scene, but I chose the scale 8.0 for mine.  NVIDIA suggests 1.1
		// or higher.  The 4.0 should stay fairly the same.
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

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
// That was a pretty simple tutorial right?  There is a million ways to make
// it more complicated, but this is the simplest way I could think of to show
// the technique of shadow mapping.
//
// Just a quick review of how shadow mapping works:
//
// We render the scene from the light's view and store the depth values in a
// texture.  We then render the world normally from the camera's view.  Texture
// generation is used to calculate and project our texture coordinates for the
// shadow mapping.  We test the depth values of the light's view and the camera's
// view in the light's clip space to see if there is a shadow.  Basically, if
// the camera can see something that the light can't, that means that that part
// should be shadowed.
//
// * Optimizations *
// 
// It would be faster to not use quadrics, as well as create display lights to
// call for each object.  I didn't do these optimizations to make the tutorial
// as simple as possible.  Check out our PCF Shadow Mapping tutorial to see how
// to make the shadows more anti-aliased.
//
// You should notice that since the light is so close to the world there is
// artifacts outside of the shadow map that is projected.  To get rid of these
// you will want to move the light farther away or make the perspective's FOV
// wider than 60.  In our GLSL version we eliminate these artifacts completely.
// Please move on and view this tutorial.
// 
// 
// © 2000-2005 GameTutorials