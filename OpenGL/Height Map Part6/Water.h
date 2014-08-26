#ifndef _WATER_H
#define _WATER_H


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This clamps our animation to a frame rate (like 30 frames)
bool AnimateNextFrame(int desiredFrameRate);

// This loads all of our animation textures and stores them in our texture array
void LoadAnimTextures(UINT textureArray[], LPSTR szFileName, int startIndex, int bitmapCount);

// This renders the current screen to a texture map, given a specified size and type
void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID);

// This cycles through our caustic bitmaps to produce the animation
void RenderCaustics(float waterHeight, float causticScale);

// This renders our screen to a texture that is used for reflection
void CreateReflectionTexture(float waterHeight, int textureSize);

// This renders our screen to a texture for refraction and depth
void CreateRefractionDepthTexture(float waterHeight, int textureSize);

// This draws our quad for the water
void RenderWater(float waterHeight);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Above are our functions necessary for the basics of the water code.
//
//
// ©2000-2005 GameTutorials