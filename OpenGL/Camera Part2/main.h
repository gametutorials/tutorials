#ifndef _MAIN_H
#define _MAIN_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

// Here is our vector class
class CVector3 {										// Remember, we use a C in front to show it's a class

public:													// We want this data public so we don't have to use data access functions.  
	float x, y, z;										// We just want a float for a X Y and Z.
};

// This is our camera class
class CCamera {

public:

	CVector3 m_vPosition;									// The camera's position
	CVector3 m_vView;										// The camera's View
	CVector3 m_vUpVector;									// The camera's UpVector

	// Our camera constructor
	CCamera();	
	
	// This changes the position, view, and up vector of the camera.
	// This is primarily used for initialization.
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This rotates the camera's view around the position depending on the values passed in.
	// The rotation is done using axis-angle rotation. That means you give this function
	// a degree (in radians) and a axis rotate around.  
	void RotateView(float angle, float x, float y, float z);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This will move the camera forward or backward depending on the speed
	void MoveCamera(float speed);
};


extern bool  g_bFullScreen;									// Set full screen as default
extern HWND  g_hWnd;										// This is the handle for the window
extern RECT  g_rRect;										// This holds the window dimensions
extern HDC   g_hDC;											// General HDC - (handle to device context)
extern HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;								// This holds our window hInstance

// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

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
// In this tutorial all we added to this file was the RotateView() function to
// our CCamera class.  This function will allow us to rotate the camera's view. 
// 
//
// ©2000-2005 GameTutorials
