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
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#include "jpeglib.h"

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

#define MAX_TEXTURES 1									// This says how many texture we will be using

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This loads a jpeg file and returns it's data
tImageJPG *LoadJPG(const char *filename);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This loads a texture into OpenGL from a file (IE, "image.jpg")
void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID);

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
// In this version we included the jpeglib.h, as well as placed a prototype for:
// 
// tImageJPG *LoadJPG(const char *filename);
//
// This is our cool function that loads the jpeg, then returns it's data.  
// Here is the info for the jpeg library:
//
// * Copyright (C) 1991-1998, Thomas G. Lane.
// * The jpeglib.h and jpeg.lib files are part of the Independent JPEG Group's software.
// * For conditions of distribution and use, visit the website at: http://www.ijg.org/
//
// Note, that I added my own structure for the jpeg info, tImageJPG.  This is not
// part of the library.  I also combined the jconfig.h and jmoreconfig.h into jpeglib.h
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
