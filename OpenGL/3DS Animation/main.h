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
#include <fstream>
#include <vector>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <crtdbg.h>
using namespace std;

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This file includes all of the model structures that are needed to load
// in a .3DS file.  If you intend to do animation you will need to add on
// to this.  These structures only support the information that is needed
// to load the objects in the scene and their associative materials.

#define MAX_TEXTURES 100								// The maximum amount of textures to load

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3 
{
public:
	float x, y, z;
};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2 
{
public:
	float x, y;
};

// This is our face structure.  This is is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	BYTE  color[3];				// The color of the object (R, G, B)
	int   texureId;				// the texture ID
	float uTile;				// u tiling of texture  (Currently not used)
	float vTile;				// v tiling of texture	(Currently not used)
	float uOffset;			    // u offset of texture	(Currently not used)
	float vOffset;				// v offset of texture	(Currently not used)
} ;

// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject 
{
	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	int  positionFrames;		// The number of key frames for the position
	int  rotationFrames;		// The number of key frames for the rotations
	int  scaleFrames;			// The number of key frames for the scaling

	CVector3 vPivot;			// The object pivot point (The local axis it rotates around)

	vector<CVector3> vPosition;	// The object's current position list
	vector<CVector3> vRotation;	// The object's current rotation list	
	vector<CVector3> vScale;	// The object's current scale list

	vector<float> vRotDegree;	// The object's current rotation degree (associated with vRotation)

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	CVector3  *pVerts;			// The object's vertices
	CVector3  *pNormals;		// The object's normals
	CVector2  *pTexVerts;		// The texture's UV coordinates
	tFace *pFaces;				// The faces information of the object
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
struct t3DModel 
{
	int numOfObjects;			// The number of objects in the model
	int numOfMaterials;			// The number of materials for the model

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	int numberOfFrames;			// The number of frames of animation this model (at least 1)
	int currentFrame;			// The current frame of animation that the model is in

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	vector<t3DObject> pObject;			// The object list for our model
};


extern bool  g_bFullScreen;				// Set full screen as default
extern HWND  g_hWnd;					// This is the handle for the window
extern RECT  g_rRect;					// This holds the window dimensions
extern HDC   g_hDC;						// General HDC - (handle to device context)
extern HGLRC g_hRC;						// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;			// This holds our window hInstance

// This is our MAIN() for windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// This controls our main program loop
WPARAM MainLoop();

// This creates a texture and stores it in the texture array with it's ID.
bool CreateTexture(LPTSTR szFileName, GLuint &textureID);

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
// We added to this file some new variables for the t3DModel and t3DObject structs.
// To the model we added some variables to store the current frame and number of
// frames in the animation.  To the object structure we added 4 vectors to store
// the postion, rotation axis and scale for each frame, as well as the rotation
// degree for that axis.  We also added the amount of position, rotation and scale
// frames.  These aren't used much, except when filling in the STL vectors.
// 
// Look at 3ds.cpp or Main.cpp for a better idea of how these are used and how
// the animation is read and utilized.
// 
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
