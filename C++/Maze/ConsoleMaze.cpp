//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com					//
//																					//
//		$Program:		ConsoleMaze													//
//																					//
//		$Description:	Creates a maze that you can walk around and collide with	//
//																					//
//**********************************************************************************//


// In this tutorial we show you how to create a start to a maze game.  We use console
// functions to create a maze using ASCII characters (the 255 or so keys on the keyboard)
// and allow the user to control a character in the maze with the arrow keys.  This
// is a great tutorial to show you how to have a character in a world and be able to
// control them.  We also will show how to do some simple collision in a very small
// amount of code.  Basically, we create an array (a list of characters) and store
// some random '#' characters to represent walls, and spaces ' ' to represent space
// that the character can walk.  The character is represented as an "at" symbol, which
// is the ASCII character value 1.  To check collision we just store the position of
// the character and then check if the position runs into an '#' symbol in our array.
// Check below to see our tutorial that shows how we code all of this.


#include <windows.h>									// We need to include this for "console" functions.

#define SCREEN_WIDTH  20								// This is the width of our window 
#define SCREEN_HEIGHT 15								// This is the height of our window
#define START_X        0								// This is the starting X position for our character
#define START_Y        7								// This is the starting Y position for our character

#define PLAYER 1										// This defines our player character (smiley face)
#define WALL '#'										// This defines our wall character for collision
	
// This is our prototype for our drawing maze function (We have this so we can define it below main())
void DrawMaze(CHAR_INFO screenBuffer[], int playerX, int playerY);

// This is the prototype for the function that moves our character around the screen
bool MovePlayer(CHAR_INFO screenBuffer[], COORD &playerPos);

int main()												// Here is our main().
{														// This is our buffer that will hold all the maze data we wish to display to the screen.
	CHAR_INFO screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};	
	COORD bufferSize = {SCREEN_WIDTH , SCREEN_HEIGHT};	// This is a COORD that holds the width and height of our window that we will display
	COORD playerPos = {START_X, START_Y};				// This will hold our players position

	srand( GetTickCount() );							// This seeds our random numbers with time so it isn't always the same maze

	// Below we create our maze with a bunch of random # signs.
	// This just fills in our screen buffer with the data, but DOESN'T draw it yet.
	// It is faster to draw it all at once, rather than using WriteConsoleOutputCharacter().

	for(int y = 0; y < bufferSize.Y; y++)				// This fills in the columns (y value) of our buffer
	{
		// You will notice that we start at x = 2.  This is so we have some room to freely walk around in front of the maze.

		for(int x = 2; x < bufferSize.X; x++)			// This fills in the rows (x value) of our buffer.
		{												// Below, we fill in the current index with a wall and color.
			if(!(rand() % 3))							// If our random number is 2, then draw a wall in our maze.  This mixes up the maze.
			{
				screenBuffer[x + y * SCREEN_WIDTH].Char.AsciiChar = WALL;
				screenBuffer[x + y * SCREEN_WIDTH].Attributes = FOREGROUND_GREEN;
			}
		}
	}

	// This will draw the player in it's current position, along with the maze.
	// It takes our screen buffer, and the players position to be drawn at.
	// This will draw the initial maze to the screen.
	DrawMaze(screenBuffer, playerPos.X, playerPos.Y);		

	// Below is our main loop that check to see if we move the player, and if so, redraw the screen.

	while(1)
	{
		// We pass in our player position because we will change it if we move.
		// If we do move, then we return TRUE from MovePlayer() and we need to redraw the screen.

		if(MovePlayer(screenBuffer, playerPos))
		{
			// Draw the new updated player position within the maze
			DrawMaze(screenBuffer, playerPos.X, playerPos.Y);		
		}
	}
	
	return 0;											// Return with zero problems
}														// End of the program


//////////////////////////////// DRAW MAZE /////////////////////////////////
/////
/////	This draws our screenBuffer to the screen, along with the player's position
/////
//////////////////////////////// DRAW MAZE /////////////////////////////////

void DrawMaze(CHAR_INFO screenBuffer[], int playerX, int playerY)
{														// This is our rectangle that holds the coordinates that we are drawing to
	SMALL_RECT drawRect = {0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1}; 
	COORD bufferSize = {SCREEN_WIDTH , SCREEN_HEIGHT};	// This is a COORD that holds the width and height of our window that we will display
	COORD zeroZero = {0, 0};							// This tells us the upper left corner to write from
	HANDLE hOutput;										// This holds our output handle to the screen
	
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);			// Get an OUTPUT handle to our screen (Permission/key to draw).

	// Draw the player at the position passed in
	screenBuffer[playerX + playerY * SCREEN_WIDTH].Char.AsciiChar = PLAYER;
	screenBuffer[playerX + playerY * SCREEN_WIDTH].Attributes = FOREGROUND_RED;

	// This draws our buffer to the screen lightning fast!
	WriteConsoleOutput(hOutput, screenBuffer, bufferSize, zeroZero, &drawRect);
}


//////////////////////////////// MOVE PLAYER /////////////////////////////////
/////
/////	This checks for keyboard input, changes the player's position 
/////   and erases its previous position if the player moved.
/////
//////////////////////////////// MOVE PLAYER /////////////////////////////////

bool MovePlayer(CHAR_INFO screenBuffer[], COORD &playerPos)						
{													
	INPUT_RECORD InputRecord;							// Here is our structure to hold the information on the input buffer (what the user does)
	COORD oldPosition = playerPos;						// This stores the old player position so we can erase it when we move.
	DWORD Events=0;										// We create DWORD to be compatible with ReadConsoleInput().  (Holds how many input events took place)
	HANDLE hInput;										// This will be used to query the keyboard
	bool bPlayerMoved = false;							// This holds if the player move or not
	int bKeyDown = 0;									// This tells us if we pressed DOWN a key
				
	hInput = GetStdHandle(STD_INPUT_HANDLE);			// Here we initialize our input handle "hInput"  A HANDLE could be many things so we specify.
														// Read in the input from the user, storing the information into the InputRecord structure.
	ReadConsoleInput(hInput, &InputRecord, 1, &Events);
													
	// Now we check if there was a keyboard event.

	// For Windows 95 and 98, we don't need this, but for NT based systems, it handles
	// the console functions different.  In Windows 95 and 98, it only registers a key
	// when the key is pressed down, where NT OS's register the key up and key down.
	// This makes the player go 2 moves every time, so we need to make sure we just
	// move the player when the key is DOWN.  We use the bKeyDown variable stored in the
	// input record, then just make sure it equals "true" when a key is hit.
	// Notice the "&& bKeyDown" in the following if statement.  That says that if the key
	// was pressed, we only want to enter that if statement if it was a key down press.
	bKeyDown = InputRecord.Event.KeyEvent.bKeyDown;

	if(InputRecord.EventType == KEY_EVENT && bKeyDown)
	{													// If the user hits the keyboard:
														// Check to see if the user hit the RIGHT arrow key														
		if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT)
		{												
			if(playerPos.X < (SCREEN_WIDTH - 1) )		// Check to make sure our new position doesn't go outside of our buffer
			{
				playerPos.X++;							// Increase the player's X value because we are going RIGHT
				bPlayerMoved = true;					// Set the flag to TRUE that the player moved
			}
		}	
														// Check to see if we hit the LEFT arrow key
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_LEFT)
		{										
			if(playerPos.X > 0)							// Check to make sure our new position doesn't go outside of our buffer
			{
				playerPos.X--;							// Decrease the player's X value because we are going LEFT
				bPlayerMoved = true;					// Set the flag to TRUE that the player moved
			}
		}											
														// Lets check if we hit the UP arrow:								
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_UP)
		{
			if(playerPos.Y > 0)							// Check to make sure our new position doesn't go outside of our buffer
			{
				playerPos.Y--;							// Decrease the player's Y value because we are going UP
				bPlayerMoved = true;					// Set the flag to TRUE that the player moved
			}
		}	
														// Let's check if we hit the DOWN 
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_DOWN)
		{							
			if(playerPos.Y < (SCREEN_HEIGHT - 1) )		// Check to make sure our new position doesn't go outside of our buffer
			{
				playerPos.Y++;							// Increase the player's Y value because we are going DOWN
				bPlayerMoved = true;					// Set the flag to TRUE that the player moved
			}
		}												// If we hit ESCAPE
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
		{												
			exit(0);									// Quit the program
		}
	}	

	// If the player moved
	if(bPlayerMoved)
	{
		// Here we check for collision with a wall.  If our new positive is the same X and Y
		// as a wall, we want to set the characters position back to it's old position.
		// Otherwise, if it isn't a wall, then we want to erase the old position with a space

		// If we didn't hit a wall, erase the old position
		if(screenBuffer[playerPos.X + playerPos.Y * SCREEN_WIDTH].Char.AsciiChar != WALL)
		{
			// Erase the old position of the player with a space
			screenBuffer[oldPosition.X + oldPosition.Y * SCREEN_WIDTH].Char.AsciiChar = ' ';
			screenBuffer[oldPosition.X + oldPosition.Y * SCREEN_WIDTH].Attributes = 0;
		}
		else // We hit a wall
		{
			// Set the player's position back to it's old position (Hence, we collided)
			playerPos = oldPosition;
		}

		return true;									// Return true to say we need to redraw the screen
	}

	return false;										// We didn't move, so return false
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//	*Quick Notes*
//
//	This tutorial lets you make a random maze and have a character walk around in it.
//  Now you can actually make a cool game!!! This is probably the simplest form
//  of collision.  Basically, all you are doing is testing if the player hits a wall
//  by checking an X and Y position within the screen buffer array.  If there is a wall
//  character, then don't let the player walk there.  It's only ONE line of code to do this.
//  
//  Using console functions can be used for many things, including menus for applications
//  and any other graphics representation you would like to show to the user, without having
//  to program a lot of graphics with bitmaps.  Hopefully you can think of some other awesome 
//  things to do with this.
//  
//
// © 2000-2005 GameTutorials
