#include <assert.h>
#include "gl_texture.h"
#include "bitmap_class.h"
#include "gl_obj.h"

// Constructor -- Set everything to a default state
CGLTexture::CGLTexture()
{
	id = TEXTURE_NOT_LOADED; // Not loaded yet
	tx = ty = 0.0f;
	scale = 1.0f; // No scale
	angle = 0.0f;
}

// Loads the texture specified by "fileName".  Returns
// true on success, false otherwise
bool CGLTexture::load(const char *fileName, bool buildMipMaps)
{
		// Error Check
		if(!fileName) return false;
		
	// Can't load if already loaded
	if(id != TEXTURE_NOT_LOADED) return false; 
	
	CBitmap texture;
	
	// Try and load the texture
	if(!texture.loadBMP(fileName))
		return false;								

	// Generates a texture "name", for the number of textures
	// passed in, in the texture name array passed in.  The CGLTexture
	// class only encompasses a single texture, so we will always be 
	// generating one texture with one texture id
	glGenTextures(1, (GLuint*)&id);

	// Bind the texture to a particular format and location in a texture
	// array.
	glBindTexture(GL_TEXTURE_2D, id);
	
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
// geometry with a texture map
void CGLTexture::select()
{
	assert(id != TEXTURE_NOT_LOADED);

	// Load the texture matrix	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	
	// Scale the texture matrix
	glScalef(scale, scale, scale);
	
	// Rotate the UVs around the center of the texture
	glTranslatef(0.5f, 0.5f, 0.0f); // Translate so center of texture is at the texture 
									 // coordinate system's origin
	glRotatef(angle, 0.0f, 0.0f, 1.0f); // Rotate around the Z-axis 
	glTranslatef(-0.5f, -0.5f, 0.0f); // Translate back
	
	// Translate the texture coordinates
	glTranslatef(tx, ty, 0.0f);
		
	glBindTexture(GL_TEXTURE_2D, id);
	glMatrixMode(GL_MODELVIEW);
}

// Scroll the texture matrix in the passed in direction
void CGLTexture::scroll(EScrollDir dir, float amt)
{
	switch(dir)
	{
		case eLeft:
			tx += amt;
				break;
				
		case eRight:
			tx -= amt;
				break;
				
		case eUp:
			ty -= amt;
				break;
				
		case eDown:
			ty += amt;
				break;
	}			
}

// Free the texture resource
void CGLTexture::freeMem()
{
	if(id != TEXTURE_NOT_LOADED)
	{
		glDeleteTextures(1, (GLuint*)&id);
		id = TEXTURE_NOT_LOADED;
	}
}

// Deconstructor
CGLTexture::~CGLTexture() { freeMem(); }
