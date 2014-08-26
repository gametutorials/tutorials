// You should include this file and sprite.cpp with your projects from now on.
// Now you don't need to cut and paste the functions you want to use, just include
// the .h and .cpp files to your project if you want to do animation

#ifndef _SPRITE_H					// If we haven't included this file
#define _SPRITE_H					// Set a flag saying we included it

#include "main.h"					// Here we include main.h because that holds all the #include info.
									// This way we don't have to #include everything in every .h file
									// like iostream and the namespace stuff.

struct SPRITE {						// This is our SPRITE structure for animation
	HBITMAP *pBitmaps;				// This is a pointer that will hold all the bitmaps
	POINT position;					// This the position of the sprite on the screen
	int currentBitmap;				// This is the index of the current bitmap being displayed
	int bitmapCount;				// This is the amount of bitmaps that the sprite animates
	int width;						// The width in pixels of the bitmap
	int height;						// The height in pixels of the bitmap
};

// This loads a sprite and reads in the number of bitmaps passed in
void LoadSprite(SPRITE *pSprite, LPSTR szFileName, int bitmapCount);

// This returns if we should animate the next frame depending on the frame rate
BOOL AnimateNextFrame(int desiredFrameRate);

// Sets a sprites X and Y position
void SetSpritePosition(SPRITE *pSprite, int x, int y);

// Animates and displays the current bitmap
void AnimateSprite(BUFFER *pBuffer, SPRITE *pSprite);

// This frees a sprite that is passed in
void FreeSprite(SPRITE *pSprite);

#endif

////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// This needs to be include from now on in your projects if
// you want to use sprites, also sprite.cpp.  This allows you
// to not have to copy and paste functions into your program,
// just include these files in your project.  This is also
// needed for animation if you are including bitmap.cpp/bitmap.h.
// 
// 
// 
// 
// 
// 
// 
// 
//
