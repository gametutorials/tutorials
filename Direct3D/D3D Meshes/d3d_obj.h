#ifndef D3D_OBJ_H
#define D3D_OBJ_H

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <assert.h>
#include "vertex_types.h"

// A position in 3D space
class CPos
{
	public:
	
		CPos() { /* do nothing */ }
		CPos(float X, float Y, float Z) : x(X), y(Y), z(Z) { /* do nothing */ }
		
		// Public data
		float x, y, z;
};

// An RGBA color 
// (0,0,0) == Black
// (1,1,1) == White
class CColor
{
	public:
	
		CColor() { /* do nothing */ }
		CColor(float R, float G, float B) : r(R), g(G), b(B) { /* do nothing */ }
		
		// Public data
		float r, g, b;
};

// Our 3D Object that allows for rendering and setting 
// projection, view, and world matrices
class CD3DObj
{
	public:

		CD3DObj() { /* do nothing */ } // Constructor

		static bool init(HWND hwnd); // Initializes our 3D Object

		static void begin(); // Begins a scene
		static void end(); // Ends a scene

		// Renders a ID3DXMesh
		static bool render(ID3DXMesh *mesh);

		// Functions to set either the projection, view, or world matrix
		static void setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip);
		static void setViewMatrix(const CPos &eye, const CPos &target); 
		static void setWorldMatrix(const D3DXMATRIX *matrix);

		// Clears the viewport color and z-buffer
		static bool clear(int color = D3DCOLOR_XRGB(0,0,0), float zDepth = 1.0f);

		static void deinit(); // Free up our 3D Object

		~CD3DObj() { /* do nothing */ }; // Deconstuctor

		// This is the "mother" of all D3D interfaces.  We'll use this to create other
		// D3D objects/interfaces.
		static IDirect3D9 *mInterface;

		// This is the D3D object that we'll use the most.  It gives us "direct" access to 
		// the video card.  By using it we can render polygons, create resources, work with 
		// system-level variables, adjust gamma ramp levels, work with palettes, and create shaders.
		static IDirect3DDevice9 *mDevice;

	private:

		static HRESULT mResult; // Used for error checking

		// We make the copy constructor and assignment operator private because
		// we do NOT want anyone accidentally making a copy of this class
		CD3DObj(const CD3DObj &obj) {}
		CD3DObj& operator =(CD3DObj &obj) { return *this; }
};

// Here we define our global 3D object
extern CD3DObj *g3D;

#endif
