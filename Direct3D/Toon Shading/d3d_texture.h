#ifndef D3D_TEXTURE_H
#define D3D_TEXTURE_H

#include <d3dx9tex.h>

// This is our D3D texture class.  It will handle all of the 
// texture needs for our app.
class CD3DTexture
{
	public:
		
		CD3DTexture(); // Constructor
		
		// Loads the texture specified by the file name and optionally lets you specify the
		// width and height of the texture.  Returns true for success, false otherwise
		bool load(const char *fileName, int width = 0, int height = 0);
		
		void select(const char *name); // Selects the texture as the current texture into 
									  // the effect file
		
		~CD3DTexture(); // Free up memory
		
	private:
	
		IDirect3DTexture9 *mTexture; // DX9 texture interface.  This allows us to manage
									// a texture resource inside of a D3D app
		
		// We make the copy constructor and assignment operator private because
		// we do NOT want anyone accidentally making a copy of this class
		// It should always be passed by pointer or reference
		CD3DTexture(const CD3DTexture &obj) {}
		CD3DTexture& operator =(CD3DTexture &obj) { return *this; }	
};

#endif
