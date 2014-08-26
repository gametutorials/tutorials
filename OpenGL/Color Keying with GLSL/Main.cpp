//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		ShadersColorKeying								 //
//																		 //
//		$Description:	Using GLSL we create transparencies in textures  //
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

// From our previous tutorial on multitexturing with GLSL, you should be more
// familiar with handling textures in a shader.  Now we are ready to start using
// shaders and texturing to do some cool effects.  In this shader tutorial we
// will go over creating transparencies, or see-through textures.  This is also
// called "color keying".  This means that you pick a color in your picture that
// you want to be ignored when rendering, and OpenGL will then only draw the
// colors that are not the color key.  
//
// In our example, we have 2 textures to choose from.  The default texture is 
// "Tree.bmp", which has a black background.  The background is exactly (0, 0, 0).  
// When we draw it we don't render any of the pixels that are black, creating a 
// transparent tree.  The other file called "Picture.bmp" has some pink starts 
// in it with a color key of (1, 0, 1).  You can change Tree.bmp in the code the 
// Picture.bmp to see this in action.  Remember to also change the color to 
// (1, 0, 1) as well.
//
// The color keying lesson is built off of the ShadersTexturing tutorial.
// This tutorial is pretty simple so there won't be too much explanation here.
// Once you see the code it will be pretty much self-explanatory.  Let's go!
//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// Like usual, include our global shader object.
CShader g_Shader;

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
	CreateTexture("Tree.bmp", g_Texture[0]);		// Load the background picture into our first texture index

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we inititialize our shader code and extensions
	InitGLSL();

	// Load the 2 shaders for multitexturing
	g_Shader.InitShaders("ColorKeying.vert", "ColorKeying.frag");

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

////////////////////////////// DRAW QUAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a simple quad and sets the color key
/////
///////////////////////////// DRAW QUAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawQuad()
{
	// In this function we set our current active texture, give our color
	// and texture data to the shader, then draw the textured quad.

	// Activate the first texture ID and bind our texture to it
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);
	glEnable(GL_TEXTURE_2D);

	// *** This is important! ***
	// Because we are using gluBuild2DMipmaps() to create our textures
	// in Init.cpp, we need to set our texture parameters to the following.
	// If we don't mipmaps will be created so that it blurs our texture and
	// the color keying gets screwed up and the borders look really bad.
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Here pass in our texture unit 0 (GL_TEXTURE0_ARB) for "texture1" in the shader.
	g_Shader.SetInt(g_Shader.GetVariable("texture1"), 0);

	// Now we actually get to set the color key!  For the tree texture we
	// use pure black as the color key, where the Picture.bmp uses (1, 0, 1)
	// if you want to test that texture as well.

	// Here we set the transparent color to be discarded - BLACK (0, 0, 0).
	g_Shader.SetFloat3(g_Shader.GetVariable("ColorKey"), 0.0f, 0.0f, 0.0f);

	// Display a textured quad to the screen
	glBegin(GL_QUADS);

		// Display the top left vertice with each texture's texture coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
		glVertex3f(-1, 1, 0);

		// Display the bottom left vertice with each texture's coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
		glVertex3f(-1, -1, 0);

		// Display the bottom right vertice with each texture's coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
		glVertex3f(1, -1, 0);

		// Display the top right vertice with each texture's coordinates
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
		glVertex3f(1, 1, 0);

	// Stop drawing QUADS
	glEnd();
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	gluLookAt(0, 0, 4,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// For our rendering, what we do is display two quads that are NOT color keyed,
	// to show the difference.  Then we display our middle quad, which is the
	// transparent one.  We only turn on the shaders when we draw the last quad.

	// Push on a new matrix so the other quads aren't effect by the translation.
	// Below we just translate the quad to the left and draw it.
	glPushMatrix();
		glTranslatef(1.0f, 0.0f, 0.0f);
		DrawQuad();
	glPopMatrix();

	// Push on a new matrix so the other quad isn't effect by the translation.
	// Below we just translate the quad to the right and draw it.
	glPushMatrix();
		glTranslatef(-1.0f, 0.0f, 0.0f);
		DrawQuad();
	glPopMatrix();

	// To make the tree rotate we just create a static float and increase it.
	static float rotY = 0;
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);
	rotY += 0.9f;

	// Now we turn on the shaders, then draw the transparent quad, then turn off shaders.
	g_Shader.TurnOn();
		DrawQuad();
	g_Shader.TurnOff();

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
// I bet you've been waiting for this tutorial huh? :)  Finally, we have an
// example of creating transparent bitmaps.  The cool thing is that you don't
// have to have a nice image editor to create 32-bit alpha images if you
// don't want.  If it's a pain to use colors then you can always just do a
// test for the alpha value to see if you should render that pixel.  Remember
// that alpha is the fourth index into the vec4 array returned from the texture2D()
// function.
//
// * Warning 1 *
//
// You need to make sure that when you do color keying that you do not create mipmaps
// that will change and blur your image.  I suggest using the following texture
// parameters when using this code:
//
// 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
// I didn't want to change the CreateTexture() function to not distract you with
// more code to look at, so I just changed the texture parameters to the above
// details when using a texture that needs transparency.
//
// * Warning 2 *
//
// Like I have many times, you might have art that you will try to color key and
// you can still see the edges of the color key, which looks ugly of course.  This
// is because of the joy of art programs using anti-aliasing.  This means the art
// program, like Photoshop for example, blends surrounding color values so it looks
// smoother, especially for round edges.  If you run into this problem you will need
// to make sure to turn off anti-aliasing in your art program.  In Photoshop you can
// usually click a check box for it.  In the picture I created I made some custom
// star shapes and even though there was no button to turn of anti-aliasing, Photoshop
// created anti-aliasing anyway.  To solve this I control-clicked the layer, selecting
// the stars, then I made the selection larger by choosing Select->Modify->Expand.  I
// did about 3 pixels, then I chose the paint bucket, un-clicked the anti-aliasing box
// and did a really high tolerance so that it filled in the stars over the blended edges.
// There are many other ways to do this, but this is just an example of what you might
// have to do sometimes.
//
// Sometimes no matter what you do you will need to not just use a single color value
// but a small range to color key.  After we go over the shaders I will supply the
// fragment code to do that at the bottom of this page.
//
// Now let's go through the vertex shader line-by-line and explain the code
// we used.
//
// * ColorKeying.vert shader file explanation *

/*

void main()
{		
	gl_TexCoord[0] = gl_MultiTexCoord0;	
	gl_Position = ftransform();
} 

*/

// Just like the previous tutorial on texturing with shaders, we set the
// UV coordinates in the first line, then do a normal ftransform() to project
// the current vertice by the appropriate OpenGL matrices.
//
// Let's explain every line now:
//

/*

*** Start the vertex shader program
void main()
{		
	*** Here we set the UV coordinates so our fragment shader can access it.
	gl_TexCoord[0] = gl_MultiTexCoord0;	

	*** Multiply the current matrices by the current vertex like normal.
	gl_Position = ftransform();
} 

*/
 
// Let's look now at the fragment shader:
//
// * ColorKeying.frag shader file explanation *

/*

uniform sampler2D texture1;
uniform vec3 ColorKey;

void main()
{
	vec4 pixel = texture2D(texture1, gl_TexCoord[0].st);

	if(pixel[0] == ColorKey[0] && pixel[1] == ColorKey[1] && pixel[2] == ColorKey[2])
	{
		discard;
	}

	gl_FragColor = pixel;
}

*/

// Nearly all the functions and syntax in this shader should look familiar.  Notice 
// that the logical operations like && and == are the same as C/C++.  The only thing 
// that should be new to you is the keyword "discard".  Like you can imagine, this
// keyword discards the current pixel and returns from the fragment shader.  That is
// what makes our texture see through.  If the color of the current pixel in the
// texture matches the color key then we don't draw anything and move on to the next
// pixel.  If you have ever done transparency in 2D, it's exactly the same thing here.
//
// Our Line-by-line explanation:
//

/*
*** This variable stores our texture unit (0 in this case)
uniform sampler2D texture1;

*** So that we can change our color key in code, this variable stores the 
*** RGBA value for the color key.
uniform vec3 ColorKey;

*** Start our fragment shader program
void main()
{
	*** This line extracts the current pixel in our texture from the current UV's.
	vec4 pixel = texture2D(texture1, gl_TexCoord[0].st);

	*** This is where the color keying happens.  Like in 2D, we just check to see
	*** if the current pixel has the same RGB value as our color key.  If so, we
	*** then want to discard this pixel and NOT draw it to the screen.  Simple huh?
	if(pixel[0] == ColorKey[0] && pixel[1] == ColorKey[1] && pixel[2] == ColorKey[2])
	{
		discard;
	}

	*** If we get here that means we did not find a match, so draw the current pixel.
	gl_FragColor = pixel;
}

*/

// That is pretty basic right?  Sometimes your image has blurred colors and you can't
// always get a perfect color key.  In this case you will want to use a range of colors
// to check against, like the following:

/*
	float epsilon = 0.05;
	if(pixel[0] >= (Color[0] - epsilon) && pixel[0] <= (Color[0] + epsilon) &&
	   pixel[1] >= (Color[1] - epsilon) && pixel[1] <= (Color[1] + epsilon) &&
	   pixel[0] >= (Color[2] - epsilon) && pixel[2] <= (Color[2] + epsilon))
	{
		discard;
	}
*/

// As you can see, we create an epsilon value to give us some room to match the
// pixels.  Other than that, that is the basics of color keying.  Just for fun,
// try creating a texture and see for yourself.  Then try to program an alpha
// channel key.
//
//
// © 2000-2005 GameTutorials