#include "main.h"											// Include our own header file which holds all the include and structure/function prototype information

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    LoadSprite()
///// DESCRIPTION: This loads a bitmap then display parts of it to animate
///// INPUT:       szFileName  - The name of the bitmap to load in (ie, "robot.bmp")
/////			   lastFrame   - The total amount of frames for each cycle of animation				   
/////			   width/height- The width and height of one frame of animation
///// OUTPUT:      pSprite     - The pointer to a sprite
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void LoadSprite(SPRITE *pSprite, LPSTR szFileName, int width, int height, int lastFrame)
{
	char szBuffer[255];
	int i = 0;

	// Set the sprites bitmapCount so it knows when to start over when animating
	pSprite->lastFrame = lastFrame - 1;

	// Initialize the currentFrame to 0 (start at the first frame)
	pSprite->currentFrame= 0;

	// Set the sprite animation to the first state (WEST)
	SetSpriteState(pSprite, WEST);

	// Set the sprites speed to default
	SetSpriteSpeed(pSprite, 5, 5);

	// Call our load bitmap function and pass in the bitmap to load
	pSprite->hBitmap = LoadABitmap(szFileName);

	// If we didn't receive a valid HBITMAP back, it didn't find the bitmap name
	if(!pSprite->hBitmap) 
	{
		// Store the bitmap file name in a buffer with a sentence to display
		sprintf(szBuffer, "Can't find the bitmap: %s!", szFileName);

		// Display a dialog box that says the error message and file name
		MessageBox(NULL, szBuffer, "Error!", MB_OK);
			
		// Quit the program
		exit(0);
	}

	// Set the width and height of the sprite
	pSprite->width = width;
	pSprite->height = height;
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
///// FUNCTION:    SetSpriteState()
///// DESCRIPTION: This sets the sprite's state such as WEST, NORTH, etc...
///// INPUT:       whichState - This is the state (#define for which animation)
///// OUTPUT:      pSprite - The sprite that we want to change it's position
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////


void SetSpriteState(SPRITE *pSprite, int whichState)
{
	// Set the sprites state such as WEST, EAST, etc...
	pSprite->state = whichState;
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    SetSpriteSpeed()
///// DESCRIPTION: This sets the sprite's x and y speed (in pixels)
///// INPUT:       xSpeed, ySpeed    - The x and y speed of the sprite
///// OUTPUT:      pSprite - The sprite that we want to change it's position
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void SetSpriteSpeed(SPRITE *pSprite, int xSpeed, int ySpeed)
{
	// Set the X and Y speed of the sprite
	pSprite->speed.x = xSpeed;
	pSprite->speed.y = ySpeed;
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
	// This holds the rectangle coordinates we want to display from the bitmap
	RECT rPortion;
	// This gets the y value in which we want to start at.
	// ie. If it's the third frame, we want to go down the file 3 times the height
	int yPos = pSprite->currentFrame * pSprite->height + 1 + pSprite->currentFrame + ((pSprite->lastFrame + 1) * pSprite->state) +
				     (pSprite->state * ((pSprite->lastFrame + 1) * pSprite->height));

	// Fill in the structure with coordinated we need to get from the bitmap
	rPortion.left = 1;
	rPortion.top =  yPos;
	rPortion.right = pSprite->width + 1;
	rPortion.bottom = yPos + pSprite->height;

	// Display the currentBitmap indexed into the hBitmap array.
	// This function calls TransparentBlit() which leaks memory on win98.  
	// A future tutorial should fix this by writing our own function for transparency.
	DisplayTransparentBitmap(pBuffer, pSprite->hBitmap, 
						   pSprite->position.x, pSprite->position.y, rPortion);

	// Increment to the next frame
	pSprite->currentFrame++;

	// Check if we reached the end of the animation
	if(pSprite->currentFrame > pSprite->lastFrame)
	{
		// Reset the currentBitmap to the beginning
		pSprite->currentFrame = 0;
	}

}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    MoveSprite()
///// DESCRIPTION: This Moves the sprite on screen depending on it's direction and speed
/////              You will most likely want to change this for your games
///// INPUT:       nothing
///// OUTPUT:      pSprite - The sprite to be moved
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void MoveSprite(SPRITE *pSprite)
{
	// Check the sprites direction
	switch(pSprite->state)
	{	
		case NORTH:
			// Move the sprite north
			SetSpritePosition(pSprite, pSprite->position.x, (pSprite->position.y - pSprite->speed.y) % HEIGHT);
			break;
		case SOUTH:
			// Move the sprite south
			SetSpritePosition(pSprite, pSprite->position.x, (pSprite->position.y + pSprite->speed.y) % HEIGHT);
			break;
		case EAST: 
			// Move the sprite east
			SetSpritePosition(pSprite, (pSprite->position.x + pSprite->speed.x) % WIDTH, pSprite->position.y);
			break;
		case WEST:
			// Move the sprite west
			SetSpritePosition(pSprite, (pSprite->position.x - pSprite->speed.x) % WIDTH, pSprite->position.y);
			break;
	}
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    EraseSprite()
///// DESCRIPTION: This replaces the sprite with a potion of the background
///// INPUT:       hBackground - The bitmap that holds the background
///// OUTPUT:      pBuffer - The back buffer to draw to
/////			   pSprite - The sprite to erase
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void EraseSprite(BUFFER *pBuffer, SPRITE *pSprite, HBITMAP hBackground)
{
	// Store the portion of the background that we need to redraw
	// We add and subtract the speed because if we switch states it sometimes moves
	// Without erasing the current position.  Try and see for yourself.  delete the speed from here and below
	RECT Portion= {pSprite->position.x - pSprite->speed.x, 
				   pSprite->position.y - pSprite->speed.y, 
				   pSprite->position.x + pSprite->width + pSprite->speed.x, 
				   pSprite->position.y + pSprite->height + pSprite->speed.y};

	// Display only the portion of the background that needs to be redrawn
	DisplayPortionOfBitmap(pBuffer, hBackground, pSprite->position.x - pSprite->speed.x, pSprite->position.y - pSprite->speed.y, Portion);
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
	// Check if the handle to the bitmap is valid
	if(pSprite->hBitmap)
	{
		// Free the hBitmap
		DeleteObject(pSprite->hBitmap);			
	}
}	

////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// Most of these functions could remain the same, except the MoveSprite().	
// This one is specific to your game, so feel free to change it.  Right now
// it wraps the sprite around the screen, which you probably won't want. 
// You will probably want to include this whole .cpp with your other apps.	
//  
// 
