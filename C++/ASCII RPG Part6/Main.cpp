//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 6)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#pragma comment(lib, "Winmm.lib")				// Include this windows library for PlaySound()
#pragma comment(lib, "fmodvc.lib")				// Include the Fmod library for music

#include "main.h"								// Include our main header file


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this final tutorial we will implement the save and load features.  This means
// that we will be able to save the position of the player in the game so that they
// can quit and then load the exact same position in the story as they were before.
// This means that we will save the player's position, which pa they are on, their
// inventory, their party members, and finally, the action keys that were set during
// the game.  We do this by simply saving all this data to a ".sav" file, which we
// will create.  This will be in binary, and very similar to our .map saving method.
//
// *So how do you save and load?*
//
// To load or save a game, you just need to go to the main menu (ESC) and choose
// "save" or "load" from the menu options with the left mouse button.  This will
// then bring up a prompt that allows you to type in the saved game file, or the
// file you want to save.  You don't need to type .sav at the end of the file
// name since it will be added automatically to the file.  This goes for saving
// and loading.
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Toggle the main menu (Or escapes a menu)
// Alt+Enter - Go full screen mode (standard console key code for any console application)
// Left Mouse Button - This chooses the options from the main menu
//					   - If you click twice on an item in the inventory menu it will
//					     bring up a prompt to use, drop or cancel.
// Space Bar - This toggles on and off the stats screen in the bottom left of the map
// To Attack - Run into a monster with the arrow keys
//
// We added 2 new files, SaveLoad.cpp and SaveLoad.h.  Not much code was changed
// or added in other files, except the player files and a little bit in the map
// files.  Nothing was added to Main.cpp except a global g_SaveLoad object.
// 
// Let's go look at some save/load code. 
//
//

// This is the main object that we use to load and save our character files
CSaveLoad g_SaveLoad;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
COORD g_cursorPos = {0};						// The current mouse cursor position
CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object
CAction g_ActionKeys;							// This is our global action keys array
CShop g_Shop;									// This our our global shop object


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our application-specific data
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init()
{
	// This is where we create the image of our character, a brown @ symbol
	CHAR_INFO playerImage = {'@', FOREGROUND_RED | FOREGROUND_GREEN};
	
	// Initialize our player with it's image and position
	g_Player.Init(playerImage, kPlayerStartX, kPlayerStartY);

	// Now that we have a party, we need to add the normal player to that party
	g_Player.AddPlayerToParty(&g_Player);

	// Init, load and draw the first map file
	g_Map.SetDefault();
    g_Map.Load(kStartMap);
	g_Map.SetDrawFlag(true);

	// Here we load then play the starting background music
	if(!g_Music.Init("Default.mid"))
		exit(0);

	g_Music.PlaySong();

	// we need to seed our random number generator (rand()) for moving npcs.
	srand(GetTickCount());
}


///////////////////////////////// CINPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the CInput class's constructor
/////
///////////////////////////////// CINPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CInput::CInput():m_virtualKeyCode(0)
{
	// Reset all our input variables to 0 or false
	memset(&m_inputRecord, 0, sizeof(INPUT_RECORD));
	m_bLeftMousePressed = m_bRightMousePressed = m_bKeyDown = false; 
}


///////////////////////////////// CHECK MOUSE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the actions when the user uses the mouse
/////
///////////////////////////////// CHECK MOUSE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CInput::CheckMouseInput()
{
	// Get the current position of the mouse in the console window
	g_cursorPos = m_inputRecord.Event.MouseEvent.dwMousePosition;

	// If the user hits the left mouse button, assign it to true
	if(m_inputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
	{	
		m_bLeftMousePressed = true;
	}

	// If the user hits the right mouse button, assign it to true
	if(m_inputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
	{	
		m_bRightMousePressed = true;
	}
}


//////////////////////////////// CHECK INPUT /////////////////////////////////
/////
/////	This check for input from the user
/////
//////////////////////////////// CHECK INPUT /////////////////////////////////

bool CInput::CheckInput()						
{													
	DWORD events=0;										// This holds our input events
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);		// Create a handle for checking input
		
	// We only want to check input if the user did anything with the keyboard or mouse
	GetNumberOfConsoleInputEvents(hInput, &events);
	
	// If there are no events from the user, stop this function
	if(!events) return false;

	// If there was input, read what the user did and store it in m_inputRecord
	ReadConsoleInput(hInput, &m_inputRecord, 1, &events);
													
	// We need to check if the user pressed the key, or let it up for air :)
	m_bKeyDown = m_inputRecord.Event.KeyEvent.bKeyDown;

	// Here we assign the key that the user pressed to our member variable
	m_virtualKeyCode = m_inputRecord.Event.KeyEvent.wVirtualKeyCode;

	// If we used the mouse in any way, let's record those events
	if(m_inputRecord.EventType == MOUSE_EVENT)		
		CheckMouseInput();								

	// There was input, so let's return true
	return true;
}


///////////////////////////// HANDLE GAME INPUT /////////////////////////////
/////
/////	This actually handles the main game input from the user (movement, etc...)
/////
///////////////////////////// HANDLE GAME INPUT /////////////////////////////

void HandleGameInput()
{
	// Let's get the key that the user pressed
	int keyCode = g_Input.GetKeyCode();

	// If we just hit a key from the keyboard, handle the input accordingly.
	if(g_Input.IsKeyboardEvent() && g_Input.IsKeyDown())
	{
		// Now that we have menu code, we can display a main menu when ESC is pressed
		if(keyCode == VK_ESCAPE)			// If escape was pressed
			g_Menu.BringUpMenu();
		else if(keyCode == VK_UP)			// If up arrow key was pressed
			g_Player.Move(kUp);				// Move the character up
		else if(keyCode == VK_DOWN)			// If down arrow key was pressed
			g_Player.Move(kDown);			// Move the character down
		else if(keyCode == VK_LEFT)			// If left arrow key was pressed
			g_Player.Move(kLeft);			// Move the character left
		else if(keyCode == VK_RIGHT)		// If right arrow key was pressed
			g_Player.Move(kRight);			// Move the character right
		else if(keyCode == VK_SPACE)		// If space bar is pressed
			g_Map.ToggleStatsFlag();		// Turn on/off the stats bar

		// If the key pressed was for character movement, let's check for special tiles
		if(keyCode == VK_RIGHT || keyCode == VK_LEFT || keyCode == VK_UP || keyCode == VK_DOWN)
		{
			g_Player.MoveParty();

			// Check the map for items, exits, npcs and monsters to see if the player ran over them
			g_Map.CheckForItem(g_Player.GetPosition().X, g_Player.GetPosition().Y);	
			g_Map.CheckForExit(g_Player.GetPosition().X, g_Player.GetPosition().Y);
			g_Map.CheckForNpc(g_Player.GetPosition().X, g_Player.GetPosition().Y);
			g_Map.CheckForMonster();

			// We want to draw the screen again if the character moved
			g_Map.SetDrawFlag(true);
		}
	}
}


//////////////////////////////// MAIN /////////////////////////////////
/////
/////	This is our program's entry point, the MAIN() function :)
/////
//////////////////////////////// MAIN /////////////////////////////////

int main()
{
	// Initialize our data
	Init();

	// Keep the game going while the player is alive
	while(g_Player.IsAlive())
	{
		// If the user did some input, let's check it and handle it
		if(g_Input.CheckInput())
			HandleGameInput();

		// We want the Npcs to freely walk around so we need to move them every frame
		g_Map.MoveNpcs();

		// Move the monsters randomly around the map
		g_Map.MoveMonsters();

		// Let's draw the map when we need to draw (if the draw flag = true).
		g_Map.Draw();

		// Draw the menu if needed
		g_Menu.Draw();
	}

	// Check if the player is dead of not
	if(!g_Player.IsAlive())
	{
		// Play a death sound when we "die by the sword"
		PlaySound("Death.wav", NULL, SND_FILENAME | SND_ASYNC);
		g_Menu.DrawMessageBox("You have perished!");
		Sleep(1000);
	}

	// Return a success with no problems
	return 1;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Only the global g_SaveLoad was added to this file, so we can jump
// directly to Map.cpp to see the next small additions.  
//
// This is the end of the AsciiRpg tutorial series.  The topics that
// were gone over in this tutorial series should have been simple
// enough to implement them in your own projects.  Of course, these
// are just simple ways to implement in a game, but do not reflect
// the most efficient or bug free.  To make the code more robust
// you will need to add the rest.  This should have been a HUGE push
// to get you on that road. The sky is the limit!  If you want to
// create a game like this in 2D bitmap graphics, you can check out 
// our 2D version in the Win32 section.  This version will use bitmaps
// instead of Ascii.  I personally the like the ASCII graphics better :) j/k
// 
// Good job on getting through all of the tutorials in this series.
// You should be pretty excited to make your own project now.  Go for it!
//
// The music in this tutorial was created by Bjorn Lynne (http://www.lynnemusic.com/).
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
