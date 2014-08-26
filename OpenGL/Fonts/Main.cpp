//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Fonts											 //
//																		 //
//		$Description:	Shows how to display text to the screen			 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// We are going to learn a couple new things in this tutorial.
// 1) What display list's are
// 2) How to create and setup a handle to a font
// 
// OpenGL has some functions that make using fonts extremely easy.
// There are many ways to do fonts, but one of the easiest ways to do it
// comes from NeHe's tutorials (nehe.gamedev.net).  We can create a display
// list holding every character, and then pass in that list to an OpenGL function
// called wglUseFontBitmaps that will do the work for us.  What is a display list you ask?
// A display list is used to speed up your program and cutting down your code by storing
// OpenGL objects/polygons/primitives which you can then access by a number.  For example,
// If you have the same object in your world a bunch of times, there is no need to pass in
// the vertices to OpenGL every time you want to render an instance of that object, you can just
// tell OpenGL about it, which stores it in a list, then you can just call that list.  This
// is faster than drawing the same thing 30 times because you only have to give OpenGL the
// vertices once.  Say you wanted to render 30 models of bad guys.  You just need to make
// a display list for the model, and then call that list by it's ID to render another.
// That is what we are doing with the font.  We just create a list that holds every character
// in the alphabet that we would use and then just access that character by it's ASCII value later.
// It will make sense as we go along.  Some of the font API's are just for windows.  You might
// need to do it the old fashion way by loading in parts of a bitmap if you aren't using windows.

// This is an unsigned int (since we can't have a negative base pointer) that will hold
// the ID for our display list.  It isn't so much of an ID, but it's easier to think of it
// that way.  We we create a display list, we start at 1.  If we create another one we
// then move the list pointer to 2.  g_FontListID holds the BASE number for the display list.
// We are going to create 256 lists for all the characters we need, so we will start at one,
// and the list pointer will then be at 257 if you create another list.  When using display
// lists, we need the base pointer to tell OpenGL which list number we are starting at.
// This will make more sense in the function below.
UINT g_FontListID = 0;

// This will save our old font and select it back in at the end of our program.
// We need this because we use SelectObject() to select in the new font.
// We don't want any memory leaks :)
HFONT hOldFont;

// This define is for the amount of lists we want to create.  There will need to be 1
// for every character.  Since there are 256 ascii characters, we will use 256.
// If we only wanted certain characters used, like alpha numerics, we would give less.
#define MAX_CHARS	256									

// This defines how high we want our font to be
#define FONT_HEIGHT	32

///////////////////////////////// CREATE OPENGL FONT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a windows font like "Arial" and returns a display list ID
/////
///////////////////////////////// CREATE OPENGL FONT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

UINT CreateOpenGLFont(LPSTR strFontName, int height)	// Build Our Bitmap Font
{
	UINT	fontListID = 0;								// This will hold the base ID for our display list
	HFONT	hFont;										// This will store the handle to our font

	// Here we generate the lists for each character we want to use.
	// This function then returns the base pointer, which will be 1 because
	// we haven't created any other lists.  If we generated another list after
	// this, the base pointer would be at 257 since the last one used was 256 (which is MAX_CHARS)
	fontListID = glGenLists(MAX_CHARS);					// Generate the list for the font

	// Now we actually need to create the font.  We use a windows function called:
	// CreateFont() that returns a handle to a font (HFONT).  Our CreateOpenGLFont()
	// function allows us to pass in a name and height.  For simplistic reasons, I left
	// other options out, but feel free to add them to your function (like bold, italic, width..)

	hFont = CreateFont(	height,							// Our desired HEIGHT of the font
						0,								// The WIDTH (If we leave this zero it will pick the best width depending on the height)
						0,								// The angle of escapement
						0,								// The angle of orientation
						FW_BOLD,						// The font's weight (We want it bold)
						FALSE,							// Italic - We don't want italic
						FALSE,							// Underline - We don't want it underlined
						FALSE,							// Strikeout - We don't want it strikethrough
						ANSI_CHARSET,					// This is the type of character set
						OUT_TT_PRECIS,					// The Output Precision
						CLIP_DEFAULT_PRECIS,			// The Clipping Precision
						ANTIALIASED_QUALITY,			// The quality of the font - We want anitaliased fonts
						FF_DONTCARE|DEFAULT_PITCH,		// The family and pitch of the font.  We don't care.
						strFontName);					// The font name (Like "Arial", "Courier", etc...)

	// Now that we have created a new font, we need to select that font into our global HDC.
	// We store the old font so we can select it back in when we are done to avoid memory leaks.
	hOldFont = (HFONT)SelectObject(g_hDC, hFont);

	// This function does the magic.  It takes the current font selected in
	// the hdc and makes bitmaps out of each character.  These are called glyphs.
	// The first parameter is the HDC that holds the font to be used.
	// The second parameters is the ASCII value to start from, which is zero in our case.
	// The third parameters is the ASCII value to end on (255 is the last of the ASCII values so we minus 1 from MAX_CHARS)
	// The last parameter is the base pointer for the display lists being used.  This should be 1.

	wglUseFontBitmaps(g_hDC, 0, MAX_CHARS - 1, fontListID);	// Builds 255 bitmap characters

	return fontListID;									// Return the ID to the display list to use later
}

///////////////////////////////// POSITION TEXT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the drawing position for the text
/////
///////////////////////////////// POSITION TEXT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void PositionText( int x, int y )
{
	// If you are to use this font code for your applications,
	// you must be aware that you cannot position the font in 3D,
	// which means you can't rotate and scale it.  That will be covered in
	// the next font tutorial.  BUT, though that might be a drag, this code
	// is useful because when you display the text, it will always be on top
	// of everything else.  This is good if the camera is moving around, and you
	// don't want the text to move.  If the text was positioned in 3D you would have
	// to come up with a tricky way of making it always render in front of the camera.
	// To do this, we need to set the Raster Position.  That is the position that OpenGL
	// starts drawing at.  Since it's in floating point, it's not very intuitive, so what
	// we do is create a new view port, and then always draw the text at (0, 0, 0) in that
	// view port.  The weird part is that the Y is flipped, so (0, 0) is the bottom left corner.
	// Below we do some simple math to flip it back to normal.

	// Before we create a new view port, we need to save the current one we have.
	// This saves our transform (matrix) information and our current viewport information.
	// At the end of this function we POP it back.
	glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

	// Here we use a new projection and modelview matrix to work with.
	glMatrixMode( GL_PROJECTION );						// Set our matrix to our projection matrix
	glPushMatrix();										// Push on a new matrix to work with
	glLoadIdentity();									// reset the matrix
	glMatrixMode( GL_MODELVIEW );						// Set our matrix to our model view matrix
	glPushMatrix();										// Push on a new matrix to work with
	glLoadIdentity();									// Reset that matrix

	// Because the Y is flipped, we want 0 to be at the top, not bottom.
	// If we subtract the font height from the screen height, that should display the
	// font at the top of the screen (if they passed in 0 for Y), but then we subtract
	// the Y from that to get the desired position.  Since the font's drawing point is
	// at the base line of the font, we needed to subtract the font height to make sure
	// if they passed in (0, 0) it wouldn't be off screen.  If you view this in window mode,
	// the top of the window will cut off part of the font, but in full screen it works fine.
	// You just need to add about 25 to the Y to fix that for window mode.

	y = SCREEN_HEIGHT - FONT_HEIGHT - y;				// Calculate the weird screen position

	// Now we create another view port (that is why we saved the old one above).
	// Since glViewPort takes the lower LEFT corner, we needed to change the Y
	// to make it more intuitive when using PositionText().  We minus 1 from the X and Y
	// because 0 is taken into account with the position.  The next 2 parameters are set
	// to 0 for the width and height so it will always draw in the middle of that position.
	// glRasterPos4f() takes (0, 0, 0) as the middle of the viewport, so if we give it a small
	// width/height it will draw at the X and Y given.  Sounds strange, to test this, try
	// using glRasterPos4f(0, 0, 0, 1) instead of PositionText() and you will see, everything
	// will be drawn from the middle.

	glViewport( x - 1, y - 1, 0, 0 );					// Create a new viewport to draw into

	// This is the most important function in here.  This actually positions the text.
	// The parameters are (x, y, z, w).  w should always be 1 , it's a clip coordinate.
	// don't worry about that though.  Because we set the projection and modelview matrix
	// back to the beginning (through LoadIdentity()), the view port is looking at (0, 0, 0).
	// This is the middle, so if we set the drawing position to the middle, it will draw at our
	// X and Y because the width/height of the viewport is 0, starting at X and Y.
	// You can actually call this function (or glRasterPos2f(0, 0)) instead of PositionText(),
	// but it is in floating point and doesn't work as nicely.  You will see why if you try.

	glRasterPos4f( 0, 0, 0, 1 );						// Set the drawing position

	// Now that we positioned the raster position, any text we draw afterwards will start
	// from that position.  Now we just have to put everything else back to normal.

	glPopMatrix();										// Pop the current modelview matrix off the stack
	glMatrixMode( GL_PROJECTION );						// Go back into projection mode
	glPopMatrix();										// Pop the projection matrix off the stack

	glPopAttrib();										// This restores our TRANSFORM and VIEWPORT attributes
}


///////////////////////////////// GL DRAW TEXT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws 2D text onto the screen using OpenGL, given an X and Y position 
/////
///////////////////////////////// GL DRAW TEXT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void glDrawText(int x, int y, const char *strString, ...)
{
	char		strText[256];							// This will hold our text to display
	va_list		argumentPtr;							// This will hold the pointer to the argument list

	// If you have never used a va_list, listen up.  Remember printf()?
	// or sprintf()?  Well, you can add unlimited arguments into the text like:
	// printf("My name is %s and I am %d years old!", strName, age);
	// Well, that is what va_list's do.  

	// First we need to check if there was even a string given
	if (strString == NULL)								// Check if a string was given
		return;											// Don't render anything then

	// First we need to parse the string for arguments given
	// To do this we pass in a va_list variable that is a pointer to the list of arguments.
	// Then we pass in the string that holds all of those arguments.
	va_start(argumentPtr, strString);					// Parse the arguments out of the string

	// Then we use a special version of sprintf() that takes a pointer to the argument list.
	// This then does the normal sprintf() functionality.
	vsprintf(strText, strString, argumentPtr);			// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	// Before we draw the text, we need to position it with our own function.
	PositionText(x, y);									// Call our own function to position the text on screen

	// Now, before we set the list base, we need to save off the current one.
	glPushAttrib(GL_LIST_BIT);							// This saves the list base information

	// Then we want to set the list base to the font's list base, which should be 1 in our case.
	// That way when we call our display list it will start from the font's lists'.
	glListBase(g_FontListID);							// This sets the lists base

	// Now comes the actually rendering.  We pass in the length of the string,
	// then the data types (which are characters so its a UINT), then the actually char array.
	// This will then take the ASCII value of each character and associate it with a bitmap.
	glCallLists(strlen(strText), GL_UNSIGNED_BYTE, strText);

	glPopAttrib();										// Return the display list back to it's previous state
}


///////////////////////////////// DESTROY FONT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function frees our display lists
/////
///////////////////////////////// DESTROY FONT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DestroyFont()										
{
	glDeleteLists(g_FontListID, MAX_CHARS);				// Free the display list
	SelectObject(g_hDC, hOldFont);						// Select the old font back in so we don't have memory leaks
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// After we initialize OpenGL, we want to create our font.  I chose "Arial".
	// We also can pass in a font height.  The width will be chosen according to the height.
	// This then returns the base pointer to the display list for our font.  (Should be 1)
	// We need to be sure and use this to free our display list in DestroyFont().

	g_FontListID = CreateOpenGLFont("Arial", FONT_HEIGHT);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

}

///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function Handles the main game loop
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											// if there wasn't a message
		{ 
			// Render the scene every frame to update the rotating cube
			RenderScene();								
       } 
	}

	DestroyFont();										// This frees up our font display list									
	DeInit();											// Release memory and restore settings

	return(msg.wParam);									// Return from the program
}

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	glColor3f(0, 1, 0);									// Make the next text green

	// Here we draw our text at (0, 25).  This text will be green.

	glDrawText(0, 25, "Brought to you by www.GameTutorials.com...");

	glColor3f(1, 1, 0);									// Make the next text yellow

	// Now we want to move the next text down, so we use (25 + FONT_HEIGHT * n) for the Y
	glDrawText(0, 25 + FONT_HEIGHT * 2, "abcdefghijklmnopqrstuvwxyz");
	glDrawText(0, 25 + FONT_HEIGHT * 3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	// You will notice in this one, that I added some arguments that take integers.
	// You can see how you can treat this function like printf()/sprintf()
	glDrawText(0, 25 + FONT_HEIGHT * 4, "%d%d!@#$%^&*(){}[]..,/?><", 12345, 67890);

	glColor3f(1, 0, 1);									// Make the next text purple

	glDrawText(0, 25 + FONT_HEIGHT * 6, "AREN'T FONTS EASY?!"); // They sure are!

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_KEYDOWN:
		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial covers how to use display lists and more importantly, how to display
// text to the screen in OpenGL.  The next font tutorial will allow you to display 3D text.
// Then you can rotate and scale it to your hearts delight.  
// 
// Remember though, when using this 2D text, it will always be in front of the screen.
// This could be good and bad, depending on what you are doing.  You can use the code
// for PositionText() to draw bitmaps the same way, so they will always be in front.
// To do that, you would use glDrawPixels().  This might be discussed later in another tutorial.
// 
// If you are uncomfortable with all the funky code in PositionText(), you can just use
// glRasterPos2f().  This takes an X and Y position the same way, but it's in floating point.
// If you said glRasterPos2f(0, 0) it wouldn't put the text in the top left corner of the screen,
// it would put it in the middle of the screen, depending on some other things.... 
// It isn't really intuitive and you have to do a lot of guessing, so go ahead and see what
// you like better.
// 
// Let's go over some of the basics of what we did in this tutorial.
// 
// 1) First, we needed to create a display list for each character.  To do that
//    we used:
// 
//			glGenLists( /* Max Characters */ );
// 
//	  This returns a base pointer to use when referencing this group of display lists.
//
// 2) Then we needed to create a font.  To create a font in windows we used:
//			
//			CreateFont( /*Huge list of arguments for the font */ );
//
//	  This function returns a handle to a font (HFONT).  We then need to select
//    that into our global HDC.
// 
// 3) Now that we have a display lists saved and a font selected, we can fill in the lists.
//
//			wglUseFontBitmaps( /* hdc, starting list pointer, max lists, base pointer */ );
//
//	  This function above actually creates the bitmaps from the font and stores them in the lists.
//
// 4) Now that we created our display lists and filled them in with the font, we actually want
//    to display them.  We uses a va_list with stands for "Variable Argument List".  This allows
//    us to give our glDisplayText() function arguments, like printf()/sprintf() do.  This is
//    not an OpenGL thing.  
//
// 5) After we extract the string we want to display using va_start(), vsprintf() and va_end(),
//    we then want to position the text on the screen.  This was a hairy weird part.  Basically,
//    what we did was create another viewport, and give it a width and height of 0.  This was
//    we could set the RasterPosition to (0, 0, 0, 1) and it would start the text at that X Y given.
//    Read above for more information on this, it's too long to explain again.
//
// 6) Once the text is positioned, we want to actually draw it!  First we save the list state:
//
//			glPushAttrib(GL_LIST_BIT);		// This saves the list base information
//
//			glListBase(g_FontListID);		// Then we set the list base information
//
//			// Then we actually draw the text, with each character's ASCII value referencing the associated bitmap
//
//			glCallLists( /* number of lists,  type,  array holding list number */ );
//
//			glPopAttrib();					// Then we set the list base back to what it was
//
// 7) We can also change the color of the text by using any GL color function like glColor3f();
//
// 8) Once we are finished, we need to clean up.  We do this calling:
//
//			glDeleteLists( /* base list,  number of lists */ );
//
//    Then we need to select the old font back into our HDC as to avoid memory leaks.
//
// That is pretty much it in a nutshell!  If you don't understand what is going on,
// try changing some things around, and hopefully it will hit you after much depression :)
// You might even just want to copy the code if you aren't interested in how it works.
// The next tutorial on text will be 3D text and won't be so boring.  Let me know if this helped!
//
// I want to thank NeHe (nehe.gamedev.net) for the idea and help!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
