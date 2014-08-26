//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	benjina@mindspring.com			 //
//																		 //
//		$Program:		Menus											 //
//																		 //
//		$Description:	Create a simple window with a quit menu			 //
//																		 //
//***********************************************************************//

#include "main.h"											// Include our own header file which holds all the include and structure/function prototype information

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    LoadSprite()
///// DESCRIPTION: This loads a set of bitmaps and stores it in a sprite
///// INPUT:       szFileName  - The name of the bitmap without a number:
/////			   (ie. If you have 23 bitmaps of a horse called,
/////				"horse00.bmp" through "horse22.bmp" then you just pass in "horse".
/////				The numbers are added on starting at 00 up to bitmapCount.
/////			   bitmapCount - The total amount of bitmaps to load				   
///// OUTPUT:      pSprite     - The pointer to a sprite
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void LoadSprite(SPRITE *pSprite, LPSTR szFileName, int bitmapCount)
{
	char szBuffer[255];
	char szErrorBuffer[255];
	BITMAP bitmapInfo;
	int i = 0;

	// Set the sprites bitmapCount so it knows when to start over when animating
	pSprite->bitmapCount = bitmapCount;

	// Initialize the currentBitmap to 0 (start at the first bitmap)
	pSprite->currentBitmap = 0;

	// Allocate enough dynamic memory to hold enough hBitmap's for each bitmap loaded in
	pSprite->pBitmaps = (HBITMAP *) malloc(sizeof(HBITMAP) * bitmapCount);

	// Loop until we have loaded in all the desired bitmaps
	for(i = 0; i < bitmapCount; i++)
	{
		// Store the name of the file and the number of the bitmap (ie, caust00.bmp)
		// The bitmaps must be named with 2 digits (ie, caust00.bmp caust01.bmp, etc..)
		sprintf_s(szBuffer, "%s%d%d.bmp", szFileName, i / 10, i % 10);

		// Call our load bitmap function and pass in the file name and number
		pSprite->pBitmaps[i] = LoadABitmap(szBuffer);

		// If we didn't receive a valid HBITMAP back, it didn't find the bitmap name
		if(!pSprite->pBitmaps[i]) 
		{
			// Store the bitmap file name in a buffer with a sentence to display
			sprintf_s(szErrorBuffer, "Can't find the bitmap: %s!", szBuffer);

			// Display a dialog box that says the error message and file name
			MessageBox(NULL, szErrorBuffer, "Error!", MB_OK);
			
			// Quit the program
			exit(0);
		}
	}

	// Pass in the first bitmap in the list and fill in a BITMAP structure
	GetObject(pSprite->pBitmaps[0], sizeof(BITMAP), &bitmapInfo);

	// Set the width and height of the sprite
	pSprite->width = bitmapInfo.bmWidth;
	pSprite->height = bitmapInfo.bmHeight;
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    SetSpritePosition()
///// DESCRIPTION: This sets the sprite's x and y position
///// INPUT:       x, y    - The x and y position where the sprite is displayed
///// OUTPUT:      pSprite - The sprite that we want to change it's position
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////


void SetSpritePosition(SPRITE *pSprite, int x, int y)
{
	// Set the x and y position of the sprite to the x and y passed in
	pSprite->position.x = x;
	pSprite->position.y = y;
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    AnimateNextFrame()
///// DESCRIPTION: This returns TRUE every time we want to display the next 
/////			   frame of animation.  Say we wanted 30 frames of animation
/////			   per second, it would return TRUE 30 times every second.
///// INPUT:       desiredFrameRate - The desired frame rate we want (ie. 30 fps)
///// OUTPUT:      nothing
///// RETURN:      TRUE/FALSE - Depending if the time passed is adequate
/////
//////////////////////////////////////////////////////////////////////////


BOOL AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return TRUE;
	}

	// We don't animate right now.
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    AnimateSprite()
///// DESCRIPTION: This displays the current frame of animation, then increases it. 
///// INPUT:       nothing
///// OUTPUT:      pBuffer - The back buffer to draw to
/////			   pSprite - The sprite to animate
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void AnimateSprite(BUFFER *pBuffer, SPRITE *pSprite)
{
	// Display the currentBitmap indexed into the hBitmap array.
	// We then increase the current bitmap AFTER it displays with a "++"
	DisplayBitmap(pBuffer, pSprite->pBitmaps[pSprite->currentBitmap++], 
			pSprite->position.x, pSprite->position.y);

	// Check if we reached the end of the animation
	if(pSprite->currentBitmap >= pSprite->bitmapCount)
	{
		// Reset the currentBitmap to the beginning
		pSprite->currentBitmap = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    FreeSprite()
///// DESCRIPTION: This frees the sprite and it's associated hBitmaps 
///// INPUT:       nothing
///// OUTPUT:      pSprite - The sprite to free
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void FreeSprite(SPRITE *pSprite)
{
	int i = 0;

	// Loop through all of the bitmaps
	for(i = 0; i < pSprite->bitmapCount; i++)
	{
		// If we have a valid handle to a bitmap
		if(pSprite->pBitmaps[i])
		{
			// Free the hBitmap
			DeleteObject(pSprite->pBitmaps[i]);			
		}
	}

	// This frees the dynamic memory we allocated
	free(pSprite->pBitmaps);
}	

////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
//	So, here are our sprite functions.  What is a sprite you ask?
//
//  SPRITE:  A sprite is thought of as a 2D picture on the screen
//			 that usually moves, either by animation, or just it's
//           position on the screen.  For instance, Mario jumping up
//           and down is a sprite, also all the characters in the world.
//           The background picture usually isn't considered a sprite,
//           but usually just the background image.  Also, a sprite is
//           usually thought of as an object with information about it.
//           Such as, it's X and Y position, current frame of animation,
//           it's width, height, number of frames of animation, etc...
//
//  The reason why we need the function, AnimateNextFrame(), is because
//  we want to control how fast the animation goes.  If we didn't have it
//  the program would freeze up because the pictures would be cycling so
//  fast it wouldn't have time to check for window message.  Try it :)
//  I use 30 frames of animation per second in this program, but you can 
//  increase or decrease it, depending on what your animation is.  The 
//  more frames of animation, the faster the animation cycles through each
//  bitmap.  
//
//  This tutorial was written assuming you have a bunch of bitmaps for an
//  animation.  But, what if you have 1 bitmap with a bunch of different
//  pictures in it that you want to animate?  Well, that is the next tutorial.
//  Generally, you probably want to have 1 bitmap with all the frames inside
//  it, next to each other.  Notice my bitmaps are called, caust00.bmp - caust31.bmp.
//  If you have bitmaps that you want to animate, you need to have them setup
//  like this.  Say you have a star animation.  You need to have them called,
//  star00.bmp , star01.bmp, star02.bmp ... etc... up to star11.bmp.
//  Then, call the LoadSprite() function like this:  LoadSprite(gSprite, "star", 12);
//  You just pass in the NAME (without the numbers) and it adds the numbers,
//  starting at 00.  You can change the code if you want to not be 0 padded.
//  In the star case, you have 12 bitmaps, because the first one starts at 00.
//  I chose 2 digits because animation programs sometimes spit them out like that.
