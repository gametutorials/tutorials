//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Font											 //
//																		 //
//		$Description:	Shows how to display 3D text to the screen		 //
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


// This tutorial branches off the last tutorial on fonts.  If you are not familiar
// with display lists, this concept is taught more in-depth in the previous tutorial 
// entitled "FONTS".  The difference from this tutorial and the last is that we will
// be able to create 3D fonts that can be rotated, translated and scaled, where as 
// before we had to just leave them at the front of the screen with no options but color.
// Depending on what you want a font for, you will want to use either one of these tutorials.

// This is an unsigned int (since we can't have a negative base pointer) that will hold
// the ID for our display list.  It isn't so much of an ID, but it's easier to think of it
// that way.  When we create a display list, we start at 1.  If we create another one we
// then move the list pointer to 2.  g_FontListID holds the BASE number for the display list.
// We are going to create 256 lists for all the characters, so we will start at one,
// and the list pointer will then be at 257 if you create another list.  When using display
// lists, we need the base pointer to tell OpenGL which list number we are starting at.
// This will make more sense in the functions below.
UINT g_FontListID = 0;

// This will save our old font and select it back in at the end of our program.
// We need this because we use SelectObject() to select in the new font.
// We don't want any memory leaks :)
HFONT hOldFont;		

// This define is for the amount of lists we want to create.  There will need to be 1
// for every character.  Since there are 256 ASCII characters, we will use 256.
// If we only wanted certain characters used, like alpha numerics, we would give less.
#define MAX_CHARS	256									

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This defines how much to extrude the font (how thick it is in 3D)
// Since we are doing 3D, the height parameter in CreateFont() doesn't mean jack now.
#define FONT_EXTRUDE	0.4f

// This will hold info about each 3D character (width, height, etc...).
// We can use this information later for formating the text.
GLYPHMETRICSFLOAT g_GlyphInfo[MAX_CHARS];		


///////////////////////////////// CREATE OPENGL 3D FONT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a 3D windows font like "Arial", and returns a display list ID
/////
///////////////////////////////// CREATE OPENGL 3D FONT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

UINT CreateOpenGL3DFont(LPSTR strFontName, float extrude)	
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
	// In the last tutorial we passed in a height, now we pass in a extrude value.  This
	// value basically makes the thickness in 3D.  The height parameter in CreateFont() doesn't
	// matter now, so it doesn't really matter what value we put in, it is just ignored by wglUseFontOutlines().

	hFont = CreateFont(	0,								// This HEIGHT parameter is ignored by wglUseFontOutlines(). Use glScalef()
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

	// This is the function that creates all the magic.  This will actually output the
	// polygons into our display lists, which creates the outlined characters.
	// But first, we need to give it our desired information.
	// One new thing from the last tutorial is GLYPH's.  A glyph is a 3D character.
	// The GLYPHMETRICSFLOAT structure used holds information about that 3D character,
	// such as it's width and height in 3D units, as well as orientation within it's bounding box.
	// We can use this information if we want to find out how long our sentences are in actual
	// unit lengths, not pixel length.
	wglUseFontOutlines(	g_hDC,							// This is the global HDC with the desired font selected.
						0,								// This is the starting ASCII value.
						MAX_CHARS - 1,					// This is the ending ASCII value to use (255 is the last).
						fontListID,						// This is the base pointer of our display list we wish to use.
						0,								// This is the deviation from a true outline (floating point)
						extrude,						// This is the extrude value, or in other words, the thickness in Z.
						WGL_FONT_POLYGONS,				// We specify here that we want it rendered in polygons rather than lines
						g_GlyphInfo);					// The address to the buffer that will hold the 3D font info for each character.

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	return fontListID;									// Return the ID to the display list to use later
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// GL DRAW 3D TEXT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws 3D text onto the screen using OpenGL.
/////
///////////////////////////////// GL DRAW 3D TEXT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void glDraw3DText(const char *strString, ...)
{
	char		strText[256];							// This will hold our text to display
	va_list		argumentPtr;							// This will hold the pointer to the argument list
	float		unitLength=0.0f;						// This will store the length of the 3D Font in unit length

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

	// Below we find out the total length of the characters in 3D units, then center them.

	for (int i=0; i < (int)strlen(strText); i++)		// Go through all of the characters
	{
		// Here we go through each character and add up it's length.
		// It doesn't matter where the text if facing in 3D, it uses a 2D system
		// for the width and height of the characters.  So X is the width, Y is the height.
		unitLength += g_GlyphInfo[strText[i]].gmfCellIncX;	
	}

	// Here we translate the text centered around the XYZ according to it's width.
	// Since we have the length, we can just divide it by 2, then subtract that from the X.
	// This will then center the text at that position, in the way of width.
	// Can you see how to center the height too if you wanted? (hint: .gmfCellIncY)
	glTranslatef(0.0f - (unitLength / 2), 0.0f, 0.0f);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Now, before we set the list base, we need to save off the current one.
	glPushAttrib(GL_LIST_BIT);							// This saves the list base information

	// Then we want to set the list base to the font's list base, which should be 1 in our case.
	// That way when we call our display list it will start from the font's lists'.
	glListBase(g_FontListID);							// This sets the lists base

	// Now comes the actually rendering.  We pass in the length of the string,
	// then the data types (which are characters so its a UINT), then the actually char array.
	// This will then take the ASCII value of each character and associate it with a bitmap.
	glCallLists((int)strlen(strText), GL_UNSIGNED_BYTE, strText);

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

	// Just like in the Bitmap Font we created in the last font tutorial, we need
	// to initialize it here.  Instead of a height value, we pass in a 3D extrude value
	// to make the text as thick along the Z axis as we want.  I chose "Impact" as the font.
	// If nothing shows up, try using a font like "Arial".  I believe most computers have "Impact".

	g_FontListID = CreateOpenGL3DFont("Impact", FONT_EXTRUDE);

	// Because we are doing 3D, we need to enable depth testing.  This allows it so
	// the polygons are drawn in the order they are seen.

	glEnable(GL_DEPTH_TEST);							// Enable depth testing 

	// I am not sure how you would get the normals of each polygon of the text,
	// but if we at least turn on a light and enable lighting, it will roughly
	// calculate that stuff for us, though it won't be perfect.  

	glEnable(GL_LIGHT0);								// Enable Default Light (Quick And Dirty)
	glEnable(GL_LIGHTING);								// Enable Lighting

	// Once we turn on lighting, we have to enable color material or else it won't
	// color our objects.  You can change many properties of that color in regards to
	// the light with glColorMaterial().

	glEnable(GL_COLOR_MATERIAL);						// Enable Coloring Of Material

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


}


/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function clamps a specified frame rate for consistency
/////
/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static double lastTime = 0.0f;
	double elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
	double currentTime = GetTickCount() * 0.001; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
	if( elapsedTime > (1.0 / desiredFrameRate) )
	{
		// Reset the last time
		lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop
/////
///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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
			if(AnimateNextFrame(60))					// Make sure we only animate 60 FPS
			{
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// GL ROTATE FOREVER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function takes an angle and rotates along the given axis
/////
///////////////////////////////// GL ROTATE FOREVER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void glRotateForever(float angle, float x, float y, float z)
{
	static float rotation = 0.0f;						// Create and init a static variable

	// This function is designed to increase every frame to produce a continuous rotation.
	// Other objects can use this, but they can not change the rotation.  A more useful
	// function would allow you to pass in a reference to a float.

	rotation += angle;									// Every frame increase the rotation

	glRotatef(rotation, x, y, z);						// Rotate along the given axis
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



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

	// Below we create 3 spinning objects, each spinning along it's own axis.

	glColor3f(0, 1, 0);									// Make the 3D text green

	// This next object will be green, in the top left corner.

	glPushMatrix();										// Create a matrix scope to not effect the rest

		// We create an illusion that the text is smaller, but really we are just moving
		// it back along the Z-Axis.  We would want to use glScalef() instead for that.

		glTranslatef(-6, 5, -15);						// Move the first object to the top left and back

		// We create a rotate function that spins each object at the same speed continually.
		// A more useful function would be to have a reference to a variables passed in
		// that would increase and rotate.

		glRotateForever(0.8f, 1.0f, 0.0f, 0.0f);		// Rotate the text along the X axis continually

		// If we just pass a string into our function, it will center that string
		// around the origin, unless we changed the matrix beforehand with glTranslatef().
		
		glDraw3DText("www.GameTutorials.com");			// Now we just call our text function and pass in a string.

	glPopMatrix();										// End the scope for this matrix

	// This next object will be yellow, in the middle of the screen, and a bit larger.

	glColor3f(1, 1, 0);									// Make the next 3D text yellow

	glPushMatrix();										// Create a new matrix scope

		glTranslatef(0, 0, -5);							// Move the text back a bit to make it smaller

		// Since we already called this function, we don't need to put in a new angle.
		// It will still use the same angle as before if we pass in zero.  But we want
		// to rotate around the Y-Axis this time.

		glRotateForever(0.0f, 0.0f, 1.0f, 0.0f);		// Rotate the text along the Y axis continually

		glDraw3DText("www.GameTutorials.com");			// Draw the large text object in the middle.

	glPopMatrix();										// End this matrix scope

	glColor3f(0, 1, 1);									// Make the 3D text purple

	// This next object will be purple, in the bottom right corner.

	glPushMatrix();										// Start a new matrix scope

		glTranslatef(5, -5, -15);						// Translate back, down and to the right.

		// Like the last object, we pass in zero for the angle because we don't
		// want to increase it until the next frame.  That way it will make a continuous rotation.

		glRotateForever(0.0f, 0.0f, 0.0f, 1.0f);		// Rotate the text along the Z axis continually

		glDraw3DText("www.GameTutorials.com");			// Draw the text in the bottom right corner

	glPopMatrix();										// End this matrix scope

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
        return DefWindowProc (hWnd, uMsg, wParam, lParam); 
    } 
 
    return 0;											// If we handle the message return 0
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Now we can actually have cool looking 3D text!  I think I might have over complicated
// this tutorial, but I feel that you should know as much as possible when it comes to
// such a magical subject as 3D text.  A lot of the work is done for you, so you don't
// also know what is left for you to do.  In the last tutorial you could just display
// text in 2D, but now you can actually work with a 3D object.  In 3D studio max and other
// software packages, you can create 3D text with fonts.  That means you are one step
// closer to making your own 3D software :-)
// 
// An easy way to make a tunnel would be to make a font with a huge extrude value
// and then print out a 'O'.  Either that or do a quadric cylinder.
//
// The only thing really different from Bitmap Fonts and 3D fonts is:
// 
// wglUseFontOutlines() verses wglUseFontBitmaps()
// 
// Most of the rest of the stuff is the same, except you position the differently.
// With bitmaps you use the glRasterPos*() functions, and with 3D objects you position
// them normally, like every other object with glTranslatef().
//
// I would like once again to thank Nehe (nehe.gamedev.net) for the ideas.  
// I hope this tutorial helped someone out.  Let us know if you ever use it.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
