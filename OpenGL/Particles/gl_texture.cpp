#include <assert.h>
#include "gl_texture.h"
#include "bitmap_class.h"
#include "gl_obj.h"

// Constructor -- Set everything to a default state
CGLTexture::CGLTexture()
{
	mId = TEXTURE_NOT_LOADED; // Not loaded yet
	mAngle = 0.0f;
}

// Loads the texture specified by "fileName".  Returns
// true on success, false otherwise
bool CGLTexture::load(const char *fileName, bool buildMipMaps)
{
		// Error Check
		if(!fileName) return false;
		
	// Can't load if already loaded
	if(mId != TEXTURE_NOT_LOADED) return false; 
	
	CBitmap texture;
	
	// Try and load the texture
	if(!texture.loadBMP(fileName))
		return false;								

	// Generates a texture "name", for the number of textures
	// passed in, in the texture name array passed in.  The CGLTexture
	// class only encompasses a single texture, so we will always be 
	// generating one texture with one texture id
	glGenTextures(1, (GLuint*)&mId);

	// Bind the texture to a particular format and location in a texture
	// array.
	glBindTexture(GL_TEXTURE_2D, mId);
	
	// Determine the pixel layout of the image.  If it's a 24-bit .bmp
	// then the pixels will be laid in BGR format, otherwise we'll
	// assume it's a 32-bit .bmp and it's laid out in BGRA format
	int pixelLayout = (texture.getChannels() == 3) ? GL_BGR_EXT : GL_BGRA_EXT;
	
	// Create the texture
	// GL_TEXTURE_2D -- Type of texture, must be GL_TEXTURE_2D
	// 0 -- Mip map level.  This is the raw texture so we use 0
	// texture.getChannels() -- Number of channels in image.  Can also be special GL constants
	// texture.getWidth() -- Width of image
	// texture.getHeight() -- Height of image
	// 0 -- Size of texture border.  Can either be 0 pixels or 1 pixel
	// pixelLayout -- Layout of pixel data
	// GL_UNSIGNED_BYTE -- Data type of pixel data
	// texture.getLinePtr(0) -- Pointer to the texture's pixels
	glTexImage2D(GL_TEXTURE_2D, 0, texture.getChannels(), texture.getWidth(), texture.getHeight(), 0,
				 pixelLayout, GL_UNSIGNED_BYTE, texture.getLinePtr(0));
	
	if(buildMipMaps)
	{
		// Build Mipmaps
		gluBuild2DMipmaps(GL_TEXTURE_2D, texture.getChannels(), texture.getWidth(), texture.getHeight(),
						  pixelLayout, GL_UNSIGNED_BYTE, texture.getLinePtr(0));
						  
		// Specify texture quality		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Can only be GL_NEAR 
																		 // or GL_LINEAR
	}
	else // Specify texture quality not considering mip-maps
	{
		// Specify texture quality		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	
	return true; // Texture loaded successfully
}

// Selects this texture as the texture to be used for the drawing of any 
// geometry with a texture map -- Returns true for success, false otherwise
bool CGLTexture::select()
{
	// Can't select a texture that is not loaded
	if(mId == TEXTURE_NOT_LOADED)
		return false;

	// Load the texture matrix	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	
	// Rotate the UVs around the center of the texture
	glTranslatef(0.5f, 0.5f, 0.0f); // Translate so center of texture is at the texture 
									 // coordinate system's origin
	glRotatef(mAngle, 0.0f, 0.0f, 1.0f); // Rotate around the Z-axis 
	glTranslatef(-0.5f, -0.5f, 0.0f); // Translate back
		
	glBindTexture(GL_TEXTURE_2D, mId);
	glMatrixMode(GL_MODELVIEW);
		return true;
}

// Free the texture resource
void CGLTexture::freeMem()
{
	if(mId != TEXTURE_NOT_LOADED)
	{
		glDeleteTextures(1, (GLuint*)&mId);
		mId = TEXTURE_NOT_LOADED;
	}
}

// Deconstructor
CGLTexture::~CGLTexture() { freeMem(); }
