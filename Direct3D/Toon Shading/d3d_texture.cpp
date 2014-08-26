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
bool CD3DTexture::load(const char *fileName, int width, int height)
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
	
	// Okay if width and height are set, great!  We'll use those values.  Otherwise
	// we set them to the D3DX_DEFAULT flag.  This says the width and height will be
	// taken directly from the image loaded, then rounded up to the nearest power of 2,
	// if the width or height is not already a power of 2.
	if(width == 0 && height == 0)
	{
		width = D3DX_DEFAULT;
		height = D3DX_DEFAULT;
	}
	
	// Load the texture.  We do not want mip maps, so we need to specify that on load.
	// By parameter:
	// CD3DObj::mDevice -- The IDirect3DDevice9 that we are using.
	// fileName -- The full path name of the file to be loaded.
	// width -- Width of texture in pixels.
	// height -- Height of texture in pixels.
	// D3DX_FROM_FILE -- The number of mip-map levels to generate.  By passing D3DX_FROM_FILE we
	//					 tell it not to create mip-map levels and that we're just using the texture
	//					 as is.
	//					 **NOTE** If the textures dimensions are not supported, this call will fail
	// 0 -- Special usage for the texture, typically a dynamic texture or a render target.  We don't
	//		have any special usage for the texture so we pass 0.
	// D3DFMT_FROM_FILE -- The pixel format of the image being loaded.  By passing D3DFMT_FROM_FILE we
	//					   saying we want the texture to have the same pixel format as the image loaded.
	// D3DPOOL_MANAGED -- Memory allocation scheme to use for the texture.  Managed memory is managed
	//					  by DirectX and is backed up in system memory and automatically copied to
	//					  the video card when needed.
	// D3DX_FILTER_POINT -- This specifies how the texture is filtered, or rather, how a color is 
	//						calculated from it when it's time to draw a pixel to the screen.  The
	//						D3DX_FILTER_POINT gets it's color information from the texture by
	//						by using the given texture coordinates, sampling the nearest 
	//						pixel in the texture and returning it.
	// D3DX_DEFAULT -- This specifies how mip-maps are filtered.  We don't have any, so we 
	//				   just pass the default mip-map filter.
	// 0 -- A color key for transparency.  We don't have a transparent color key, so we pass 0 to
	//	    disable color key transparency.
	// NULL -- Optional pointer to a D3DXIMAGE_INFO to be filled when creating the texture.
	// NULL -- Optional pointer to PALETTEENTRY to be filled with a 256 color palette for
	//		   textures using a palette.
	// &mTexture -- Pointer to a IDirect3DTexture9 to be filled with the created texture. 
	HRESULT r = D3DXCreateTextureFromFileEx(CD3DObj::mDevice, fileName, width, height,
											D3DX_FROM_FILE, 0, D3DFMT_FROM_FILE, D3DPOOL_MANAGED,
											D3DX_FILTER_POINT, D3DX_DEFAULT, 0, NULL, NULL, &mTexture);
											
	// Return the result of loading
	return (r == D3D_OK);
}

// Selects this texture as the texture to be used for the drawing of any 
// geometry with a texture map
void CD3DTexture::select(const char* name)
{
	assert(CD3DObj::mDevice != NULL); // Make sure device is valid

	HRESULT result = CD3DObj::mEffect->SetTexture(name, mTexture);
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