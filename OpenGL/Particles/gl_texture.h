#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

#define TEXTURE_NOT_LOADED -1 // ID value for a texture that hasn't been loaded

// OGL Texture class
class CGLTexture
{
	public:
		
		CGLTexture(); // Constructor
		
		// Loads the texture specified by the file name
		// Returns true on success, false otherwise 
		bool load(const char *fileName, bool buildMipMaps = false);
		
		bool select(); // Selects the texture as the current texture to use
					  // Returns true for success, false otherwise
		
		// Set the rotation angle of the texture's UVs
		void setRotation(float rotAngle) { mAngle = rotAngle; }
			
		// Data Access ***
		
			int getId() { return mId; }
			float getRotAngle() { return mAngle; }
		
		// *** End Data Access
		
		void freeMem(); // Frees up the texture memory
		
		~CGLTexture(); // Free up memory
		
	private:
	
		int mId; // The texture id
		
		float mAngle; // Amount to rotate the UVs by
		
		// We make the copy constructor and assignment operator private because
		// we do NOT want anyone accidentally making a copy of this class
		// It should always be passed by pointer or reference
		CGLTexture(const CGLTexture &obj) {}
		CGLTexture& operator =(CGLTexture &obj) { return *this; }	
};

#endif
