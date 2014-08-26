//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		ScreenBuffer									 //
//																		 //
//		$Description:	Directly Displays color text to the screen buffer//
//																		 //
//***********************************************************************//


// This is a very important lesson on understanding how to control the console
// window (the DOS window) by creating an array out of the entire screen for
// every character position, and being able to manipulate it.  This is like
// controlling the pixels for the screen when doing 2D graphics, but in DOS
// we just control characters, not individual pixels.  We want to do this when
// we want to place a string of characters at a specific position on the screen,
// not just following the cursor position when using cout.


#include <windows.h>									// We need to include this for "console" functions.

#define GRID_WIDTH  15									// This is the width of our grid that we will display to the screen
#define GRID_HEIGHT 10									// This is the height of our grid that we will display to the screen
#define POS_X       25									// This is the starting positive we will draw for X
#define POS_Y       2									// This is the starting positive we will draw for Y
	
void main()												// Here is our main().
{
	CHAR_INFO screenBuffer[GRID_WIDTH * GRID_HEIGHT];	// This is our buffer that will hold all the data we wish to display to the screen.
														// This is a data type that holds a rectangle.  This will be the drawing rectangle to the screen.
	SMALL_RECT drawRect = {POS_X, POS_Y, POS_X + (GRID_WIDTH - 1), POS_Y + (GRID_HEIGHT - 1)}; 
	COORD gridSize = {GRID_WIDTH , GRID_HEIGHT};		// This is a COORD that holds the width and height of our grid that we will display
	COORD zeroZero = {0, 0};							// This tells us the upper left corner to write from
	HANDLE OutputH;										// This holds our output handle to the screen
	int x = 0, y = 0;									// This will be used to index our 1D array as a 2D array
	
	// I will go into a bit more detail about what the variables above do:
	// screenBuffer - You notice that we make an array of CHAR_INFO's.  CHAR_INFO is a structure that holds a color and character.
	//				  the size of that array is the width times the height.
	//                This is basically like the area equation of a rectangle (length * width).
	//                That is how many characters we are going to display each time.
	//                In this case, we are display 15 * 10 which is 150.  So it's the same as screenBuffer[150].
	//                We could make a double array like this: screenBuffer[GRID_WIDTH][GRID_HEIGHT], but
	//                unfortunately, the function WriteConsoleOutput() takes it as a SINGLE array, so we must do it this way.
	//                Below we will learn how to treat a single array like a double array.
	// drawRect - This is a rectangle that holds a {Left, Top, Right, Bottom}.  This variable
	//            is used to tell WriteConsoleOutput() the coordinates where we want to display our buffer.
	//            We feed it the starting position, then we add the width and height to that starting position.
	//            We minus 1 from the width and height because it is zero based.  It might error on other operating system if we didn't.
	//            It would be going outside the array index by 1, so we need to minus one.  Remember the screen starts at (0, 0).
	// gridSize - This just tells WriteConsoleOutput() the width and height of our buffer.  We don't need to minus
	//            1 from the width and height because this isn't used in an array, this just gives width and height values.
	// zeroZero - This just lets WriteConsole() know the top left corner that we wish to draw from.  Just keep this at (0, 0).
	//            You would think that you should change that value to {POS_X, POS_Y}, but strangly it doesn't work that way.
	// OutputH - This holds the handle to the screen.  We need this to gain access to draw to the screen.

	OutputH = GetStdHandle(STD_OUTPUT_HANDLE);			// Get a OUTPUT handle to our screen.
	
	// Below we fill in the buffer.  To fill in a 2 dimensional array, we use 2 for loops.
	// The first for loop is the y position, where the inner for loop is the x position into
	// the buffer.  If you think about what's happening, you will notice that is fills this
	// in row by row.  It will do this:  *************** , then go to the next line and do the same thing.

	for(y = 0; y < gridSize.Y; y++)						// This fills in the columns (y value) of our buffer
	{
		for(x = 0; x < gridSize.X; x++)					// This fills in the rows (x value) of our buffer
		{												// Below, we fill in the current index with a character and color.
			// I will explain what the x + y * WIDTH is doing...  Since we aren't using a 2D
			// array, we have to treat out 1D array like a 2D array.  To do this, there is a
			// equation :  x + (y * width)    - Run through a small loop in your head to understand
			// why this works.  Think about it, y will be zero in the beginning so 0 * width is 0.
			// The first row will be in the 0 index of y.  Next, y will be 1, so 1 * width = the width.
			// If we take x and add the width to it, we are on the next row right?  Well, it's all a huge line
			// in memory, but WriteConsoleOutput() treats it as a 2D array, that is why we pass in a width and height.

			// Now that we have the desired index, we need to set the character and color.
			// To set the color, we need to access a structure named "Char", inside of the CHAR_INFO structure.
			// Inside of the "Char" structure, there is 2 options, either a unicode variable or a AsciiChar variable.
			// We want the AsciiChar variable.  Then we just set that variable to the desired character we want.
			screenBuffer[x + y * GRID_WIDTH].Char.AsciiChar = '*';

			// To set the color, we use the same index into the screenBuffer, but access the Attributes variable.
			// We set this variable to the desired color we want for this index and character.
			// Remember, we can do stuff like:  FOREGROUND_GREEN | BACKGROUND_RED;
			// That will make the background red and text color green.  
			screenBuffer[x + y * GRID_WIDTH].Attributes = FOREGROUND_GREEN;
		}
	}

	// Now that we have finished initializing the array, we want to draw or "blit" this data
	// to the screen.  The cool thing about this is that it is fast.  It does it in one swoop,
	// rather than doing it character by character.  Here is the parameters for WriteConsoleOutput():
	// (The output handle, the screen buffer to draw, the width and height, top left corner, the draw rectangle).
	// We pass the address of drawRect because WriteConsoleOutput() fills it in with the rectangle that
	// was actually drawn.  This is a good way to error check to see if it drew to the right place.

	WriteConsoleOutput(OutputH, screenBuffer, gridSize, zeroZero, &drawRect);

	// Here we add 5 to the X and Y location of the draw rectangle.  This is so we can draw another
	// buffer layered over top of the last one we drew.  It's kinda like drawing a bitmap :)

	drawRect.Top += 5;									// Increase the top part of the rectangle by 5	
	drawRect.Left += 5;									// Increase the left part of the rectangle by 5
	drawRect.Right += 5;								// Increase the right part of the rectangle by 5
	drawRect.Bottom += 5;								// Increase the bottom part of the rectangle by 5

	// Once again, fill in the screen buffer with new characters and values.

	for(y = 0; y < gridSize.Y; y++)						// This fills in the columns
	{
		for(x = 0; x < gridSize.X; x++)					// This fills in the rows
		{
			// You will notice this time we just say "1" for the character.
			// Well, this is because the ASCII character 1 is for the smiley face :)
			// Every character has a value associated with it (0 through 255).

			screenBuffer[x + y * GRID_WIDTH].Char.AsciiChar = 1;

			// Here we make the text color BLUE and the background color white.
			// If we | all the background colors together it becomes white.  Cool huh?
			// It's like in paint programs, the RGB value (255, 255, 255) is white.

			screenBuffer[x + y * GRID_WIDTH].Attributes = FOREGROUND_BLUE | (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
		}
	}

	// Use the same code as above to draw the smiley faces to the new location (Down and to the right by 5)
	WriteConsoleOutput(OutputH, screenBuffer, gridSize, zeroZero, &drawRect);

	drawRect.Top += 5;									// Increase the top part of the rectangle by 5	
	drawRect.Left += 5;									// Increase the left part of the rectangle by 5
	drawRect.Right += 5;								// Increase the right part of the rectangle by 5
	drawRect.Bottom += 5;								// Increase the bottom part of the rectangle by 5

	// Once again, fill in the screen buffer with new characters and values.

	for(y = 0; y < gridSize.Y; y++)						// This fills in the columns
	{
		for(x = 0; x < gridSize.X; x++)					// This fills in the rows
		{
			// The ASCII value of 3 gives us the heart character :)
			screenBuffer[x + y * GRID_WIDTH].Char.AsciiChar = 3;

			// We want a black background with red colored text
			screenBuffer[x + y * GRID_WIDTH].Attributes = FOREGROUND_RED;
		}
	}

	// Finally, draw the hearts to the screen, producing a card-like layer effect.
	WriteConsoleOutput(OutputH, screenBuffer, gridSize, zeroZero, &drawRect);


	// This gives us a delay for a second so we don't see the "press any key to continue" at first.
	Sleep(1000);
}														// End of the program


/////////////////////////////////////////////////////////////////////////////////////////
//
//	*Quick Notes*
//
//  This tutorial is designed to prepare you for working with graphics.  In a way,
//  this is kinda like displaying bitmaps.  It's the same principle.  You fill in
//  a buffer with data (colors AND text in this case...) and then display it to the
//  screen at an X and Y location.  If you understand this concept, you can display
//  bitmaps to the screen.
//
//  The tough parts to get past are the 2D array with a 1D array syntax, and understanding
//  how to USE this console stuff.  It is really picky, so if you don't do it exactly how it
//  wants, it won't display anything and it's hard sometimes to tell why.  Other times it
//  displays, but it doesn't display all of it.  You really have to make sure your drawRect
//  and gridSize are the appropriate numbers, also your drawing position is set to (0, 0) (zerZero).
//
//	You will most likely get really frustrated trying to work with this stuff, so feel free
//  to email us at gametutorials, or use the message boards to ask questions.
//
//	In the next tutorial we will discuss how to make a maze using WriteConsoleOutput().  
//	Then we will put a character that can walk around and collide with the walls.
//
//
//  © 2000-2005 GameTutorials
