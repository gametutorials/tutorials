#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

#define SCREEN_WIDTH 640								// We want our screen width 640 pixels
#define SCREEN_HEIGHT 480								// We want our screen height 480 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// We only need 4 textures for this tutorial
#define MAX_TEXTURES 4									

// Here we choose a high number that says we can have up to 500 vertices for our triangles
#define MAX_VERTICES 500

// We created a very simple vector class to handle our vertices
class CVector3
{
public:

	// A default constructor
	CVector3() {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Our x y z data
	float x, y, z;
};

// We also created a simple vector class for our texture coordinates
class CVector2
{
public:

	// The default constructor
	CVector2() {}

	// This allows us to create and set a CVector2 like this:  CVector2 vUV = CVector2(0, 0, 0);
	CVector2(float X, float Y) 
	{ 
		x = X; y = Y;
	}

	// The U V data
	float x, y;
};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


extern bool  g_bFullScreen;								// Set full screen as default
extern HWND  g_hWnd;									// This is the handle for the window
extern RECT  g_rRect;									// This holds the window dimensions
extern HDC   g_hDC;										// General HDC - (handle to device context)
extern HGLRC g_hRC;										// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;							// This holds our window hInstance


extern UINT g_Texture[MAX_TEXTURES];					// This is our texture data array


// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This loads a texture into openGL from a file (IE, "bitmap.bmp")
bool CreateTexture(GLuint &textureID, LPTSTR szFileName);

// This changes the screen to full screen mode
void ChangeToFullScreen();

// This is our own function that makes creating a window modular and easy
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// This allows us to configure our window for OpenGL and backbuffering
bool bSetupPixelFormat(HDC hdc);

// This inits our screen translations and projections
void SizeOpenGLScreen(int width, int height);

// This sets up OpenGL
void InitializeOpenGL(int width, int height);

// This initializes the whole program
void Init(HWND hWnd);

// This draws everything to the screen
void RenderScene();

// This frees all our memory in our program
void DeInit();


#endif 


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this version we added 2 new classes and a define.  We created a define
// MAX_VERTICES for our vertices that make up the triangles.  We also changed the
// amount of textures we were using with the MAX_TEXTURES define to 4.
//
// Lastly, we added 2 simple vector classes that are used for 3D points (vertices) 
// and 2D points (texture coordinates, (U,V)).
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
