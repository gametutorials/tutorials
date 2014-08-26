//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersToon										 //
//																		 //
//		$Description:	Using GLSL we toon shade an MD2 model			 //
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
// Welcome to our basic toon shading tutorial!  There are many techniques to do
// toon shading.  In our Direct3D section we use a different technique for toon
// shading; however this lesson uses an extremely simple technique also used at
// http://www.LightHouse3D.com/.  Basically, all we do is take the dot product
// between the normalized light direction and current normal, which gives us our
// intensity.  We then do 4 different if statements with the intensity and assign
// different light values to different ranges.  Check the ToonShading.vert for
// these ranges.  This creates the cartoon-shading effect.
//
// * What is toon shading? *
//
// For those of you who might not understand what this shading is, I'll give a
// quick explanation.  Toon shading, also called "cell shading", is a way to
// light a model in a way that it looks more like a comic book or cartoon
// drawing.
//
// In this tutorial we took the code from our MD2 loader tutorial to be able
// to show this shading on a cool character.  We don't texture the model to
// keep it really simple.  Like the MD2 loader tutorial, you can rotate the 
// character by using the LEFT and RIGHT arrow keys.  You can also click the
// left mouse button to switch to wire-frame mode.
//

#define FILE_NAME  "Knight.md2"							// This is the 3D file we will load.

CLoadMD2 g_LoadMd2;										// This is MD2 class for loading the model
t3DModel g_3DModel;										// This holds the 3D model info that we load in

int   g_ViewMode	  = GL_TRIANGLES;					// We want the default drawing mode to be normal
float g_RotateX		  = 0.0f;							// This is the current value at which the model is rotated
float g_RotationSpeed = 0.5f;							// This is the speed that our model rotates.  (-speed rotates left)


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our global shader object
CShader g_Shader;

// Below is our starting light position and moving X direction
float g_LightPosition[4] = {0.0f, 2.0f, 80.0f, 0.0f};


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

	// To create our toon shading effect we need to set our lights that we
	// want to use.  The shading takes the lightest color as the specular.
	// The next color shade takes the diffuse color.  The third shade takes
	// 2/3 the diffuse.  Finally, the dark outline color is the ambient color.

	float ambience[4] = {0.0f, 0.0f, 0.2f, 1.0f};		// The color of the light in the world
	float diffuse[4] =  {0.0f, 0.0f, 0.6f, 1.0f};		// The color of the positioned light
	float specular[4] = {0.0f, 0.0f, 1.0f, 1.0f};		// The color of the bright specular light
	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambience );		// Set our ambience values (Default color without direct light)
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuse );		// Set our diffuse color (The light color)
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		// Let's set our specular light
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );	// This Sets our light position

	glEnable(GL_DEPTH_TEST);							// Enable depth checking for our model
	glClearColor(0.2f, 1.0f, 0.2f, 1.0f);				// Make the BK color all ... fun looking :)

	g_LoadMd2.ImportMD2(&g_3DModel, FILE_NAME, NULL);	// Load the MD2 model and store it

	glEnable(GL_CULL_FACE);								// Turn culling on
	glCullFace(GL_FRONT);								// Quake2 uses front-face culling apparently

	// Here we pass in our new vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("ToonShading.vert", "ToonShading.frag");

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the model to the screen
/////
///////////////////////////////// DRAW MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawModel()
{
	glRotatef(g_RotateX, 0, 1.0f, 0);						// Rotate the object around the Y-Axis
	g_RotateX += g_RotationSpeed;							// Increase the speed of rotation

	// Make sure we have valid objects just in case. (size() is in the vector class)
	if(g_3DModel.pObject.size() <= 0) return;

	// Get the current object that we are displaying
	t3DObject *pObject = &g_3DModel.pObject[0];

	// Render lines or normal triangles mode, depending on the global variable.
	glBegin(g_ViewMode);

		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// Go through each corner of the triangle and draw it.
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				// Get the index for each point in the face
				int index = pObject->pFaces[j].vertIndex[whichVertex];

				// Get the index for each texture coord in the face
				int index2 = pObject->pFaces[j].coordIndex[whichVertex];
			
				// Give OpenGL the normal for this vertex.  Notice that we put a 
				// - sign in front.  It appears that because of the ordering of Quake2's
				// polygons, we need to invert the normal
				glNormal3f(-pObject->pNormals[ index ].x, -pObject->pNormals[ index ].y, -pObject->pNormals[ index ].z);
					
				// Make sure there was a UVW map applied to the object or else it won't have tex coords.
				if(pObject->pTexVerts) 
				{
					glTexCoord2f(pObject->pTexVerts[ index2 ].x, pObject->pTexVerts[ index2 ].y);
				}
				
				// Pass in the current vertex of the object (Corner of current face)
				glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
			}
		}

	glEnd();
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// When we are done, we need to free all the model data.
	// We do this by walking through all the objects and freeing their information.

	// Go through all the objects in the scene
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		if(g_3DModel.pObject[i].pFaces)		delete [] g_3DModel.pObject[i].pFaces;
		if(g_3DModel.pObject[i].pNormals)	delete [] g_3DModel.pObject[i].pNormals;
		if(g_3DModel.pObject[i].pVerts)		delete [] g_3DModel.pObject[i].pVerts;
		if(g_3DModel.pObject[i].pTexVerts)	delete [] g_3DModel.pObject[i].pTexVerts;
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// In our render function it's pretty simple.  We just render the sphere in the
	// light's position, then turn on the shader and draw our character.

	// Give OpenGL our position,	then view,		then up vector
	gluLookAt(		0, 1.5f, 90,		0, 0.5f, 0,			0, 1, 0);

	GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack

	gluQuadricDrawStyle(pObj, GLU_FILL);				// Draw the sphere normally

	// Turn off our shader so that we can render the light not effected.
	g_Shader.TurnOff();

	// Below we push on a new matrix so that our character model isn't effect
	// by our translation of the light (sphere).  We set the color to red and
	// position the sphere to the position of the light.
	glPushMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		glTranslatef(g_LightPosition[0], g_LightPosition[1], g_LightPosition[2]);
		gluSphere(pObj, 0.05f, 25, 25);
	glPopMatrix();

	// Now that we already rendered the light we can turn shaders back on.
	g_Shader.TurnOn();
	
	// This draws our toon-shaded MD2 character
	DrawModel();

	// Free the Quadric
	gluDeleteQuadric(pObj);								

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Below we define our controls for this simple tutorial.
	// The controls are: 

	// Left Mouse Button - Changes the Render mode from normal to wire frame.
	// Left Arrow Key - Spins the model to the left
	// Right Arrow Key - Spins the model to the right
	// Escape - Quits

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		
		if(g_ViewMode == GL_TRIANGLES) {				// We our drawing mode is at triangles
			g_ViewMode = GL_LINE_STRIP;					// Go to line strips
		} else {
			g_ViewMode = GL_TRIANGLES;					// Go to triangles
		}
		break;

	case WM_KEYDOWN:									// If we pressed a key

		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;

			case VK_LEFT:								// If the LEFT arrow key was pressed
				g_RotationSpeed -= 0.05f;				// Decrease the rotation speed (eventually rotates left)
				break;

			case VK_RIGHT:								// If the RIGHT arrow key is pressed
				g_RotationSpeed += 0.05f;				// Increase the rotation speed (rotates right)
				break;
		}
		break;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

 
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
// This is a pretty cool effect for how little of work we did?  It took longer
// to write the tutorial than it was to code it :)  One thing we need to remember
// is that there must be normals passed in, otherwise the lighting won't work.
// 
// As you run this application you should see a rotating character with 4 layers
// of toon shading.  The code is pretty straight forward, but let's take a look
// at the vertex shader.
//
// * ToonShading.vert shader file explanation *

/*

varying vec3 lightDir, normal;

void main()
{
	lightDir = normalize(vec3(gl_LightSource[0].position));
	normal = gl_NormalMatrix * gl_Normal;

	gl_Position = ftransform();
} 

*/

// All we are doing here is grabbing the normalized light position
// and making sure we have the correct normal by multiplying it by
// the OpenGL normal matrix.  We could take the actual position and
// subtract the world position of the vertex from it, but we just
// treat the position as a direction.

/*

*** Create two variables to share between our vertex and fragment shader
varying vec3 lightDir, normal;

*** Start the vertex shader
void main()
{
	*** Normalize the light position and treat it as a direction
	lightDir = normalize(vec3(gl_LightSource[0].position));

	*** Make sure the normal is the true normal
	normal = gl_NormalMatrix * gl_Normal;

	*** Process the vertex like normal
	gl_Position = ftransform();
} 

*/

// 
// Let's look now at the fragment shader, ToonShading.frag:
//
// * ToonShading.frag shader file explanation *

/*

varying vec3 lightDir, normal;

void main()
{
	vec4 pixel;
	
	float intensity = dot(lightDir, normalize(normal));
	
	if (intensity > 0.85)
		pixel = gl_LightSource[0].specular;
	else if (intensity > 0.50)
		pixel = gl_LightSource[0].diffuse;
	else if (intensity > 0.25)
		pixel = gl_LightSource[0].diffuse * 2.0/3.0;
	else
		pixel = gl_LightSource[0].ambient;
	
	gl_FragColor = pixel;
} 

*/

// In the fragment shader we first calculate the intensity of light
// that is shining on each pixel.  That is done by dotting the light
// direction (or position in this case) with the current normal.
// We then test the intensity with 3 different values: 85%, 50%, 25%
// and less than 25%.  Like you can imagine, the more intensity the
// light is, the lighter the color.  The specular color is naturally
// the first shade, the diffuse second, 2/3 of diffuse 3, and finally
// the ambient color.  Keep in mind that this method is taking into
// effect the texture of color of the model.
// 
// A line-by-line explanation:
/*

*** Like in the vertex shader, add our two shared variables.
varying vec3 lightDir, normal;

*** Start the fragment shader
void main()
{
	*** This will store the pixel color for our model
	vec4 pixel;
	
	*** This calculates the brightness of the light (1 = bright, 0 = no light)
	float intensity = dot(lightDir, normalize(normal));
	
	*** Below we check the brightness value and assign the appropriate
	*** lighting to each intensity.  We use our specular, diffuse and ambient
	*** values we set in our Init() function.  For the third range we
	*** just use a decreased diffuse value.
	if (intensity > 0.85)
		pixel = gl_LightSource[0].specular;
	else if (intensity > 0.50)
		pixel = gl_LightSource[0].diffuse;
	else if (intensity > 0.25)
		pixel = gl_LightSource[0].diffuse * 2.0/3.0;
	else
		pixel = gl_LightSource[0].ambient;
	
	*** Finally, assign the changed pixel value.
	gl_FragColor = pixel;
} 

*/

// The shaders in this tutorial are pretty intuitive.  Once you go
// through them you can see exactly why it creates the toon shading.
// Your assignment is to see if you can apply the toon shading to a
// texture mapped model.  I will give you a hint, you will need to
// do the texture mapping inside of the shader, applying the light
// value to the texture map's RGB values.  Go give it a try!
//
// The Quake2 .MD2 file format is owned by ID Software.
//
// Quake and Quake2 are trademarks of ID Software.
// All trademarks used are properties of their respective owners. 
//
// ©2000-2005 GameTutorials