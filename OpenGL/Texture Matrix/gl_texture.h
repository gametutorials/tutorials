#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#define TEXTURE_NOT_LOADED -1 // 
#define TEX_SCROLL_AMT 0.005f // Default amount to scroll a texture UVs by
#define TEX_SCALE_AMT 0.001f // Default amount to scale texture UVs by
#define TEX_ROTATION_AMT 0.25f // Default amount to increase/decrease texture's UVs angle of rotation

// OGL Texture class
class CGLTexture
{
	public:
		
		CGLTexture(); // Constructor
		
		// Loads the texture specified by the file name
		// Returns true on success, false otherwise 
		bool load(const char *fileName, bool buildMipMaps = false);
		
		void select(); // Selects the texture as the current texture to use
		
		enum EScrollDir
		{
			eLeft, 
			eRight, 
			eUp,
			eDown,
			eMaxDir
		};
		
		void scroll(EScrollDir dir, float amt = TEX_SCROLL_AMT); // Set the scrolling direction and
																// the amount to scroll in that 
															   // direction
		// Set the scale amount of the texture
		void setScale(float amt) { scale = amt; }
		
		// Set the rotation angle of the texture's UVs
		void setRotation(float rotAngle) { angle = rotAngle; }
		
		
		
		void freeMem(); // Frees up the texture memory
		
		~CGLTexture(); // Free up memory
		
	private:
	
		int id; // The texture id
		
		float tx; // Amount to translate the UVs in the X direction 
		float ty; // Amount to translate the UVs in the Y direction
		float scale; // Amount to scale the UVs by
		float angle; // Amount to rotate the UVs by
		
		// We make the copy constructor and assignment operator private because
		// we do NOT want anyone accidentally making a copy of this class
		// It should always be passed by pointer or reference
		CGLTexture(const CGLTexture &obj) {}
		CGLTexture& operator =(CGLTexture &obj) { return *this; }	
};

#endif
