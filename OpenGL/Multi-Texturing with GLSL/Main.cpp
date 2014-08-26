//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersTexturing								 //
//																		 //
//		$Description:	Using GLSL we multitexture a quad and animate it //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

UINT g_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL.  
														// UINT is  an unsigned int (only positive numbers).

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// In this tutorial we use GLSL to multitexture a quad, then animate the fog
// texture over a background image.  The shaders are pretty simple to accomplish
// this result, but we will go over and explain each line.  We still need to load 
// the ARB extensions for multitexturing.  It is assumed that you are familiar
// with these extensions and have done basic multitexturing.  If not, please
// refer to our multitexturing tutorial in OpenGL, as we will not go over these
// functions in detail.
//
// The only thing we add in this tutorial in the way of code is the loading of the 
// extensions and drawing a quad.  The rest of the code is the standard shader
// base code that is added upon from previous shader tutorials.  You will only
// need to look in Main.cpp.  Like usual, the shaders are explained in detail
// at the bottom of this page.
//
//

// Like usual, include our global shader object.
CShader g_Shader;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// These are the 2 function pointer defines for our 2 multitexturing functions.
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum layer, GLfloat u, GLfloat v);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum layer);

// These are the needed defines for the multitexturing.  Each one is a different texture layer.
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2

// Using our defines up above we create a more variable of each function pointer type.
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		= NULL;


///////////////////////////////// INIT WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the application data.
/////
///////////////////////////////// INIT WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Here we load the functions from wglGetProcAddress and store them in our function pointers.
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	// Let's make sure our multitexturing functions were loaded correctly and supported.
	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your version of OpenGL does not support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// There will be 2 textures loaded to demonstrate the effect.  We use our own 
	// CreateTexture to load the bitmaps and save them in OpenGL's texture memory.
	CreateTexture(g_Texture[0], "Background.bmp");			// Load the background picture into our first texture index
	CreateTexture(g_Texture[1], "Fog.bmp");					// Load the fog into our second texture index


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we inititialize our shader code and extensions
	InitGLSL();

	// Load the 2 shaders for multitexturing
	g_Shader.InitShaders("MultiTexturing.vert", "MultiTexturing.frag");

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
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 3,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Okay, let's go over what is necessary to do multitexturing with GLSL.
	// First, like normally, we set our active textures, bind the texture to
	// the current active texture, then enable texture mapping with glEnable().
	// Next, we need to tell our shader which texture unit is which.  We do this
	// by setting the "texture1" and "texture2" variables in the shader to the
	// correct texture number.  GL_TEXTURE0_ARB is unit 0, GL_TEXTURE1_ARB is 1,
	// and so on.  We just set the integer value to its respective texture unit.
	// Also, just like normal, we use the glMultiTexCoord2fARB() to assign the
	// UV coordinates.  We add a animated "wrap" variable to move the fog along
	// the background that is passed in to our shader.  The wrap is added to the
	// UV coordinates in the shader.
	//
	// So what is the benefit with using shaders for multitexturing if it's the
	// same code, along with more code added for shaders?  Good question!  If this
	// is all we were doing there would be no reason to use shaders.  When it comes
	// to special programming like color keying, special blending and lighting,
	// it's faster to use shaders.  Just like in 2D, you can do a check for every
	// pixel to see if it's (0, 0, 0), black, and then "discard" it, which would
	// render an only the colors not black.  This can be used for rendering plants,
	// decals and special billboards onto other textures.  This method eliminates
	// the need to render a see through texture twice with a black and white mask.

	// Activate the first texture ID and bind the background texture to it
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);
	glEnable(GL_TEXTURE_2D);

	// Activate the second texture ID and bind the fog texture to it
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D,  g_Texture[1]);
	glEnable(GL_TEXTURE_2D);

	// Here pass in our texture unit 0 (GL_TEXTURE0_ARB) for "texture1" in the shader.
	g_Shader.SetInt(g_Shader.GetVariable("texture1"), 0);

	// Here pass in our texture unit 1 (GL_TEXTURE1_ARB) for "texture2" in the shader.
	g_Shader.SetInt(g_Shader.GetVariable("texture2"), 1);

	// Like our "time" variable in the first shader tutorial, we pass in a continually
	// increasing float to create the animated wrapping effect of the fog.
	static float wrap = 0.0f;
	g_Shader.SetFloat(g_Shader.GetVariable("wrap"), wrap);
	wrap += 0.002f;

	// Display a multitextured quad texture to the screen
	glBegin(GL_QUADS);

		// Display the top left vertice with each texture's texture coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
		glVertex3f(-1, 1, 0);

		// Display the bottom left vertice with each texture's coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
		glVertex3f(-1, -1, 0);

		// Display the bottom right vertice with each texture's coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
		glVertex3f(1, -1, 0);

		// Display the top right vertice with each texture's coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
		glVertex3f(1, 1, 0);

	// Stop drawing QUADS
	glEnd();											

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Swap the back buffers to the foreground
	SwapBuffers(g_hDC);
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
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

	case WM_KEYDOWN:
		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is being closed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = (LONG)DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// If you run this program you will notice a quad with 2 textures, one of which
// is blended and wrapping over the background texture.  This lesson was used to
// teach the basics of texturing with GLSL, not to show a more optimized way of
// doing multitexturing.  You will want to build upon this tutorial to do fast
// methods for things like lighting, blending and color keying.
//
// Now let's go through the vertex shader line-by-line and explain the code
// we used.
//
// * MultiTexturing.vert shader file explanation *

/*

uniform float wrap;

void main()
{		
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 textureCoords = gl_MultiTexCoord0;
	textureCoords.x -= wrap;

	gl_TexCoord[1] = textureCoords;
	
	gl_Position = ftransform();
}  

*/

// See how the wrapping works in the shader?  We first set the first texture
// unit's UV coordinates, then store the second texture unit's coordinates
// and decrease the current wrapping value from the U coordinate.  Finally,
// we set the second texture unit's UV coordinates with the applied wrap.
//
// Let's explain every line now:
//

/*

*** Create a variable to store the current wrap
uniform float wrap;

*** Start the vertex shader program
void main()
{		
	*** This sets the shader's texture coordinates from glMultiTexCoord2fARB(GL_TEXTURE0_ARB, ..., ...);
	gl_TexCoord[0] = gl_MultiTexCoord0;

	*** This stores the shader's texture coordinates from glMultiTexCoord2fARB(GL_TEXTURE1_ARB, ..., ...);
	vec4 textureCoords = gl_MultiTexCoord0;
	
	*** gl_TexCoord[] and gl_MultiTexCoord* are vec4 variables.  The x and y 
	*** are the u and v coordinates.  You can also say textureCoords[0] instead
	*** of textureCoords.x.  textureCoords[2] and up do nothing, just 0 and 1 (U, V).
	textureCoords.x -= wrap;

	*** Now that we applied our wrap to the UV coordinates, assign the second
	*** texture unit's new UV coordinates.
	gl_TexCoord[1] = textureCoords;
	
	*** Apply the normal matrices to our current vertex.
	gl_Position = ftransform();
}  

*/

// 
// Let's look now at the fragment shader:
//
// * MultiTexturing.frag shader file explanation *

/*

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	gl_FragColor = texture2D(texture1, gl_TexCoord[0].st) * texture2D(texture2, gl_TexCoord[1].st);
}

*/

// The fragment shader is pretty basic for just normal multitexturing.
// Notice that we need two uniform variables for each texture unit at the
// top.  Those are set with our SetInt() function in our shader class.
// They correspond the to GL_TEXTURE*_ARB's numbers (0, 1, etc.).
// To get the color and alpha value for a texture coordinate we use the
// texture2D() function, which takes a texture unit (i.e. texture1) and
// a vec2 UV coordinate (i.e. gl_TexCoord[0].st).  The S and T are the same
// as the U and V.  You will notice that OpenGL uses S and T when dealing
// with things like sphere mapping and texture coordinates higher than 2D.
//
// texture2D() will return a vec4 value that stores the RGBA values.  Notice
// that to do the basic multitexture blending we just multiply the first
// texture pixel by the second texture pixel.  If you add them instead it
// will turn the image really bright.  The result is the current texture
// pixel and is assigned to our current fragment pixel on the screen.

/*

*** Create two variables that we set our texture units to (0 and 1).
uniform sampler2D texture1;
uniform sampler2D texture2;

*** The start of the fragment shader program
void main()
{
	*** To do the multitexture blending we just multiply the first texture's pixel by
	*** the second texture's pixel, which is then stored in our current program's pixel.
	*** Notice that gl_TexCoord[0] points to GL_TEXTURE0_ARB and gl_TexCoord[1] is GL_TEXTURE1_ARB.
	gl_FragColor = texture2D(texture1, gl_TexCoord[0].st) * texture2D(texture2, gl_TexCoord[1].st);
}

*/

// Not much to blending textures is there?  If you want to change different blending
// methods you can look in the OpenGL red book for the equations for each blending
// mode, or look in MSDN online at Microsoft's side: http://msdn.microsoft.com/
//
// Real fast, before we close let's go over some basic syntax for working with
// textures.  Below is an example of extracting the RGB and Alpha value from a
// current texture's pixel:
//
// vec4 pixel = texture2D(tex,gl_TexCoord[0].st);
// vec3 pixelRGB = pixel.rgb;
// float pixelAlpha = pixel.a;
//
// There you go, this is a way to extract the different data from a vec4 pixel.
// Can you see how easy color keying and transparency would be?  The next tutorial
// will show you how to do transparency using shaders.  Go check it out!
//
//
// © 2000-2005 GameTutorials