#include "d3d_texture.h"
#include "d3d_obj.h"
#include <assert.h>

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
	
	// Load the texture
	return (D3DXCreateTextureFromFile(CD3DObj::mDevice, fileName, &mTexture) == D3D_OK);
}

// Selects this texture as the texture to be used for the drawing of any 
// geometry with a texture map
void CD3DTexture::select()
{
	assert(CD3DObj::mDevice != NULL); // Make sure device is valid

	HRESULT result = CD3DObj::mEffect->SetTexture("gTexture", mTexture);
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
