#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <math.h>

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

extern bool  g_bFullScreen;								// Set full screen as default
extern HWND  g_hWnd;									// This is the handle for the window
extern RECT  g_rRect;									// This holds the window dimensions
extern HDC   g_hDC;										// General HDC - (handle to device context)
extern HGLRC g_hRC;										// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;							// This holds our window hInstance

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#define PI 3.14159										// This is our define for PI

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



// This is our basic vector class
struct CVector3
{
public:
	
	// A default constructor
	CVector3() {}

	// This is our constructor that allows us to initialize our data upon creating an instance
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// The x, y and z values of this vector
	float x, y, z;						
};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is our basic quaternion class
class CQuaternion 
{

public:

	// This is our default constructor, which initializes everything to an identity
	// quaternion.  An identity quaternion has x, y, z as 0 and w as 1.
	CQuaternion() { x = y = z = 0.0f;   w = 1.0f;  }
	
	// Create a constructor that will allow us to initialize the quaternion when creating it
	CQuaternion(float X, float Y, float Z, float W) 
	{ 
		x = X;		y = Y;		z = Z;		w = W;
	}

	// This converts and axis angle rotation to a quaternion
	void CreateFromAxisAngle(float X, float Y, float Z, float degree);

	// This takes in an array of 16 floats to fill in a 4x4 homogeneous matrix from a quaternion
	void CreateMatrix(float *pMatrix);

private:

	// This stores the 4D values for the quaternion
	float x, y, z, w;
};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
// We added our basic quaternion class and PI to this header file.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//

