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
#include <math.h>
#include <string>
#include <fstream>
using namespace std;


// These are the defines for the flags that are passed into glFogi()
#define GL_FOG_COORDINATE_SOURCE_EXT			0x8450
#define GL_FOG_COORDINATE_EXT					0x8451

// Here we extern our function pointer for volumetric fog positioning
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);

// These are for our multi-texture defines
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4

// These are used to increase the detail texture for our terrain
#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

// Here are the multitexture function prototypes
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);

// Here we extern our function pointers for multitexturing capabilities
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;


#define MAX_TEXTURES 1000								// The maximum amount of textures to load

extern UINT g_Texture[MAX_TEXTURES];					// This stores the texture IDs
extern float g_DT;										// Global float that stores the elapsed time between the current and last frame

#define REFLECTION_ID	0
#define REFRACTION_ID	1
#define NORMAL_ID		2
#define DUDVMAP_ID		3
#define DEPTH_ID		4
#define TERRAIN_ID		5
#define DETAIL_ID		6
#define BACK_ID		11		// The texture ID for the back side of the cube
#define FRONT_ID	12		// The texture ID for the front side of the cube
#define BOTTOM_ID	13		// The texture ID for the bottom side of the cube
#define TOP_ID		14		// The texture ID for the top side of the cube
#define LEFT_ID		15		// The texture ID for the left side of the cube
#define RIGHT_ID	16		// The texture ID for the right side of the cube

#define NUM_WATER_TEX 32
#define WATER_START_ID 17
#define WATER_END_ID WATER_START_ID + NUM_WATER_TEX

// We need to define this for glTexParameteri()
#define GL_CLAMP_TO_EDGE	0x812F						// This is for our skybox textures


#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

extern bool  g_bFullScreen;								// Set full screen as default
extern HWND  g_hWnd;									// This is the handle for the window
extern RECT  g_rRect;									// This holds the window dimensions
extern HDC   g_hDC;										// General HDC - (handle to device context)
extern HGLRC g_hRC;										// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;							// This holds our window hInstance

extern float g_WaterHeight;
extern float g_WaterFlow;
extern float g_WaterUV;

// This is our basic 3D point/vector class
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

	// Here we overload the + operator so we can add vectors together 
	CVector3 operator+(CVector3 vVector)
	{
		// Return the added vectors result.
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator-(CVector3 vVector)
	{
		// Return the subtracted vectors result
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the * operator so we can multiply by scalars
	CVector3 operator*(float num)
	{
		// Return the scaled vector
		return CVector3(x * num, y * num, z * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	CVector3 operator/(float num)
	{
		// Return the scale vector
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};


// Include our camera class and functions
#include "camera.h"

// Now we can extern the camera object
extern CCamera g_Camera;

// Include our header file for our Terrain.cpp
#include "Terrain.h"									
extern BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Include our shader header file
#include "CShader.h"

// Extern our global shader object so the water.cpp file can use it
extern CShader g_Shader;

// Include our water functions
#include "Water.h"

// This renders all of the world or just the top of it
void RenderWorld(bool bRenderCaustics);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This creates a texture map from a file name and a reference to a UINT to store it
bool CreateTexture(UINT &texture, LPSTR strFileName);

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
// We added the CShader.h and Water.h to this file, as well as a global shader
// object.  RenderWorld() was also added to this file.
//  
//
// ©2000-2005 GameTutorials