//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 4)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#pragma comment(lib, "Winmm.lib")				// Include this windows library for PlaySound()
#pragma comment(lib, "fmodvc.lib")				// Include the Fmod library for music

#include "main.h"								// Include our main header file


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this tutorial we added a ton of new features for our game.  Particularly, we added
// support for fighting monsters, action keys, cut scenes, and ending (You can now win
// AND lose the game!), and finally, we added the ability to add a new player to your
// party.  There is a lot of code to go over in this tutorial, but sit back and hopefully
// that funny/silly sound that plays when you pick up items will keep you giggling long
// enough to finish this tutorial :)
//
// There is a lot of new additions, so let's first start with the cut scenes that deal
// with the actual game.  If you go into the first town of Tafar and enter the castle,
// immediately a cut scene will start and the king will call Adol to the throne.  You will
// then be given a quest.  In order to win the game you will need to find the treasure.
// The treasure is an item, but we create that item on the fly when the player enters
// the palace.  The palace is where Valkar is.  Valkar is the last boss :)  You have to
// find him.  Once you enter the palace where Valkar is, another cut scene will happen.
// This has new music and a cool sound effect :)  Another part of the story is that you
// can add another player to add to your party.  This is an npc called Jax.  You can find
// him in one of the shops in a town.  He will talk to you about how he wants to join you.
// Once he joins you, you can have a new player that follows you.  They also add to your
// attacking power when running into monsters.  Keep in mind, that in our tutorial series
// we don't allow you to do anything with an extra party member like see their stats or
// their items.  They just stick with you until they get killed :)
//
// *So, how do I attack a monster?*
//
// In order to attack a monster, you need to run into them.  If they run into you first
// then they will attack you.  But, you need to be careful.  Monsters are smart.  If
// you are in their radius they will come and attack you.  Don't get too close to them
// or you will most likely get attacked.
//
// *So what are these action keys?*
//
// We use "action keys" to add special cases to our game.  Since we want to allow
// cut scenes and special things to happen which make our game, we need ways to
// tell the code that we want to do something special.  For instance, if we want
// Jax to be added to our party, we give him an action key in our map editor when
// we placed Jax to the map as an npc.  We then check the action keys of each npc
// and see if we have a special case that needs to be handles.  This also works
// for shop keepers.  We will use the action key 555 to signal our code that this
// npc is a special npc which sells goods.  We can then program a shop for that npc
// when the player talks to them.  This will be in the next tutorial.  Not just action
// keys will signify this, but we also do checks every time we open a new map.  We do
// a special check that sees if we need to do a cut scene (like the King cut scene).
// After the cut scene is over, we set an index of our action key array that says
// that scene is already over, so don't do it again.  This is a simple way to keep track
// of what has happened in the game.  It makes it easy to save the game because we just
// need to save the action key array and we saved the game state.  Cool huh?  Of course
// we still need to save the player's data and inventory, etc.
//
// Since there is much to cover, you will better understand how these new features work
// by just jumping into the code.  We will explain further as we come across these features.
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
//

// This is our global action keys array.  This stores all the game situations and
// special cases.  We use this for cut scenes and story progression, along with shops (later).
CAction g_ActionKeys;

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Now that we have a party, we need to add the normal player to that party
	g_Player.AddPlayerToParty(&g_Player);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// This function updates our party to take the last position of the
			// party member in front of them.  If we have 3 players in our party
			// there will be a domino effect each time the main player moves.  The
			// second player will take the last step of the first player, and the
			// third player will take the last step of the second player.  They follow
			// you every step of the way...  What a loyal party :)
			g_Player.MoveParty();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


			// Check the map for items, exits and npcs to see if the player ran over them
			g_Map.CheckForItem(g_Player.GetPosition().X, g_Player.GetPosition().Y);	
			g_Map.CheckForExit(g_Player.GetPosition().X, g_Player.GetPosition().Y);
			g_Map.CheckForNpc(g_Player.GetPosition().X, g_Player.GetPosition().Y);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// Now that we have monsters moving around we need to check to see if we
			// ran into them so we know when to attack.  Notice that this takes no
			// parameters.  This is because we do a loop inside that goes through all
			// of our party members and checks if they ran into the monster.
			g_Map.CheckForMonster();

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

		// We want the Npcs to freely walk around so we need to move them every frame
		g_Map.MoveNpcs();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// Move the monsters randomly around the map.  The monsters will also move
		// towards the players if they are in a determined radius around them.
		g_Map.MoveMonsters();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// In this file we added the global action keys array, as well as put
// in place some functions that deal with our party.  We also needed to
// check for monsters every time the player moved (CheckForMonster()).
// 
// In the next tutorial we will deal with buy and selling from shop keepers.
// From here let's jump to Map.cpp and see the new additions in the map.
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
