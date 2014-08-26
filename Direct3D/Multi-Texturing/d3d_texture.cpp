#include "d3d_texture.h"
#include "d3d_obj.h"

// Constructor -- Zero out everything
CD3DTexture::CD3DTexture()
{
	mTexture = NULL;
}

// Loads the texture specified by "fileName".  Returns
// true on success, false otherwise
bool CD3DTexture::load(const char *fileName)
{
	// Error Check
	if(!fileName)
		return false;
		
	// Error Check -- If device isn't valid return false
	if(!CD3DObj::mDevice) 
		return false; 
	
	// If a texture is already loaded, free it before we load a new one
	if(mTexture)
	{
		mTexture->Release();
		mTexture = NULL;
	}
	
	// D3D makes it VERY EASY for us to load a texture.  Lets look at 
	// this function by parameter:
	// CD3DObj::mDevice -- Pointer to a IDirect3DDevice9 (D3D display device)
	//					   that is to be associated with the texture
	// fileName -- The name of the texture file to load.  Valid texture file
	//			   extensions are:  .bmp, .dib, .dds, .jpg, .png, .tga
	// &mTexture -- The address of a pointer to a IDirect3DTexture9 (D3D texture
	//				   interface) that will be filled with the created texture resource
	return (D3DXCreateTextureFromFile(CD3DObj::mDevice, fileName, &mTexture) == D3D_OK);
	
	// **NOTE** This function creates a full mipmap chain.  So if your image, for instance, is 
	// 128x128 this will produce mipmaps of sizes 64x64, 32x32, 16x16, 8x8, 4x4, 2x2, and 1x1. 
}

// Selects this texture as the texture to be used for the drawing of any 
// geometry with a texture map
void CD3DTexture::select(int stage)
{
	assert(CD3DObj::mDevice != NULL); // Make sure device is valid

	// Select this texture, at the given texture stage, to be used for rendering
	// By parameter:
	// stage -- The texture stage (0 - max texture stages for the graphics device)
	// mTexture -- The texture resource we want to select
	HRESULT result = CD3DObj::mDevice->SetTexture(stage, mTexture);
	assert(result == D3D_OK);
}

// Free up the memory
CD3DTexture::~CD3DTexture()
{
	// If we have a D3D texture resource, free it
	if(mTexture)
	{
		mTexture->Release();
		mTexture = NULL;
	}
}
