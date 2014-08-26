//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	digiben@gametutorials.com					//
//																					//
//		$Program:		MoveMan													    //
//																					//
//		$Description:	Using the arrow keys and mouse to move a character around   //
//																					//
//**********************************************************************************//


// This tutorial shows you how to use console functions to create an ASCII character
// on the screen and move it around with the arrow keys on the keyboard.  This is where
// programming gets a little more excited.  Once you get this tutorial understood you
// can start to program some fun things.  Check our Maze tutorial for more on this.


#include "MoveMan.h"								// Here we include our header file that includes all the other headers, functions and structures.

HANDLE hInput, hOutput;								// We create some HANDLES for input and output.  We don't put them in main() because we
													// Want them to be GLOBAL.  That means ALL of the functions can use them, not just inside of main.

CONSOLE_SCREEN_BUFFER_INFO ScrInfo;					// We declare our Screen Information structure.  We'll fill this with all the screen info for our 
													// console window.  We want this GLOBAL so all of our functions can utilize it.

void DrawPlayer(PLAYER Player, int Draw)			// Here we define what our function does
{
	if(Draw)										// We Draw == DRAW (the #define DRAW 1)
	{												// Set the cursor position according to the player's position.
		SetConsoleCursorPosition(hOutput, Player.Position);
		cout << "@";								// Print the '@' (at symbol) to the screen at the cursor's position
	}
	else											// If "Draw" == ERASE, then we want to erase the character
	{												// We want to draw over the character with a ' ' (space), so we set the cursor to the player's position.											
		SetConsoleCursorPosition(hOutput, Player.Position);
		cout << " ";								// Now when we print a ' ', it will draw OVER the last '@' (at symbol).
													// If we didn't erase the player, it would look like he is multiplying and would leave a trail where he had been.
	}
}

void MovePlayer(PLAYER &Player)						// This is where we create our function MovePlayer().
{													// Notice that we put these variable inside of the function, because no one else uses them besides this function.
	INPUT_RECORD InputRecord;						// Here is our structure to hold the information on the input buffer (what the user does)
	DWORD Events=0;									// We create DWORD to be compatible with ReadConsoleInput().  (Holds how many input events took place)
												
													// Read in the input from the user, storing the information into the InputRecord structure.
	ReadConsoleInput(hInput, &InputRecord, 1, &Events);
													// Here we use the "EventType" variable.  This variable holds if it was a MOUSE or KEYBOARD Event.	Meaning, did we use the mouse or keyboard.
	if(InputRecord.EventType == MOUSE_EVENT)		// MOUSE_EVENT is defined somewhere in Windows.  We are checking if we touched the mouse...
	{												// If we did use the mouse, check if we pressed the left mouse button.  FROM_LEFT_1ST_BUTTON_PRESSED is also defined somewhere in windows.
													// If you remember in the last tutorial we used "...Event.KeyEvent...." Now we are checking the mouse information.
													// Inside the "MouseEvent" structure we have variables that hold the button pressed, mouse position, and if the CONTROL key is pressed down.
													// There is one more not worth mentioning... Here we are checking if the Left mouse button is pressed.
		if(InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{											
			DrawPlayer(Player, ERASE);				// Here we call our DrawPlayer() function, but pass in ERASE.
													// We want to erase the OLD character before draw the new character.
													// Remember that "Player" was passed in as a "reference" to this function?
													// Well, since "DrawPlayer()" doesn't need a reference, it just makes a copy of it like normal

													// If we clicked the left Mouse Button, Put the Player where ever the mouse was clicked.
													// We are setting the players position to where ever the mouse clicked... The position of where the mouse clicked
													// Is stored in The InputRecord.Event.MouseEvent.dwMousePosition.
													// dwMousePosition is a COORD.  So to get it's X value we put a ".X" after dwMousePosition.  Same for the Y value.
			Player.Position.X = InputRecord.Event.MouseEvent.dwMousePosition.X;
			Player.Position.Y = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			DrawPlayer(Player, DRAW);				// Here we was to DRAW the character in it's new position so we de-reference the pointer "Player", and pass in DRAW to the function DrawPlayer().
		}
	}

	// Below we do a check to make sure if a key was pressed, and only pressed down.
	// To make sure it was just a down press, and not an up press, we check ...bKeyDown.
	// For Windows 95 and 98, we don't need this, but for NT based systems, it handles
	// the console functions different.  In Windows 95 and 98, it only registers a key
	// when the key is pressed down, where NT OS's register the key up and key down.
	// This makes the player go 2 moves every time, so we need to make sure we just
	// move the player when the key is DOWN.  We use the bKeyDown variable stored in the
	// input record, then just make sure it equals "true" when a key is hit.
	
													// Now we check if there was a keyboard event.
	if(InputRecord.EventType == KEY_EVENT && InputRecord.Event.KeyEvent.bKeyDown)
	{												// If the user hit the keyboard:
													// Check to see if the user hit the RIGHT arrow key
													// Remember we change "...Event.MouseEvent" to "...Event.KeyEvent" because we aren't checking the mouse.
		if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_RIGHT)
		{											// If we hit the RIGHT arrow key
			DrawPlayer(Player, ERASE);				// Erase the at it's current location by passing in ERASE.
			Player.Position.X++;					// Increase the player's X value because we are going RIGHT.

			if(Player.Position.X > ScrInfo.srWindow.Right)	// If we go over the right edge of our window
			{												// Clamp to the right edge of the window
				Player.Position.X = ScrInfo.srWindow.Right; 
			}

			DrawPlayer(Player, DRAW);				// Now we DRAW the player in it's new location by passing in DRAW to our DrawPlayer() function.
		}	
													// Check to see if we hit the LEFT arrow key
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_LEFT)
		{											// If we hit the LEFT arrow key
			DrawPlayer(Player, ERASE);				// Erase the at it's current location by passing in ERASE.
			Player.Position.X--;					// Increase the player's X value because we are going LEFT.

			if(Player.Position.X < ScrInfo.srWindow.Left)	// If we go over the left edge of our window
			{												// Clamp to the left edge of the window
				Player.Position.X = ScrInfo.srWindow.Left; 
			}

			DrawPlayer(Player, DRAW);				// Now we DRAW the player in it's new location by passing in DRAW to our DrawPlayer() function.
		}											
													// Lets check if we hit the UP arrow:								
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_UP)
		{											// If we hit the UP arrow key
			DrawPlayer(Player, ERASE);				// Erase the at it's current location by passing in ERASE.
			Player.Position.Y--;					// Increase the player's X value because we are going UP.
			
			if(Player.Position.Y < ScrInfo.srWindow.Top)	// If we go over the top edge of our window
			{												// Clamp to the top edge of the window
				Player.Position.Y = ScrInfo.srWindow.Top; 
			}

			DrawPlayer(Player, DRAW);				// Now we DRAW the player in it's new location by passing in DRAW to our DrawPlayer() function.
		}	
													// Let's check if we hit the DOWN 
		else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_DOWN)
		{											// If we hit the DOWN arrow key
			DrawPlayer(Player, ERASE);				// Erase the at it's current location by passing in ERASE.
			Player.Position.Y++;					// Increase the player's X value because we are going DOWN.
			
			if(Player.Position.Y > ScrInfo.srWindow.Bottom)	// If we go over the bottom edge of our window
			{												// Clamp to the bottom edge of the window
				Player.Position.Y = ScrInfo.srWindow.Bottom; 
			}

			DrawPlayer(Player, DRAW);				// Now we DRAW the player in it's new location by passing in DRAW to our DrawPlayer() function.
		}	
		
	}	
													
	FlushConsoleInputBuffer(hInput);				// This clears the input buffer each time so it doesn't echo.

}

int main()											// We don't need to return anything from main.
{													// Start of Program.
	PLAYER Player;									// Here we declare our PLAYER structure.  Look in MoveMan.h to see our new structure we created.
	
	hInput = GetStdHandle(STD_INPUT_HANDLE);		// Here we initialize our input handle "hInput"  A HANDLE could be many things so we specify
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);		// What type of Handle we want.  We do the same for the output handle.								
													
													// We set the console mode to enable CONTROL-C and MOUSE_INPUT.  Now we can use the mouse.
													// You'll notice the "|" OR operator.  This combines 2 numbers together, not addition, but their bits.
													// This is a binary operation that sets 0 bits to 1.  Quick lesson in Binary... (Don't expect to understand this really)
													// Here is the number 8 in binary:  "1000"
													// Here is the number 2 in binary:  "0010"
													// Do you see how I got those numbers?  The numbers read from right to left.
													// 0 = "0000" 1 = "0001" 2 = "0010" 4 = "0100" and 8 = "1000"
													// Each to the left is the number to the right times 2 essentially. So... if we add 2 "0010" + 1 "0001" = "0011" -> we get 3.
													// So... what an OR "|" does.. is .. well let me just show you...
													//						"8:  "1000"
													//			OR'd with   "2:  "0010"
													//			becomes		"10: "1010"
													// So 8 OR'd with 2 becomes 10 because it combines the bits together if the same slots have a 1 and a 0.
													//						8: "1000"
													//			 OR'd with	8: "1000"
													//					=	8: "1000:
													// So 8 OR'd with 8 becomes 8 because they both have "1" in that first slot.  Anyway.. . I will do
													// A huge tutorial on binary operators, but basically it compares the 2 numbers and sets the bits of the other number in the same slot to 1 if they are zero.
													// Don't stress it, I didn't get it for a LONG time.  The teachers did a horrible job in teaching it (like I just did)..
													// My friend had to teach it to me, but once he showed me some simple examples, I got it.  You just need a lot of examples.
													// To close this concept up, for now accept that is allows you to input 2 options.  What the function does is
													// Takes a MASK which checks which bits (bits meaning slots containing a 1 or a 0) are set to 1.
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	GetConsoleScreenBufferInfo(hOutput, &ScrInfo);	// Fills our global structure with all the console screen info

	Player.Position.X = SCREEN_WIDTH  / 2;			// We want our player to start in the center of the screen.
	Player.Position.Y = SCREEN_HEIGHT / 2;			// So we divide the width and height by 2 right?

													// Here we call this function to Set the cursor position.  The position comes from our player structure.
	SetConsoleCursorPosition(hOutput, Player.Position);
	
	////////////////////////////// G*A*M*E L*O*O*P ////////////////////////////////
	
	while(1)										// Let's do an infinite loop so that we keep checking for keyboard and mouse input.
	{												// Remember, while(1) is ALWAYS true, so it will never stop, unless we close the window or hit CONTROL-C on the keyboard.
		MovePlayer(Player);							// Here we call our function move player.  
	}

													// Doesn't our game loop look pretty now? :)  Just one function call.
													// This is what a game loop should look like.  Just function calls.  Maybe an "if" or so, but
													// The idea is to keep it simple.  We just want to create functions that control and run the game..
													// And then call those function inside the game loop.
													// Layer, you will see a "while(GameIsRunning)" .. 
													// The "GameIsRunning" Flag would be set to 1 (or TRUE) first, then if they user hits VK_ESCAPE (the escape key) "GameIsRunning" will be set to 0 (or FALSE)
													// Causing the "while(GameIsRunning)" loop to quit and the game is over.  For a list of all the Virtual Key Codes like "VK_etc", you can goto
													// http://msdn.microsoft.com/library/
													// Hopefully the site is still the same by the time people read this :)
													// That is also where you can search for HELP on programming and syntax.
													// For now we will just let CONTROL-C quit.
	return 0;										// Return zero for "zero problems in this program"
}													// End of Program


// Now I am sure you understand how to use structures, and if you don't... you will never make it!
// Just kidding!  Like everything, practice makes perfect.  It's like learning ANY language, you need to
// Constantly be around it and more importantly USE it.
// 
// * Windows 2000 Problems *
//
// You should keep in mind that the mouse code will NOT work on some operatoring systems, such as
// windows2000 until you turn off QuickEdit and Insert Mode for that window.  To do this, you will
// want to run the program, then click in the upper left hand corner of the window.  A menu will come
// up, now go to properties.  Make sure you have the Options tab selected and look in the Edit Options
// area.  Then, uncheck QuickEdit and Insert Mode.  That way it won't try and think you are wanting
// to copy some text from the window.  Next, choose the save button and make sure it saves it for
// all window titles with the same name.  Then close the program and open it up again. It should work then.
//
//
// ©2000-2005 GameTutorials
