#ifndef D3D_TEXTURE_H
#define D3D_TEXTURE_H

#include <d3dx9tex.h>

// This is our D3D texture class.  It will handle all of the 
// texture needs for our app.
class CD3DTexture
{
	public:
		
		CD3DTexture(); // Constructor
		
		// Creates an empty texture to be render to
		bool createRenderTexture(int wid, int hgt, D3DFORMAT format);
		
		// Loads the texture specified by the file name
		// Returns true on success, false otherwise 
		bool load(const char *fileName);
		
		void select(); // Selects the texture as the current texture into the effect file
		
		void deinit(); // Free's up all memory
					  
		~CD3DTexture() { deinit(); }
		
		// Data Access ***
		
			IDirect3DSurface9* getSurface() { return mSurface; }
		
			int getWidth(); // Width of texture
			int getHeight(); // Height of texture
		
		// *** End Data Access
		
	private:
	
		IDirect3DTexture9 *mTexture; // DX9 texture interface -- This allows us to manage
									// a texture resource inside of a D3D app
									
		IDirect3DSurface9 *mSurface; // DX9 surface interface.  This allows us access to the surface
									 // pixels.
		
		// We make the copy constructor and assignment operator private because
		// we do NOT want anyone accidentally making a copy of this class
		// It should always be passed by pointer or reference
		CD3DTexture(const CD3DTexture &obj) {}
		CD3DTexture& operator =(CD3DTexture &obj) { return *this; }	
};

#endif
