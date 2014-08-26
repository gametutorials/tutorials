#ifndef D3D_OBJ_H
#define D3D_OBJ_H

#include <d3d9.h>
#include <assert.h>
#include "vertex_types.h"

// Here is our 3D Object to handle all of our 3D needs -- It will create our needed D3D interfaces
// and allow us to render a list or vertices of a vertex type that we created.
// You'll notice that all of the functions/variables are static.  This is because 
// we only want ONE instance of the CD3DObj.  By making a variable or function static in a 
// class it insures that only one instance of that variable or function will be created.
class CD3DObj
{
	public:

		CD3DObj() { /* do nothing */ } // Constructor

		static bool init(HWND hwnd); // Initializes our 3D Object

		static void begin(); // Begins a scene
		static void end(); // Ends a scene

		// Renders a list of SVertex
		static bool render(SVertex *vertList, int numVerts);

		// Clears the viewport's color
		static bool clearColor(int color);
		
		static void deinit(); // Free up our 3D Object
		
		~CD3DObj() { /* do nothing */ }; // Deconstuctor
		
		// This is the "mother" of all D3D interfaces.  We'll use this to create other
		// D3D objects/interfaces.
		// **NOTE** You may also see this defined as LPDIRECT3D9 or PDIRECT3D9 which
		//			are both typedefed to be pointers to a IDirect3D9 interface
		static IDirect3D9 *mInterface;
		
		// This is the D3D object that we'll use the most.  It is what gives up "direct" access
		// to the graphics card.  By using it we can render polygons, create resources, work 
		// with system-level variables, adjust gamma ramp levels, work with palettes, 
		// and create shaders.  For this tutorial, will use our D3D device to render a white
		// triangle to the screen.
		// **NOTE** You may also see this defined as LPDIRECT3DDEVICE9 or PDIRECT3DDEVICE9 which
		//			are both typedefed to be pointers to a IDirect3DDevice9 interface
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
