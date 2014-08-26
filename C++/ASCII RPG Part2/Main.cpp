//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 2)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

#pragma comment(lib, "Winmm.lib")				// Include this windows library for PlaySound()
#pragma comment(lib, "fmodvc.lib")				// Include the Fmod library for music

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


#include "main.h"								// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this second tutorial we will jump right into more game code.  We will plug in Menu code,
// moving Npcs, dialog and music and sound.  The SoundFX won't really be used until later
// tutorials, but we will at least put in the code for playing a sound when the player
// dies.  The only sounds we will use will be an item pick up sound, a death sound and an
// evil laugh sound for the end boss during a cut scene.  These all can't be coded yet since
// we won't probably any of that until later tutorials.  We can, however, add the cool music
// in the game immediately :)
//
// In this tutorial, the character can walk up to an Npc and talk to them.  This is done
// by running into them.  The user can then press any key to skip through the dialog.  There
// is a default small pause though before the dialog can be skipped.  Next, you can press
// Escape and toggle a menu.  The menu has options for inventory, saving, loading and quitting,
// yet only the quitting option does anything.  It.... quits :)  The next tutorial will allow
// for inventory menus.  You must use the mouse to choose options from the menu.  We create
// a menu and a button class.  This allows us to create menus with buttons.  You can assign
// some text to a button, and when the user clicks on it with the mouse, it will be directed
// to a function that handles the action for that button.  We do this by function pointers.
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Toggle the main menu (Or escapes a menu)
// Alt+Enter - Go full screen mode (standard console key code for any console application)
// Left Mouse Button - This chooses the options from the main menu
//
// Play the sweet game music and let it inspire you as you check out this tutorial :)
// (Note: you do not need to look at Fmod files, as they are not part of the tutorial lesson)
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
COORD g_cursorPos = {0};						// The current mouse cursor position


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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

	// Init, load and draw the first map file
	g_Map.SetDefault();
    g_Map.Load(kStartMap);
	g_Map.SetDrawFlag(true);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Here we load then play the starting background music
	if(!g_Music.Init("Default.mid"))
		exit(0);

	g_Music.PlaySong();

	// we need to seed our random number generator (rand()) for moving npcs.
	srand(GetTickCount());

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Now that we have menu code, we can display a main menu when ESC is pressed
		if(keyCode == VK_ESCAPE)			// If escape was pressed
			g_Menu.BringUpMenu();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		else if(keyCode == VK_UP)			// If up arrow key was pressed
			g_Player.Move(kUp);				// Move the character up
		else if(keyCode == VK_DOWN)			// If down arrow key was pressed
			g_Player.Move(kDown);			// Move the character down
		else if(keyCode == VK_LEFT)			// If left arrow key was pressed
			g_Player.Move(kLeft);			// Move the character left
		else if(keyCode == VK_RIGHT)		// If right arrow key was pressed
			g_Player.Move(kRight);			// Move the character right

		// If the key pressed was for character movement, let's check for special tiles
		if(keyCode == VK_RIGHT || keyCode == VK_LEFT || keyCode == VK_UP || keyCode == VK_DOWN)
		{
			// Here we check if the character stepped on an exit, if so .... exit.
			g_Map.CheckForExit(g_Player.GetPosition().X, g_Player.GetPosition().Y);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			
			// If we run into an NPC, we need to know so we can display dialog.
			// The dialog code will be called from this function CheckForNpc().
			g_Map.CheckForNpc(g_Player.GetPosition().X, g_Player.GetPosition().Y);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		// We want the Npcs to freely walk around so we need to move them every frame
		g_Map.MoveNpcs();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Let's draw the map when we need to draw (if the draw flag = true).
		g_Map.Draw();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Just like the g_Map.Draw() command, we will attempt to draw the menu, but it
		// will only be draw if it's draw flag is set to true.  This is the main menu.
		g_Menu.Draw();
	}

	// If the player died because they were killed, let's display a death sound.
	// This of course can't be heard until we start fighting monsters :)
	if(!g_Player.IsAlive())
	{
		// We use PlaySound() to play our .wav files. We tell it
		// that we want to play the Death.wav file, and it should be
		// played from a file asynchronously.  That means that it will
		// play on a separate thread and we don't have to wait for it to
		// finish before the game continues on.  We then display a death message.
		PlaySound("Death.wav", NULL, SND_FILENAME | SND_ASYNC);
		g_Menu.DrawMessageBox("You have perished!");
		Sleep(1000);
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Return a success with no problems
	return 1;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we added some small things for doing music and sound,
// as well as making sure we check if we run into Npcs so that we can
// display dialog.
//
// You can jump to Map.cpp for more of the beef of this tutorial, then
// skip over to Menu.cpp.  In the next tutorial we will handle the
// inventory system.  This means, picking up items, dropping them,
// adding equipment to the player, using items, etc...
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
