//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 3)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#pragma comment(lib, "Winmm.lib")				// Include this windows library for PlaySound()
#pragma comment(lib, "fmodvc.lib")				// Include the Fmod library for music

#include "main.h"								// Include our main header file


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this tutorial we will add the functionality for our inventory system.  This mean
// that we will need to add support for picking up items, dropping items, and adding
// equipment to the player like a weapon, armor, etc.  Since we already made a simple
// menu system in the last tutorial, we can use that to create an inventory screen
// to manage our items and such that we pick up.  In this menu we will program the
// options to add equipment to the player, as well as the option to use or drop an
// item.
//
// For the purpose of this tutorial, we added some items in the first town of Tafar.  You
// can go in the town and walk around and pick up the items and then check your inventory.
// To pick up an item you just need to walk right over it.  It will do it automatically.
// You just hit ESC and choose the "Inventory" option from the main menu.  This is of
// course done with the mouse.  This will bring up the inventory menu.  You can then click
// on an item and see it's stats at the bottom of the inventory menu.  If you click again
// on the same item it will bring up a prompt that will ask you if you want to use the
// item, drop the item or cancel.  If the item is a type of equipment then when you use
// it that will add it to your equipment that the player is wearing.  This is seen on
// the left hand side of the inventory menu under the headings of "Head", "Chest", "Weapon",
// and "Feet".  However, if the item is not a type of equipment, then it will add
// (or subtract) to your current stats, depending on the item's stats.  When you equip
// something, the stats associated with that item will be added to the player.  When you
// change equipment, those stats will be removed from the player and the new stats of the
// new item will be added.  You might make some items or equipment bad to wear, or not
// as effective.  For instance, a bad mushroom item will actually take away health when
// used.  Also, maybe a sword might help you with strength, but actually have a negative
// effect on your protection stat.
//
// Remember, you can always hit ESC to exit out of menus (or go back to the previous menu).
// We add a silly little sound when ever the player picks up an item.  Feel free to giggle
// every time you hear it :)
// 
// Another addition in this tutorial is a little stats screen that shows up at the bottom
// of the screen.  This tells the player their health, level, experience and gold.  You can
// increase these stats as you fight monsters, but that won't be setup until a later tutorial.
// To turn on and off this stats menu you can just press the space bar.  This can get in
// the way of game play, so you wouldn't always want it on.  We currently just draw this
// screen over the map and the character, but this causes some flicker.  If you want to get
// rid of this for your own game you will want to draw the stats screen directly into the
// screen buffer of the map so it draws at the same time.
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
// 
// Let's go pick up some items!  (Note that when you pick up an item and leave the map
// it's back there on the map.  When we get into saving and loading of maps we will discuss
// how to get rid of this if so desired.)
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
COORD g_cursorPos = {0};						// The current mouse cursor position
CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// If we hit space let's toggle on and off the stats at the botton of the screen
		else if(keyCode == VK_SPACE)
			g_Map.ToggleStatsFlag();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// If the key pressed was for character movement, let's check for special tiles
		if(keyCode == VK_RIGHT || keyCode == VK_LEFT || keyCode == VK_UP || keyCode == VK_DOWN)
		{


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// Since we need to pick up items, we need to check for them just like exits, etc.
			g_Map.CheckForItem(g_Player.GetPosition().X, g_Player.GetPosition().Y);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			
			
			// Check for an exit and then an NPC in the player's new position
			g_Map.CheckForExit(g_Player.GetPosition().X, g_Player.GetPosition().Y);
			g_Map.CheckForNpc(g_Player.GetPosition().X, g_Player.GetPosition().Y);

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
// In Main.cpp we added a check for space bar, which toggles on and
// off the stats menu at the bottom of the screen.  We also needed to
// check for items on the ground, so we added CheckForItem() in that
// same function, right above checking for exits and npcs.
// 
// You can now jump over to Map.cpp to see the new additions we added there. 
// The next tutorial will go over action keys and cut scenes, as well as 
// fighting monsters and gaining levels and other stats.
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
