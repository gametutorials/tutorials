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

extern bool  g_bFullScreen;								// Set full screen as default
extern HWND  g_hWnd;									// This is the handle for the window
extern RECT  g_rRect;									// This holds the window dimensions
extern HDC   g_hDC;										// General HDC - (handle to device context)
extern HGLRC g_hRC;										// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;							// This holds our window hInstance

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
// This tutorial creates a bezier curve and allows a sphere to move along it
// with the LEFT and RIGHT arrow keys.  You will also note that we display
// 2 of the curve control points to get a better visualization of what is going on.
// 
// Here is the equation we are using:
//
// B(t) = P1 * ( 1 - t )^3 + P2 * 3 * t * ( 1 - t )^2 + P3 * 3 * t^2 * ( 1 - t ) + P4 * t^3 
//
// This is pretty straight forward, even though it looks complicated. If you got 
// up to Trig, you will notice that this is a polynomial.  That is what a curve is.
// "t" is the time from 0 to 1.  You could also think of it as the distance along the
// curve, because that is really what it is.  P1 - P4 are the 4 control points.
// They each have an (x, y, z) associated with them.  P1 is the starting point. P2 is
// the first control point, P3 is the second control point, and P4 is the end point of the
// curve.  You will hear that this is a 4 control point equation. I usually think of it
// as 2 control points because the 2 end points are just the start and end of the curve.
// 
// Bezier curves are awesome.  You can use them to make realistic camera movements.
// Tunnel effects most frequently use bezier curves to shape the tunnel.  Once you
// understand one single bezier curve, you can then branch off and learn how to create
// curved surfaces.  This will also prepare you to learn about NURBS and SPLINES.
// 
// I would like to thank Paul Bourke at http://astronomy.swin.edu.au/pbourke/curves/bezier/
// 
// His web site helped TREMENDOUSLY with understanding Bezier Curves, as well as his source code.  
// I suggest checking it out because it has a HUGE range of other graphics programming topics.
// 
// Once again, go to http://astronomy.swin.edu.au/pbourke/curves/bezier/ to get a more clear
// understanding of the math behind Bezier Curves as well as excellent visual examples.
//
// Good Luck!
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
