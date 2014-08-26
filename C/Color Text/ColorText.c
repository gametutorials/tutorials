//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		ColorText										 //
//																		 //
//		$Description:	Displays Color Text in a Console Window.		 //
//																		 //
//***********************************************************************//


// In this tutorial we will show you how to draw text to the screen that has
// different colors for the foreground and background.  Black and white text
// can be boring so we will spice it up with color.  Once you get color text
// down you can start making an ASCII colored game.  Check out our ASCII RPG
// tutorial series eventually to see how to use color and to make a game without
// bitmap graphics.
//
// So, do create color text all we need to do is get a handle to our screen,
// then set the text attribute with our desired color.  An output handle is 
// basically a "key" to the screen that allows us to set and get information 
// and attributes of the user's screen.
//
// Now let's go below and see what is going on.  We created a function that
// allows us to pass in a string, a position and a color to print text.


#include <windows.h>									// We need to include this for "console" functions.
#include <stdio.h>										// Our basic set of functions.

														// Here is our function that draws a color string in a DOS window.
														// We just pass in the string, the X and Y position, then the color we desire.
														// The color can be a background color AND a foreground color.  Here are some color examples:
														// FOREGROUND_GREEN; FOREGROUND_RED; FOREGROUND_BLUE; - BACKGROUND_RED; BACKGROUND_BLUE; BACKGROUND_GREEN;
														// You can mix and match these to create other colors.  Look in "Main()" to see how.
void DrawColorString(char *szText, int X, int Y, WORD color)
{
	COORD  screenPos={X, Y};							// Put the X and Y position in a structure.
	WORD   *colorBuf=NULL;								// Create a pointer to a buffer that holds the color for each character.
    DWORD  dwResult=0;									// This holds the amount of characters drawn to the screen. (We don't care about this though...)
	HANDLE OutputH;										// This is an OUTPUT handle.  Think of this as our link to the video card.
	int    length=0;									// This will hold the length of the string passed in.
	int    i=0;											// This is used as a counter.
	
	if(szText == NULL)									// Check if the string is NULL (This makes something was passed in).
      return;											// Quit from the function if no string was passed in.

	OutputH = GetStdHandle(STD_OUTPUT_HANDLE);			// Get an OUTPUT handle to our screen.

	length = strlen(szText);							// strlen() finds the amount of characters in a string.
														// Ok, here is a new concept called "dynamic allocation".  Since we don't
														// know how big the string is that is passed in, we can't just create a colorBuffer like this:
														// WORD colorbuf[I don't know yet];
														// So what we do is just make a pointer, then fill it in on the fly.  This is called "dynamically allocating memory".
														// In, C, we use a function called malloc().  You might notice that it has a weird thing in front of it.  (WORD *).
														// This is called "type-casting".  Since malloc returns a "void *", we need to "cast" the information received into a "WORD *".
														// A "void *" is used so you don't have to have a malloc() for each type of variable, like a WordMalloc(), or an IntMalloc().
														// We can just "cast" the "void *" to what ever variable we are using.  Nifty huh?
														// You can cast anything.  Like, if you want to take a floating point number, and store it into
														// an integer, just say, var1 = (int)var2.  say var2 = 1.2; now var1, being an integer, = 1.  It cuts off the excess.
														// The parameter for malloc() is the number of bytes of memory to allocate.
														// You might say, "But I don't know how many bytes a WORD is..."  That's fine, the sizeof() function does it for you.
														// So, now that we have the bytes of a WORD, we need to figure out how many WORDs we want to allocate.
														// Since we know the length of the string passed in, we just times the amount of bytes by the length.  bytes*length.
	colorBuf = (WORD*)malloc(sizeof(WORD) * length);	// So sizeof(WORD) * length gives us how many bytes of memory we will need to allocate.  Later, we must "free()" the memory.

	for (i=0; i < length; i++)							// Now we go through the color buffer and assign a color for each character.
	{													// A pointer is a one dimensional array.  so we can use the "[]" brackets.
		colorBuf[i] = color;							// Here we assign each cell of colorBuf a color.  The color is what we passed into the function.
	}													// We could say "colorBuf[i] = rand(); and every character in the string would be a random color.
															
														// These bad boys are what draws the text and the text's color to the screen.
														// The first one draws the color to the screen. It's parameters are:
														// "(Output handle, an array to hold the color of each character, the length of the string, the screen position, and the address of a DWORD to hold the number of characters drawn)"
	WriteConsoleOutputAttribute(OutputH, colorBuf, length, screenPos, &dwResult);
														// The next function draws the actual characters to the screen.  It's parameters are:
														// "(Output handle, the string to print, the length of the string, the screen position, and the address of a DWORD to hold the number of characters drawn)"
	WriteConsoleOutputCharacter(OutputH, szText, length, screenPos, &dwResult);
														// The last parameter, we don't care about, we just need it or else the program will crash :)
	free(colorBuf);										// This is how we free memory that we allocated.  There should ALWAYS be a "free()" for every "malloc()".
	colorBuf = NULL;									// After we free a pointer, we should always set it to NULL.  Checking if a pointer equals NULL is the way a pointer is determined if it's "valid" or not.
														// If we didn't set this pointer to NULL it would still point to an address in memory.  However, if we tried to access that memory we'd get a CRASH.  So just set freed pointers to NULL always.
														// You know how you are playing a game, or leave a program running for a while? ... And your computer starts to slow down after a while...?
														// Or when you quit the program, your computer runs slower?  That's not the computer getting tired... that because the programmers
														// didn't free their memory.  If you ask the OS for memory, it locks that memory address so no other program can use it.
														// If you don't free that memory, then the computer still assumes that it's still being used.  You have to restart your computer
														// to get that memory back.  It resets all the permissions you could say.
}

void main()												// Here is our main().
{
	DrawColorString("Red",  40, 0, FOREGROUND_RED);		// Draw "Red" in the color RED.  Draw "White" in the color WHITE.  If we OR (|) the colors together, it becomes white.
	DrawColorString("White",40, 1, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	DrawColorString("Blue", 40, 2, FOREGROUND_BLUE);	// Draw "Blue" in the color BLUE.

	DrawColorString("Red",  40, 4, BACKGROUND_RED);		// Draw "Red" with a background of RED.  Draw "White" with a background of WHITE.
	DrawColorString("White",40, 5, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
	DrawColorString("Blue", 40, 6, BACKGROUND_BLUE);	// Draw "Blue" with a background of BLUE.
														// Draw a red text color with a green background.
	DrawColorString("Color Text!", 40, 8, FOREGROUND_RED | BACKGROUND_GREEN);
}														// End of the program

// There's color for ya.  The next tutorial will focus on a "screen buffer".
// This will allow use to draw things to the screen in one swoop, rather than character by character.
// If any of this is unclear, post your questions on the message board at www.GameTutorials.com.

// © 2000-2005 GameTutorials
