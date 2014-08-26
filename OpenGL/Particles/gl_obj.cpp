#include <assert.h>
#include "gl_obj.h"

// This sets up "OpenGL" for us -- If an error occurs the method returns false
// otherwise the method returns true
bool CGLObj::init(HDC win_hdc, GLint colorBits, GLint depthBits)
{
	// Error check
	if(!win_hdc)
		return false;

	// DO NOT allow double initialization
	if(hglrc)
		return false;
		
	hdc = win_hdc; // Save off the window's HDC

	// Set the pixel format
	if(!setPixelFormat(colorBits, depthBits))
		return false;

	// Set the render context
	if(!setRenderContext())
		return false;

	// Enable flags we want
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	
	// Okay here's some functions that makes the particle magic happens
	// First we have to enable blending, without that well the particles couldn't blend!
	glEnable(GL_BLEND);
	
	// Next we need to set how we want the particles to blend.  In general blending
	// occurs like this:
	// Red = min(1.0f, (SrcBlendRed * SrcRed) + (DestBlendRed * DestRed)) 
	// Green = min(1.0f, (SrcBlendGreen * SrcGreen) + (DestBlendGreen * DestGreen))
	// Blue = min(1.0f, (SrcBlendBlue * SrcBlue) + (DestBlendBlue * DestBlue)) 
	// Alpha = min(1.0f, (SrcBlendAlpha * SrcAlpha) + (DestBlendAlpha * DestAlpha))
	// We chose the following blend flags, which are defined as follows:
	// SrcBlend = GL_SRC_COLOR = (SrcRed,SrcGreen,SrcBlue,SrcAlpha) 
	// DestBlend = GL_ONE = (1,1,1,1)
	// This makes our final color equations used the following:
	// Red = min(1.0f, (SrcRed * SrcRed) + DestRed) 
	// Green = min(1.0f, (SrcGreen * SrcGreen) + DestGreen)
	// Blue = min(1.0f, (SrcBlue * SrcBlue) + DestBlue) 
	// Alpha = min(1.0f, (SrcAlpha * SrcAlpha) + DestAlpha)
	// Thus when we draw a particle over a spot on the screen that already has color, it
	// will make that particle a little brighter.  The more particles that overlap, the
	// brighter the color they will be.  That is why the point where are particles emit
	// from looks hot white.
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
		return true; // OpenGL is up and running :)
}

// Sets the view port
bool CGLObj::setViewPort(GLint wid, GLint hgt, GLdouble fov)
{
	// Make sure we've been passed in a valid view port width and height
	if(wid <= 0)
		return false;

	if(hgt <= 0)
		return false;

	glViewport(0,0,wid,hgt); // Set the view port

	glMatrixMode(GL_PROJECTION); // Set projection matrix to the identity matrix
	glLoadIdentity();

	// fov == Field of view in degrees
	// (GLdouble)vp_wid/(GLdouble)height == Aspect Ratio
	// 1.0f == Front clipping plane
	// 8192.0f == Back clipping plane
	// Set the perspective we view the app with
	gluPerspective(fov,(GLdouble)wid/(GLdouble)hgt,1.0f,8192.0f); 

	glMatrixMode(GL_MODELVIEW); // Set the model view matrix to the identity matrix
	glLoadIdentity();
		return true; // Success

}

// This is our "standard" OpenGL "begin to draw" code 
void CGLObj::begin()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// (0,0,5) == The eye (ie where are we looking from)
	// (0,0,0) == The reference point (ie what point are we looking at)
	// (0,1,0) == The world's up vector
	gluLookAt(0,0,5, 0,0,0, 0,1,0);
}

// This is our "standard" OpenGL "blit to the screen, we've drawn everything" code
void CGLObj::end() 
{ 
	glFlush();
	SwapBuffers(hdc);
	assert(glGetError() == GL_NO_ERROR); // Make sure everytime we complete a drawing
									    // sequence, that we did not encounter any errors
}

// Free memory
CGLObj::~CGLObj()
{
	if(hglrc)
	{
		wglMakeCurrent(NULL,NULL); // Set render context back
		wglDeleteContext(hglrc); // Delete render context
		hglrc = NULL;
	}
	
	hdc = NULL;
}


/*************** Private class functions *******************/

// Sets pixel format
bool CGLObj::setPixelFormat(GLint colorBits, GLint depthBits)
{
	// Declare a pixel format descriptor
	PIXELFORMATDESCRIPTOR pfd = {0};

	// Init the fields we care about
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Always equals the size of the structure
    pfd.nVersion = 1; // Always equals 1
	
	// Set the appropriate OpenGL flags
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.iPixelType = PFD_TYPE_RGBA; // 32-Bit pixel type
    pfd.cColorBits = colorBits; // Bits per pixel
    pfd.cDepthBits = depthBits; // Bits per pixel

	// Choose a compatible pixel format (storing the pixel format index in "result")
	int result = ChoosePixelFormat(hdc,&pfd);

		// Error Check
		if(result == 0)
			return false;

	// Attempt to set the pixel format
	if(!SetPixelFormat(hdc,result,&pfd))
		return false;

	return true; // Success
}

// Creates render context and makes it current
bool CGLObj::setRenderContext()
{
	// Create the OpenGL render context
	hglrc = wglCreateContext(hdc);

		// Error Check
		if(hglrc == NULL)
			return false;

	// Make it the current render context
	if(!wglMakeCurrent(hdc,hglrc))
		return false;

	return true; // Success

}

