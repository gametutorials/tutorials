#include "d3d_texture.h"
#include "d3d_obj.h"
#include <assert.h>

// Zero out everything
CD3DTexture::CD3DTexture()
{
	mTexture = NULL;
	mSurface = NULL;
}

// Returns true if we could create the requested render texture, false otherwise
bool CD3DTexture::createRenderTexture(int wid, int hgt, D3DFORMAT format)
{
	// Create a texture that can be used as a render target
	// By parameter:
	// wid -- Width of texture
	// hgt -- Height of texture
	// 1 -- Number of mimmap levels.  We want one level because this is a surface to render to
	// D3DUSAGE_RENDERTARGET -- Usage flag to say "this is gonna be a render target"
	// format -- The data format of the texture
	// D3DPOOL_DEFAULT -- Memory pool of the render texture.  It needs to be default for render targets
	// &mTexture -- The IDirect3DTexture9 to get filled
	// NULL -- MS likes to reserve handles and have us pass NULL, helps us type better :) 
	HRESULT r = CD3DObj::mDevice->CreateTexture(wid, hgt, 1, D3DUSAGE_RENDERTARGET, 
												format, D3DPOOL_DEFAULT, &mTexture, NULL);

	if(r != D3D_OK)
		return false;
	
	// Get the surface of the texture	
	if(mTexture->GetSurfaceLevel(0, &mSurface) != D3D_OK)
		return false;
		
	return true;		
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
	deinit();
	
	// Load the texture
	if(D3DXCreateTextureFromFile(CD3DObj::mDevice, fileName, &mTexture) == D3D_OK)
	{
		if(mTexture->GetSurfaceLevel(0, &mSurface) == D3D_OK)
			return true;
	}
	
	return false;
}

// Selects this texture into the current shader
void CD3DTexture::select()
{
	assert(CD3DObj::mDevice != NULL); // Make sure device is valid

	// Select the texture into "gTexture" in our shader
	HRESULT result = CD3DObj::mShader->SetTexture("gTexture", mTexture);
	assert(result == D3D_OK);
}

// Release all allocated memory
void CD3DTexture::deinit()
{
	SAFE_RELEASE(mSurface);
	SAFE_RELEASE(mTexture);
} 

int CD3DTexture::getWidth()
{
	// If there is a valid surface
	if(mSurface)
	{
		D3DSURFACE_DESC desc;
		
		// Get and return the texture's width
		if(mSurface->GetDesc(&desc) == D3D_OK)
			return desc.Width;
	}
	
	return 0;
}

int CD3DTexture::getHeight()
{
	// If there is a valid surface
	if(mSurface)
	{
		D3DSURFACE_DESC desc;
		
		// Get and return the texture's width
		if(mSurface->GetDesc(&desc) == D3D_OK)
			return desc.Height;
	}
	
	return 0;
}

