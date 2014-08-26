#ifndef _MAIN_H
#define _MAIN_H

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#pragma warning(disable: 4786)							// This disables vector<> string warnings

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <crtdbg.h>
using namespace std;

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

#define MAX_TEXTURES 100								// The maximum amount of textures to load



// This is our 4D point class for quaternions and such
class CVector4 
{
public:
	float x, y, z, w;
};

// This is our 3D point class.  This will be used to store the vertices of our model.
class CVector3 
{
public:
	float x, y, z;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Create a constructor to initialize our variables
	CVector3() { x = 0;  y = 0;  z = 0; }
	
	// Create a constructor that allows us to initialize the data ourselves
	CVector3(float x2, float y2, float z2)
	{
		x = x2;  y = y2;  z = z2;
	}

	// This is our operator overloaded addition against a CVector3
	CVector3 operator+(const CVector3& v) const
	{
		return CVector3(x + v.x, y + v.y, z + v.z);
	}

	// This is our operator overloaded addition/assignment for a CVector3
	CVector3& operator+=(const CVector3& v)
	{
		// Add the vector to our current vector
		x += v.x;
		y += v.y;
		z += v.z;

		// Return the changed vector back
		return *this;
	}

	// This is our operator overloaded multiplication against a scalar
	CVector3 operator*(float scalar)
	{
		return CVector3(x * scalar, y * scalar, z * scalar);
	}

	// The dot product between two CVector3 vectors
	float Dot(const CVector3& v) const
	{
		return ( (x * v.x) + (y * v.y) + (z * v.z) );
	}

	// The cross product between two CVector3 vectors
	CVector3 Cross(CVector3 vVector2)
	{
		CVector3 vNormal;	

		// Calculate the cross product with the non-commutative equation
		vNormal.x = ((y * vVector2.z) - (z * vVector2.y));
		vNormal.y = ((z * vVector2.x) - (x * vVector2.z));
		vNormal.z = ((x * vVector2.y) - (y * vVector2.x));

		// Return the cross product
		return vNormal;										 
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


};

// This is our 2D point class.  This will be used to store the UV coordinates.
class CVector2 
{
public:
	float x, y;
};


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This will hold a list of weights for every single weight influence.
// We have a CVector2 that stores the bone index and weight for that index.
struct tWeightInfo
{
	int numBlendLinks;			// The number of weight influences
	CVector2	*pWeightInfo;	// The list of weights for the current weight influence
};

// This holds a translation and quaternion rotation for our animation
struct tBoneMatrix
{
	CVector3 vTranslation;		// The translation vector
	CVector4 qRotation;			// The quaternion rotation vector
};

// This stores the number of bones and the translation and rotation list
struct tBoneInfo
{
	int numBones;				// The number of bones
	tBoneMatrix *pBoneMatrices;	// The list of translations and rotations
};

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////



// This is our face structure
struct tFace
{
	int vertIndex[3];			// indices for the verts that make up this triangle
	int coordIndex[3];			// indices for the tex coords to texture this face
	int textureID;
};


// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them.
struct tMaterialInfo
{
	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	BYTE  color[3];				// The color of the object (R, G, B)
	int   texureId;				// The texture ID
	float uTile;				// u tiling of texture  
	float vTile;				// v tiling of texture	
	float uOffset;			    // u offset of texture
	float vOffset;				// v offset of texture
} ;


// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject 
{
	int  numOfMaterials;		// The number of material per object

	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object	
	char strName[255];			// The name of the object
	CVector3  *pVerts;			// The object's vertices
	CVector3  *pNormals;		// The object's normals
	CVector2  *pTexVerts;		// The texture's UV coordinates
	tFace *pFaces;				// The faces information of the object


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// We need to add a bunch of variables and lists for our bone animation

	bool bAnimated;						// This tells us if there is animation or not
	int numBones;						// This stores the number of bones for this mesh
	int numFrames;						// The number of frames of animation
	vector<tWeightInfo> vWeightInfo;	// The list of weight influence data
	vector<tBoneInfo>   vBoneInfo;		// The bone lists

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


};


// This our model structure
struct t3DModel 
{
	int numOfObjects;					// The number of objects in the model
	vector<tMaterialInfo> pMaterials;	// The list of material information (Textures and colors)
	vector<t3DObject> pObject;			// The object list for our model


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This information only needs to be stored once for all meshes so we add them here.
	int animSpeed;						// The frame rate for our animation
	int currentFrame;					// The current frame of the current animation 
	int nextFrame;						// The next frame of animation to interpolate too
	float t;							// The ratio of 0.0f to 1.0f between each key frame
	float lastTime;						// This stores the last time that was stored
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


};


extern bool  g_bFullScreen;									// Set full screen as default
extern HWND  g_hWnd;										// This is the handle for the window
extern RECT  g_rRect;										// This holds the window dimensions
extern HDC   g_hDC;											// General HDC - (handle to device context)
extern HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;								// This holds our window hInstance
extern float g_DT;											// Amount of time, in seconds, the last frame took to process

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

// This allows us to configure our window for OpenGL and back buffered
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
// In this file we added a few structures to handle our animation/bone 
// information.  You will also notice that a bunch of functions were added
// to the CVector3 class.  In order to keep track of our animation and
// bone information, we also needed to add a bunch of variables and lists
// to our t3DObject t3DModel structures.
//
//
// © 2000-2006 GameTutorials