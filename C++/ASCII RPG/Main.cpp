//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 1)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "main.h"								// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the first part of a large tutorial series that show you how to
// take our console tutorials and incorporate all of them to make a simple game.  We
// will be using the Ascii Map Editor that we created in that tutorial series to create
// the maps for our game.  This game will not be a robust game, but will include most
// of the major features you need to have in order to make a (RPG) game.  For example:
//
// Loading maps, collision detection, player movement, exiting to other maps,
// the ability to interact with characters, inventory system, sound/music, fighting
// monsters, gaining levels/experience, stats, buying and selling from shops, saving
// and loading previously saved games, added party members, simple ai, cinematic cut
// scenes, menus and dialog.
//
// Wow, looking at all of those options make you want to cry!  But don't worry, we will
// go over each one :)  You can wipe away your tears of intimidation and put on some
// sunglasses and sip on a cool lemonade because this tutorial series is gonna be *Hot*!
//
// *So what is the game?*
//
// The game is a simple RPG where the character, "Adol" is given a quest by the King.
// Apparently, the King's treasure has been taken he Enlists Adol to go find it.  Adol
// goes to different towns and gets hints as the where the treasure might be.  He is
// then able to join up with a character "Jax", who will join the party and go fight for
// the treasure too.  They then find the treasure is taken by Valkar, who used to work
// for the King, but was a monster in disguise and a murderer.  You fight Valkar and then
// take the treasure, which ends the game.  The game ending is different depending on who
// is in your party.  
//
// This is a very simple game with no complex or interesting plot, but it should get
// you excited to use your own creativity to make a robust game with a robust plot and
// interesting dialog with the characters :).  The programming techniques in this tutorial
// series don't reflect the best or most efficient/fastest methods, but the simplest methods
// to help someone who wouldn't know how to do these features to be able to get a start
// to do them.  That is the intent of this series, to give you the most basic ways to
// program a game like this.  You then can take it and improve it.
//
// *What does this tutorial include?*
//
// In this tutorial we will be able to move a character around, load maps, exit to other
// maps and ride a pony.  Well, ... we won't be able to ride a pony, but it's just as fun!
// So, just the basic movement of the game, with the collision detection added as well.
// The character starts on the "Default.map", which is right outside the town "Tafar".  
//
// Since we are using the maps that we created from the Ascii Map Editor it will be easy
// to get us moving around on them.  The work is already done.  I went and created the
// whole world for our game in the editor.  You can find those maps in the Maps\ directory.
// Each map is connected to another map.  The character will be able to walk around the maps
// and when they hit an exit, it will transition to another map (depending on the destination
// map set in the editor).  We have shops, houses, towns, outside wilderness, etc...  
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Quit the game
// Alt+Enter - Go full screen mode (standard console key code for any console application)
//
// The base for this code was primarily taken from the Ascii Map Editor code, but has been
// changed enough to warrant everything *** NEW *** :)  If you haven't looked at that code then
// you should, since we won't be going over much explanation in this tutorials of topics
// already covered, but will focus more on the game design and theory of how to put the
// game together.
// 
// Let's get on with the code!
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// To handle our map loading and such, we have a global map object.  We also
// have our player object global as well.  To handle input we created an object
// to control the user's input.  We also store the cursor position for menus later.

CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
COORD g_cursorPos = {0};						// The current mouse cursor position


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

	// Init the map and load the first map.  Then set the flag that tells
	// the map's Draw() function to draw that frame.  We don't want to draw
	// every frame, but only when something happens.
	g_Map.SetDefault();
    g_Map.Load(kStartMap);
	g_Map.SetDrawFlag(true);
}


// Below we define our CInput class to handle the input from the user.
// We don't create another file for it since it's not that big yet.

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
	// This function is checked every frame and tells us if the mouse is being
	// used or not, and which buttons were pressed.

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
		
	// This function just checks if the user did anything, and if so, handles
	// the input for it.  It doesn't handle it as much as stores the information
	// for us to use elsewhere.  For instance, the key or mouse button that was pressed.
	// We only need to write this code once, and then we can use it in many other
	// places when we do different code for things like character movement, menu
	// input, etc...

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
	// This is not a function in our CInput class.  We wanted the CInput class
	// to be generic and not actually handle our game code, but just tell us what
	// the user did.  We can then make different function to handle the input
	// for a given situation.  We do this for character movement, save\load prompts,
	// menus, etc...
	
	// Let's get the key that the user pressed
	int keyCode = g_Input.GetKeyCode();

	// If we just hit a key from the keyboard, handle the input accordingly.
	if(g_Input.IsKeyboardEvent() && g_Input.IsKeyDown())
	{
		if(keyCode == VK_ESCAPE)			// If escape was pressed
			exit(0);						// Quit the game
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

	// Below we do our basic game loop.  This is what control the game primarily.
	// We only want the game to keep going as long as the character is alive.  Of
	// course, in this tutorial the character can't die, but we set this up for later.
	// In the game loop we always check for input every frame, then we draw the map.
	// We don't draw the map every frame, but inside the Draw() function there is
	// a check to see if the draw flag is set.  That is why we use the SetDrawFlag()
	// function to tell the map to draw.  If we don't set that each frame, the map
	// will not draw, even though we call the Draw() flag.  

	// Keep the game going while the player is alive
	while(g_Player.IsAlive())
	{
		// If the user did some input, let's check it and handle it
		if(g_Input.CheckInput())
			HandleGameInput();

		// Let's draw the map when we need to draw (if the draw flag = true).
		g_Map.Draw();
	}

	// Return a success with no problems
	return 1;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// This first file was pretty simple.  Nothing too new was really added,
// but it's important to get a input and drawing setup done correctly
// so that you can specify the game loop and have the basis for your game.
// 
// I suggest checking out Player.cpp next to see the player movement and 
// draw code.  You can then check out the beefy parts in Map.cpp :)  This
// code is what was formerly called Tile.cpp in the Ascii Map Editor.
// 
// All we did in this Main.cpp file is create an Init() function to load
// and setup our map and player.  We then created an object to check for
// input from the user (CInput).  Next, we create a function to handle that
// input for the main game loop (movement of the character...), which is
// HandleGameInput().  This isn't all of the input, but the main game
// movement.  We use other functions for menu and save\load functions.
// 
// We then have out main() function which has our main game loop.  The
// loop will go until we either quit with ESC, or die, which is impossible
// in this tutorial.
// 
// In the next tutorial we will focus on the dialog of Npc's and music/sfx.
// We need to also go over menu code in order to do this.
//
// Good luck till then!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
