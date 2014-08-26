//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Fire											 //
//																		 //
//		$Description:	Using GDI and DIBSections we create fire!		 //
//																		 //
//***********************************************************************//


// Include our own header file which holds all the include and structure/function prototype information
#include "main.h"									

// Create our double buffering structure
BUFFER gBuffer;												
	
unsigned char* pImageBits;
BITMAPINFO bmBitmapInfo = {0};
HBITMAP hBitmap, hOldBitmap;
HDC hdc2;

#define FIRE_X		0
#define FIRE_Y		0
#define FIRE_WIDTH	WIDTH
#define FIRE_HEIGHT	HEIGHT

int gCoalPercentage = 30;

int RValue = 255;
int GValue = 128;
int BValue = 0;

int gFireIntensity   = 1;
int gCoalWidth       = 3;
int gCoalHeight      = 1;

bool bRandomColor = false;
int gDirection       = 0;

#define GetIndex(x, y) ((y * 3 * FIRE_WIDTH) + x)

///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This changes the screen to FULL SCREEN
/////
///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ChangeToFullScreen()
{
	DEVMODE dmSettings;									// Device Mode

	memset(&dmSettings,0,sizeof(dmSettings));			// Makes Sure Memory's Cleared

	// Get current settings -- 
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= WIDTH;			// Selected Screen Width
	dmSettings.dmPelsHeight	= HEIGHT;			// Selected Screen Height
	dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; // Say we're changing Screen Width/Height
														
	// CDS_FULLSCREEN Gets Rid Of Start Bar.
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
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
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
    float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
    if( elapsedTime > (1.0f / desiredFrameRate) )
    {
		// Reset the last time
        lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
        return TRUE;
    }

	// We don't animate right now.
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    CalculateFrameRate()
///// DESCRIPTION: This Calculates the frames per second and stores it in the window title
///// INPUT:       hWnd - The handle to the window to display the result
///// OUTPUT:      nothing
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void CalculateFrameRate(HWND hWnd)
{
	static float fFPS      = 0.0f;
    static float fLastTime = 0.0f;
    static DWORD dwFrames  = 0;
	char strFrameRate[55] = {0};

    // Keep track of the time lapse and frame count
    float fTime = GetTickCount() * 0.001f; // Convert time to seconds
    ++dwFrames;

    // If our current time minus the last recorded time is great than 1 second, start over
    if( fTime - fLastTime > 1.0f)
    {
		// Store the current time to test again
        fLastTime = fTime;

		// Store our Frames Per Second Into a buffer
		sprintf(strFrameRate, "Frames Per Second: %d", dwFrames);

		// Display the Frames Per Second in our window title
		SetWindowText(hWnd, strFrameRate);

		// Reset the frames
		dwFrames  = 0;
    }
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    MakeCoal()
///// DESCRIPTION: This places a coal at an x and y position with a specific color
///// INPUT:       X, Y - The position to display the coal at starting from the center
/////              R, G, B - The RGB color to make the coal
///// OUTPUT:      pImageBits - The array to the bitmap bits
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void MakeCoal(unsigned char *pImageBits, int x, int y, UCHAR R, UCHAR G, UCHAR B)
{
	int index = 0;

	// Make sure we don't go outside of our array index
	if(x >= 3) 
	{
		// Draw the 2 levels of pixels
		for(int i = 0; i < 2; i++)
		{
			// Draw a pixel up 1 and to the left
			index = GetIndex((x - 3), (y + i));
			pImageBits[index] = B;
			pImageBits[index + 1] = G;
			pImageBits[index + 2] = R;

			// Draw a pixel up and to the right
			index = GetIndex((x + 3), (y + i));
			pImageBits[index] =  B;
			pImageBits[index + 1] = G;
			pImageBits[index + 2] = R;
		
			// Draw a pixel in the bottom and up
			index = GetIndex(x, (y + i));
			pImageBits[index] = B;
			pImageBits[index + 1] = G;
			pImageBits[index + 2] = R;
		}
	}

	// If we are drawing black coals, stop the function so we leave the top orange pixel
	// Also, if the fire intensity is above 1, we don't want to leave the top pixel there
	if(!R && !G && !B && (gFireIntensity < 2)) return;

	// Draw the top pixel (up 3 from the bottom)
	index = GetIndex(x, (y + 2));
	pImageBits[index] = B;
	pImageBits[index + 1] = G;
	pImageBits[index + 2] = R;

}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    GetSurroundingPixelAverage()
///// DESCRIPTION: This takes the values of the surrounding pixels and return the average
///// INPUT:       X, Y - The position of the pixel to get the surrounding average
/////		       pImageBits - The array to the bitmap bits
///// RETURN:      avgColor - The average value of the pixels surrounding
/////
//////////////////////////////////////////////////////////////////////////

unsigned char GetSurroundingPixelAverage(unsigned char *pImageBits, int x, int y)  
{  
	unsigned char below, above, right, rightAbove, rightBelow; 
	unsigned char left = 0, leftAbove = 0, leftBelow = 0;		// Note how we init these variables because they might not get set
	unsigned char avgColor;
	int index = 0;  

	below		= pImageBits[GetIndex(x, (y - 1))];				// Below The pixel
	above		= pImageBits[GetIndex(x, (y + 1))];				// Above the pixel
	rightBelow	= pImageBits[GetIndex((x + 3), (y - 1))];		// Right/Below of the pixel
	rightAbove	= pImageBits[GetIndex((x + 3), (y + 1))];		// Right/Above of the pixel
	right		= pImageBits[GetIndex((x + 3), y)];				// Right of the pixel

	// Check to see if we aren't going outside the bounds of the image (array)

	if(y >= 1 || x >= 3)  
		leftAbove	= pImageBits[GetIndex((x - 3), (y + 1))];	// Left/Above of the pixel
	if(y >= 1 || x >= 3)  
		left		= pImageBits[GetIndex((x - 3), y)];			// Left of the pixel
	if(y >= 2 || x >= 3)  
		leftBelow	= pImageBits[GetIndex((x - 3), (y - 1))];	// left/Below of the pixel

	// Get the average of the value (could be R G or B) and divide it by 8 ( >> 3 is faster)
	avgColor = (below + above + leftBelow + rightBelow + left + right + leftAbove + rightAbove) >> 3;  

	// This makes sure we don't get a banding effect.  It fades the top faster
	if(avgColor) 
		if( (avgColor - (y / 30)) > 0)
			avgColor -= (y / 30);

	// Keep shrinking the value to produce a better fade
	if(avgColor) avgColor--;

	// Return the avgColor (could be the R the G or B, depending on what X is passed in)
	return(avgColor);  
} 

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    AverageImage()
///// DESCRIPTION: Takes a array to image bits and produces a burning effect upwards
///// INPUT:       width/height - The width and height of the image to effect
///// OUTPUT:      pImageBits - The array to the bitmap bits
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void AverageImage(unsigned char *pImageBits, int width, int height)
{
	int currentIndex = 0;
	int direction = 0;

	// Average all the pixels depending on the height except the bottom row
	for(int y = 1; y < height; y++)
	{
		// Average the current X line
		for(int x = 0; x < width * 3; x += 3)
		{
			// Get the current index (xPos + (y * bytes-per-pixel) * imageWidth))
			currentIndex = x + ((y * 3) * width);

			// the gStepAmount changes the flame height and gDirection makes a blowEffect if used (F5/F6 for blow effect)
			// If you don't want these, it would be just (x + ((y + 1) * 3) * width) for the index
			pImageBits[x + (((y + gFireIntensity) * 3) * (width + gDirection))]     = GetSurroundingPixelAverage(pImageBits, x, y);		
			pImageBits[x + (((y + gFireIntensity) * 3) * (width + gDirection)) + 1] = GetSurroundingPixelAverage(pImageBits, x + 1, y);		
			pImageBits[x + (((y + gFireIntensity) * 3) * (width + gDirection)) + 2] = GetSurroundingPixelAverage(pImageBits, x + 2, y);
		}	
	}

	// This sets the X direction of the flame back to 0 to produce a cheap wind effect on the flame
	gDirection = 0;
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    InitFire()
///// DESCRIPTION: This creates the DIB section into a backbuffer
///// INPUT:       destX/destY - The start x and y position to blit to the frontbuffer
/////			   destWidth/destHeight - The width and height of the desired DIB section
///// OUTPUT:      hdc - The front buffer hdc
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void InitFire(HDC hdc, int destX, int destY, int destWidth, int destHeight)
{

	// Fill in our BitmapInfo structure (we want a 24 bit image)
	bmBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmBitmapInfo.bmiHeader.biCompression = BI_RGB;
	bmBitmapInfo.bmiHeader.biHeight = destHeight;
	bmBitmapInfo.bmiHeader.biWidth = destWidth;	
	bmBitmapInfo.bmiHeader.biBitCount = 24;
	bmBitmapInfo.bmiHeader.biClrUsed = 0;
	bmBitmapInfo.bmiHeader.biPlanes = 1;

	// Create 2 DIB Sections.  One for the Front Buffer and one for the BackBuffer
	hBitmap  = CreateDIBSection(hdc,  &bmBitmapInfo, DIB_RGB_COLORS,(void **)&pImageBits, 0,0);	

	// Create a compatible DC for the front buffer and Select our Dib Section into it
	hdc2 = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(hdc2, hBitmap);

	// Blit the transparent image to the front buffer (Voila!)
	BitBlt(hdc, destX, destY, destWidth, destHeight, hdc2, 0, 0, SRCCOPY);
}


//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    DrawFire()
///// DESCRIPTION: This puts the fire effect into our global DIB section and display it
///// INPUT:       destX/destY - The start x and y position to blit to the frontbuffer
/////			   destWidth/destHeight - The width and height of the desired DIB section
///// OUTPUT:      hdc - The front buffer hdc
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

				
void DrawFire(HDC hdc, int destX, int destY, int destWidth, int destHeight)
{
	int randomValue = 0;
	int randomIndex[FIRE_WIDTH * 3] = {0};

	// Draw coals onto the screen, depending on the height we have set
	for(int y = 0; y < gCoalHeight; y++)
	{
		// Draw random coals on the current line
		for(int x = 0; x < gCoalWidth; x += 3)
		{
			// Depending on the percentage, draw a coal on that X value
			if(((rand() % 100) < gCoalPercentage)) 
			{
				// Draw a coal at the x and y position with the current color
				MakeCoal(pImageBits, x, y, RValue, GValue, BValue);
				// Set a flag that shows we set a coal there so we can erase it later
				randomIndex[x] = 1;			
			}		
		}
		y += rand() % 5;
	}

	// Now do the fire effect on the coals
	AverageImage(pImageBits, destWidth, destHeight / 2);

	// Destroy the current line with black coals before we draw it to the screen
	for(int x = 0; x < gCoalWidth; x += 3)
	{
		// Depending on the percentage, draw a coal on that X value
		if(randomIndex[x]) 
		{
			// Draw a black coal at the x and y position to erase the previous
			MakeCoal(pImageBits, x, 0, 0, 0, 0);
			// Erase the flag
			randomIndex[x] = 0;			
		}		
	}

	// Blit the fire to the front buffer
	BitBlt(hdc, destX, destY, destWidth, destHeight, hdc2, 0, 0, SRCCOPY);


	// Make the color change if we have a random color on
	if(bRandomColor) 
	{
		// Change the R G and B value but make sure we don't go over 255
		RValue = ((RValue + (rand() %  2)) % 256);
		GValue = ((GValue + (rand() %  2)) % 256);
		BValue = ((BValue + (rand() %  2)) % 256);
	}

	// If the fire is less than the screen width than increase it (produces the coming in effect) 
	if(gCoalWidth < (destWidth * 3 - 3)) 
	{
		// Increase the coal width by an arbitrary number
		gCoalWidth += ((destWidth * 3 - 3) / 35);
	}

	// Make sure we stop the fire if the fire is coming in from the side
	if(gCoalWidth > (destWidth * 3 - 3)) 
	{
		// Set the coal width to the end of the screen
		gCoalWidth = (destWidth * 3 - 3);
	}
}

//////////////////////////////////////////////////////////////////////////
/////
///// FUNCTION:    PlasmaBurst()
///// DESCRIPTION: This makes a burst of fire come up when we press the space bar
///// INPUT:       destX/destY - The start x and y position to blit to the frontbuffer
/////			   destWidth/destHeight - The width and height of the desired DIB section
///// OUTPUT:      hdc - The front buffer hdc
///// RETURN:      nothing
/////
//////////////////////////////////////////////////////////////////////////

void PlasmaBurst(HDC hdc, int destX, int destY, int destWidth, int destHeight)
{
	// Make the flame 25 pixels high , which fades out to nothing
	for(int y = 0; y < 25; y++)
	{
		// Put a coal at the current line and x pos
		for(int x = 0; x < (destWidth * 3 - 3); x += 3)
		{
			MakeCoal(pImageBits, x, y, RValue, GValue, BValue);
		}
	}

	// Average the image with the plasma flame in it
	AverageImage(pImageBits, destWidth, destHeight / 2);

	// Blit the backbuffer to the front buffer (Voila!)
	BitBlt(hdc, destX, destY, destWidth, destHeight, hdc2, 0, 0, SRCCOPY);
}

																							
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			// Here is our "main()" for windows.  Must Have this for a windows app.
{														
	// Create the handle to the window.  
    HWND        hwnd;										
	// Create the variable to hold the window messages
    MSG         msg;	
	
	// Create the structure that holds the attributes of the window.
	// I just crammed them in like this to save space because we know what they do.
    WNDCLASSEX  wndclass = {sizeof (wndclass), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance,
						  LoadIcon (NULL, IDI_WINLOGO), LoadCursor (NULL, IDC_ARROW), (HBRUSH) GetStockObject (WHITE_BRUSH),
						  NULL, "Window Class", LoadIcon (NULL, IDI_WINLOGO)};
	

	// Register the window class with the operating system
	RegisterClassEx (&wndclass);	
	
	// Now, we actually create the window
    hwnd = CreateWindow ("Window Class",					// window class name 
						 "Bitmap Background",	  			// window's Title    
						 WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,						// window style	- This style won't allow the window to resize
						 CW_USEDEFAULT,						// initial x position
						 CW_USEDEFAULT,						// initial y position
						 WIDTH,								// Here we pass in our desired width (800)	 
						 HEIGHT,						    // Here we pass in our desired height (600)	 
						 NULL,								// This is the parent window handle.  
						 NULL,								// This is the window menu handle
						 hInstance,						    // This is the programs instance handle.
						 NULL);								// We don't want to pass any extra data in, so NULL

	// This shows our window. 
    ShowWindow (hwnd, iCmdShow);							

	// This pretty much paints our window to the screen.
    UpdateWindow (hwnd);									

	// Here is our main loop. 
	while (1)					
    {	
		// *We use PeekMessage() instead of GetMessage() to see if there is a message
		// from windows, if not, then we want to animate when nothing is going on.*

		// Check if there is a window message and remove it from the queue
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If the message was to quit, break out of the while loop
			if(msg.message == WM_QUIT) break;

			// This Translates messages so windows understands them.
			TranslateMessage (&msg);					
		
			// This sends the messages to the WndProc().
			DispatchMessage (&msg);							
		}
		else
		{
			// *Now we do the computationally expensive stuff in this else*

			// Check if we want to animate the next frame of animation
			if(AnimateNextFrame(FRAME_RATE))
			{
				// Calculate the frames per second of our application
				CalculateFrameRate(hwnd);				
				
				// Draw the fire
				DrawFire(gBuffer.hdcFront, FIRE_X, FIRE_Y, FIRE_WIDTH, FIRE_HEIGHT);			
			}
		}
    }

	// Unregister the window class with the operating system
	UnregisterClass("Window Class",hInstance);
    
	// Quit the program
	return msg.wParam ;										
}

// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Check which message was passed in
    switch (iMsg)											
    {
		// This message is sent when the window is created (CreateWindow())
		case WM_CREATE:

			// Create the double buffer and load the bitmaps	
			Init(hwnd);										
			break;	
	
		// If we click the right button increase the percentage of fire
		case WM_RBUTTONDOWN:
			gCoalPercentage += 5;
			break;

		// If we click the left button decrease the percentage of fire
		case WM_LBUTTONDOWN:
			gCoalPercentage -= 5;
			break;

		case WM_KEYDOWN:									// If a key is down
			switch (wParam) 
			{
				case VK_ESCAPE:
					// Close Window
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case 'R':
					// Increase the R value of the flame color
					RValue = ((RValue + 5) % 256);
					break;
				case 'G':
					// Increase the G value of the flame color
					GValue = ((GValue + 5) % 256);
					break;
				case 'B':
					// Increase the B value of the flame color
					BValue = ((BValue + 5) % 256);
					break;
				case VK_SPACE:
					// Do a plasma burst of fire and set the flame color to random
					bRandomColor = !bRandomColor;
					RValue = rand() % 256;
					GValue = rand() % 256;
					BValue = rand() % 256;
					PlasmaBurst(gBuffer.hdcFront, FIRE_X, FIRE_Y, FIRE_WIDTH, FIRE_HEIGHT);
					break;
				case VK_F1:
					// Do a plasma burst and reset the flame color
					RValue = 255;
					GValue = 255 / 2;
					BValue = 0;
					bRandomColor = false;
					PlasmaBurst(gBuffer.hdcFront, FIRE_X, FIRE_Y, FIRE_WIDTH, FIRE_HEIGHT);
					break;
				case VK_F2:
					// Increase the height of the flame (creates a torch effect)
					gFireIntensity = ((gFireIntensity+ 1) % 4);
					break;
				case VK_F3:
					// Bring the flame in again
					gCoalWidth = 3;
					break;
				case VK_F5:
					// Blow the flame to the right
					gDirection++;
					break;
				case VK_F6:
					// Blow the flame to the left
					gDirection--;
					break;
				case VK_F7:
					// Increase the coal height
					gCoalHeight++;
					break;
				case VK_F8:
					// Decrease the coal height
					gCoalHeight--;
					break;
			}
			break;											// DO nothing for now

		// This message is sent when the window is destroyed
		case WM_DESTROY:									

			// Free all the data and set everything back to normal, then post the quit message
			DeInit();										
			break;				
	}													

	// Process/handle the default messages
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		
															
}

void Init(HWND hwnd)
{	
	// Check if we want to go fullscreen or not
	if(MessageBox(hwnd, "Go to full screen?", "FullScreen", MB_YESNO) == IDYES)
		ChangeToFullScreen();

	// Create our double buffering
	CreateDoubleBuffering(&gBuffer, hwnd);

	// Set the backbuffer to white first (This clears the backbuffer)
	ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			

	InitFire(gBuffer.hdcFront, FIRE_X, FIRE_Y, FIRE_WIDTH, FIRE_HEIGHT);
}

void DeInit()
{
	// Cleanup the monochrome bitmaps
	SelectObject(hdc2, hOldBitmap);

	// Free GDI Resources
	DeleteObject(hBitmap);
	DeleteDC(hdc2);

	// Free the back buffer
	FreeBuffers(&gBuffer);

	ChangeDisplaySettings(NULL,0);

	// Post the QUIT message to the window (0 = WM_QUIT)															
	PostQuitMessage(0);				
}

////////////////////////////////////////////////////////////
//
// *Quick Notes*
//
// Fire!!!  It's amazing isn't it?  When you first look at
// graphic demo's like this you are generally intimidated.
// Really, once you understand what makes the fire, you can
// use the technique for TONS of other effects.  After making
// this, I made a winamp Visualization.  Hopefully I will write
// a tutorial on that as well.
//
// Here is how the fire is done:
// 
// 1)  First, you create a DIB section so we can get access to
//     some bits.  Now we have an array that we can store
//     and mess around with the colors.
// 
// 2)  Second, we draw coals (chucks of the color the fire) at
//     at the bottom of the screen.  It takes these random coals
//     and then slowly fades them out, while rising.
// 
// 3)  How do we "raise" these coals up and fade them?  Well, 
//     We start from the second line (y = 1, x = 0) and take the average
//     of all of the pixels around each pixel.  In other words, We start
//     with the R value... We get the R value of every pixel around the
//     pixel we are on and then take the average of them.  If we get
//     8 pixels around us (top, bottom, left, right, topleft, etc..)
//     Then we take all of those values added up and divide by 8.  Now
//     we have the average R value of the surrounding pixels.  Now that new
//     R value (the average) is our pixels NEW R value.  We need to do that
//     for the G and B too!  (Remember, DIB Sections order is BGR though).
//	   We need to do that to every pixel above the coals (or to the height of the fire)	
//     For Example:
// 
// 
//			Say we want to find the Value of Pixel "p" from pixels A - H
//			which are surrounding pixel "p"
//
//								
//					A	B	C
//			
//					D	p	E		
//	
//					F	G	H
//
//
//			So, now lets say that the RGB values for A - E are 0 0 0 (black)
//          BUT, F - H is orange (R = 255 , G = 128, B = 0).
//
//			Let's Find the new R value for the pixel "p".
//			To do this, we add up all the R values of A - H (not including p)
// 
//			avgR = (0 + 0 + 0 + 0 + 0 + 255 + 255 + 255) / 8
//			avgG = (0 + 0 + 0 + 0 + 0 + 128 + 128 + 128) / 8
//			avgB = (0 + 0 + 0 + 0 + 0 + 0 + 0 + 0) / 8
// 
//			Now we know that the new pixels RGB is going to be (95, 48, 0).
//			That is the new RGB value that we give to pixel "p".  Then, of course
//			we would move to pixel E.  and then the next one to the right of that...
//
//			Start at x = 0 and go until the width of the dib section, and then
//          increase y each time until you don't want to average the pixels anymore.
//          My fire only goes halfway up the screen and then stops.
//
//	That's it!  One quick tip though, it took me many hours to finally get find this
//  out.  Consider the line in AverageImage():
//
//	pImageBits[x + (((y + gFireIntensity) * 3) * (width + gDirection))]
//	which, without the mutations could be:
//  pImageBits[x + ((y + 1) * width)]
//
//	HINT: Make SURE you have at least y + 1.  If you don't have + 1 it won't
//  rise very well.  This is very important.  Try it without that and you will see why.
//  This took me a long time to figure out.  Without at least 1, it will be a pathetic fire.
//  If you increase 1 to 2, it will make a torch flame, as show in my demo (F2).
//
//	The more coals, the more it's going to rise because there will be more orange around
//  to average.  Right click and left click so you can see this.  Don't Clear the screen
//  Or else it won't work.  You need to leave the screen the same, it will eventually fade
//	out.  You will though, want to erase the coals on the bottom before you draw it.
//  
//  That's pretty much it for fire.  All you are doing is averaging the surrounding pixels
//  for every pixel.  You can do many awesome effects with this knowledge.
//
//	Good luck!  Ask if you need more help, I probably didn't explain it very well.
//  I will eventually hopefully get some graphic examples of the process, until then, just
//  email me with questions
//
// © 2000-2005 GameTutorials





