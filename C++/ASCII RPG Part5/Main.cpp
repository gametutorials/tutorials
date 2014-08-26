//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 5)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#pragma comment(lib, "Winmm.lib")				// Include this windows library for PlaySound()
#pragma comment(lib, "fmodvc.lib")				// Include the Fmod library for music

#include "main.h"								// Include our main header file


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is probably the simplest of the tutorial series.  All we added was the
// code necessary to handle buying and selling from shop keepers.  We will now
// be able to buy items and equipment, as well as sell that equipment.  This is
// done by giving each shop keeper a simple action key of 555.  When ever we
// talk to a shop keeper, we will check their action key and then jump right
// into the buying and selling menu.
//
// *What will the shops sell?*
//
// Since our item's data is usually stored in the Items.txt file with the
// map editor, we need to access that data when we create our shops.  This is
// why we use the same items text file used to make the maps for this game.
// We just stick that text file in our project directory.  Then, when ever
// the player goes to the store, it will read some random items from that
// text file to sell.  Of course, you might want the items to be static and
// not change, which will require you to make some changes to make this happen.
// But, for the purpose of a simple way to get items in our shops, we use this
// method.  We read some random items from the text file each time the user
// talks to the shop keeper.  Ideally, you will want to add support in the
// map editor to choose which items that shop keeper will hold.  Also, you
// probably don't want the items file to be easily accessible to the user,
// since they can just easily open it up and change the data to really
// ridiculous high stats right?  This is all something you can add yourself
// in your own project.
//
// To create the shop menu, we inherit from the CMenu class, since most of
// the functionality is the same.  This allows for good code re-use.  We
// added 2 new files for the shop code, ShopKeeper.cpp and ShopKeeper.h.
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
// We have a shop keeper placed in each town, behind a counter in one of the
// houses.  Think of that as the local tavern or something :)  There is nothing
// really new in this file except the declaration of the global CShop object, so
// you can immediately jump over to ShopKeeper.cpp to go over the rest of the tutorial.
//
//

// This is our global shop object for handling our shopping
CShop g_Shop;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
COORD g_cursorPos = {0};						// The current mouse cursor position
CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object
CAction g_ActionKeys;							// This is our global action keys array


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
// Only the global g_Shop was added to this file, so we can jump
// directly to ShopKeeper.cpp to see the shop code additions.  In 
// the next tutorial we tutorial we will go how to save and load a 
// game.
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
