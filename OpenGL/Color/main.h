#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

// *EXTERNING VARIABLES*

// This might be new to some, so I will explain what an "extern" does.
// By putting an "extern" in front of a variable declaration,
// we don't actually create the variable here.  It's like a function prototype.
// It allows other .cpp files to reference these variables.  This is how
// you work with global variables.  You have all your global variables as "externs"
// In a header file, and then actually defined in a .cpp file.  You will notice
// they are created in main.cpp.  This is not a redefinition.  If you didn't actually
// create them in main.cpp it would compile, but when it got to the linker it would
// say they were all unresolved externals.  So if you ever have an extern you will always
// need to define them for real in a .cpp file.  We use externs so we don't get redefinition
// errors.  Eventually, we might have a variables.h and a variables.cpp that just has variables.
// One thing, you CAN NOT set an extern variable to a value. Like: extern bFullScreen = TRUE;
// This is because you aren't creating the variable, just defining the type and name.

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
// Now we have all of our header information in one central place.  As we add to our
// OpenGL project we will create more header files, but for right now every source
// file should include this file.  
//
// We learned about externing up above.  Make sure you understand that it is standard
// for global variables that are used throughout many source files (.cpp files).
// We don't want to define and create the variables in the .h file because of
// redefinition errors, so we use externs that don't create the variable but register
// it with the compiler, so when it links, everything works out.  Now all our source
// files that include this header file can use those variables.  Otherwise they would
// get undefined errors.
//
//
// © 2000-2005 GameTutorials
