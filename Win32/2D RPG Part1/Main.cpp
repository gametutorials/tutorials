//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 1)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "main.h"								// Include our main header file


///////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the first part of a large tutorial series that show you how to
// take many of our win32 tutorials and incorporate all of them to make a simple game.  
// We will be using the Map Editor that we created in that tutorial series to create
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
// If you happen to go through our ASCII RPG tutorial series, this series will be
// very similar, except in 2D.  Most of the same code will be the same so you don't
// need to get lost in a bunch more code.  Only a few things were needed to be changed
// in order to make the jump from ASCII to 2D.  Of course, this is because we won't
// be doing any animations in this series.  The story in this tutorial series is
// totally different, so all of the game content is of course changed.
//
// *So what is the game?*
//
// The game is a simple RPG where the character, "Quinn", has a sick brother who
// is given not very much time left to live.  Quinn has a friend named Trendor, who
// gives Quinn an idea to go find a sage who, legend has it, has a potion that could
// help his brother.  Quinn, with nothing to lose, decides to go search for that mage.
// Apparently, the sage lives in a the "new world", which is overrun by monsters and
// no one has entered into that area in many many years.  In fact, there is a guard
// who works 24/7 (which is totally against federal regulations!) to guard the bridge
// that cross over into the new world (I guess he doesn't have a family to go home to,
// poor ... guard... guy).  In order to get past the guard, you need to get permission
// from the governor, who lives near the bridge.  After you try and convince the over
// protective governor to let you cross, he makes you get enough strength first before
// you can convince him that you won't get killed.  So you have to go and kill a bunch
// of creatures to get enough strength for him to say okay (100 strength).  To help you
// out, you can go get Trendor and have him join your party.  This will make killing
// the creatures a ton easier.  Once you get permission from the governor, the guard
// will let you pass and you can find the sage, who will give you a potion.  You can
// then go home and save your brother - game over!
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
// The character starts on the "blcorner.map", which is right outside your house.  
//
// Since we are using the maps that we created from the Map Editor it will be easy
// to get us moving around on them.  The work is already done.  I went and created the
// whole world for our game in the editor.  You can find those maps in the Maps\ directory.
// Each map is connected to another map.  The character will be able to walk around the maps
// and when they hit an exit, it will transition to another map (depending on the destination
// map set in the editor).  We have shops, houses, outside wilderness, etc...  
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Quit the game
//
// The base for this code was primarily taken from the Map Editor code, but has been
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
POINT g_cursorPos = {0};						// The current mouse cursor position
CBuffer g_Buffer;								// Create a buffer for the main window

HWND  g_hWnd;									// This is a global handle for the window
HINSTANCE g_hInstance;							// This holds the global hInstance for UnregisterClass() in DeInit()


////////////////////////////// HANDLE WINDOWS MESSAGES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This handles windows messages and used for when we aren't in the main loop
/////
////////////////////////////// HANDLE WINDOWS MESSAGES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CInput::HandleWindowsMessages()
{
	MSG msg;

	// Here we check to see if there is a message, and remove it from the queue if so
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
    { 
		//If we are trying to quit, post a quit message again and return -1 to tell
		// the previous function that call us to quit what ever it's doing.
		if(msg.message == WM_QUIT)
		{
			PostQuitMessage(0);
			return -1;
		}

		// Handle the messages
		TranslateMessage(&msg);					
		DispatchMessage(&msg);					

		// We got a message so return 1
		return 1;
	}

	// No message, return 0
	return 0;
}


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our application-specific data
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	// Set our global window handle to our main window
	g_hWnd = hWnd;

	// Create our double buffering our window
	g_Buffer.CreateDoubleBuffering(hWnd);

	// This is where we create the image of our character
	HBITMAP hPlayerImage = g_Buffer.LoadABitmap((LPSTR)kPlayerImage);
	
	// Initialize our player with it's image and position
	g_Player.Init(hPlayerImage, kPlayerStartX, kPlayerStartY);

	// Init, load and draw the first map file
	g_Map.Load(kStartMap);
	g_Map.SetDrawFlag(true);

	// Set the backbuffer to black first (This clears the backbuffer)
	g_Buffer.ClearScreen(BLACK_BRUSH);			
}


///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop and calls our drawing routine
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	// Below we do our basic game loop.  This is what control the game primarily.
	// We only want the game to keep going as long as the character is alive.  Of
	// course, in this tutorial the character can't die, but we set this up for later.
	// In the game loop we always check for input every frame, then we draw the map.
	// We don't draw the map every frame, but inside the Draw() function there is
	// a check to see if the draw flag is set.  That is why we use the SetDrawFlag()
	// function to tell the map to draw.  If we don't set that each frame, the map
	// will not draw, even though we call the Draw() function.  

	// This is where we load our accelerators for keyboard shortcuts
	HACCEL hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while(g_Player.IsAlive())							// Loop until the player is dead
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message was to quit
				break;

			// Check if there was keyboard command - if not, process messages like normal
			if(!TranslateAccelerator(g_hWnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);					// Find out what the message does
				DispatchMessage(&msg);					// Execute the message
			}

			// Here is where we handle the main game input
			HandleGameInput();

			// Let's draw the map when we need to draw (if the draw flag == true).
			g_Map.Draw();

			// Swap the backbuffers to display the bitmaps to the screen
			g_Buffer.SwapBackBuffer(FALSE);
		} 
	}
	
	// Clean up and free all allocated memory
	DeInit();											

	// Return from the program
	return(msg.wParam);									
}
		

///////////////////////////// HANDLE GAME INPUT /////////////////////////////
/////
/////	This actually handles the main game input from the user (movement, etc...)
/////
///////////////////////////// HANDLE GAME INPUT /////////////////////////////

void HandleGameInput()
{
	// Store the position of the player before we possibly move
	POINT lastPosition = g_Player.GetLastPosition();

	// This is not a function in our CInput class.  We wanted the CInput class
	// to be generic and not actually handle our game code, but just tell us what
	// the user did.  We can then make different functions to handle the input
	// for a given situation.  We do this for character movement, save\load prompts,
	// menus, etc...

	if(g_Input.IsKeyDown(VK_ESCAPE))		// If escape was pressed
		PostQuitMessage(0);					// Since we have no menus yet, just quit
	else if(g_Input.IsKeyDown(VK_UP))		// If up arrow key was pressed
		g_Player.Move(kUp);					// Move the character up
	else if(g_Input.IsKeyDown(VK_DOWN))		// If down arrow key was pressed
		g_Player.Move(kDown);				// Move the character down
	else if(g_Input.IsKeyDown(VK_LEFT))		// If left arrow key was pressed
		g_Player.Move(kLeft);				// Move the character left
	else if(g_Input.IsKeyDown(VK_RIGHT))	// If right arrow key was pressed
		g_Player.Move(kRight);				// Move the character right
	
	// Store the new current position of the player
	POINT curPosition = g_Player.GetPosition();

	// Check if the player moved by testing the current position against the last position
	if(lastPosition.x != curPosition.x || lastPosition.y != curPosition.y)
	{
		// We want to draw the screen again if the character moved
		g_Map.SetDrawFlag(true);
		
		// Set the previous position of the player.  This is important for
		// collision and having party members follow behind the player.
		g_Player.SetLastPosition(curPosition);

		// Here we check to see if the player moved into an exit.  If so, exit!
		g_Map.CheckForExit(curPosition.x, curPosition.y);
	}
}


///////////////////////////////// HANDLE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the actions when a menu item is chosen
/////
///////////////////////////////// HANDLE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void HandleMenu(int menuID)
{
	// Currently not all of the options in the menu are filled in, but as we
	// press forward in the tutorials they will be.  In the help message box,
	// we give the controls and such for the game, but this doesn't mean we have
	// coded all of those yet :)

	if(menuID == ID_FILE_QUIT)					// If the user chose File->Quit
		PostQuitMessage(0);						// Quit the program
	else if(menuID == ID_FILE_OPENUP)			// If the user chose File->Open
	{	
	}
	else if(menuID == ID_FILE_SAVEIT)			// If the user chose File->Save
	{
	}
	else if(menuID == ID_HELP_ABOUT)			// If the user chose Help->About
	{
		// Instead of creating a dialog box, we can just use a simple system MessageBox() call
		MessageBox(g_hWnd, "www.GameTutorials.com RPG\n\n"
						   "\t- To move the character you use the arrow keys.\n" 
						   "\t- To access the main menu, hit the escape key.\n"
						   "\t- To exit from the menus hit the escape key.\n"
						   "\t- To turn on and off the status bar press space bar.\n"
						   "\t- To continue dialog, press space bar or escape.\n"
						   "\t- To use the menus use the left mouse button.", "About", MB_OK | MB_ICONINFORMATION);
	}	
}


/////////////////////////////////// WND PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the messages that come to and from our window
/////
/////////////////////////////////// WND PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Check which message was passed in
    switch (iMsg)											
    {
		case WM_COMMAND:							// Check if we have a command from the user						
			HandleMenu(LOWORD(wParam));				// Pass in the command ID to our menu function
			break;

		case WM_MOUSEMOVE:							// If the mouse moves
			g_cursorPos.x = LOWORD(lParam);			// Store the current cursor position
			g_cursorPos.y = HIWORD(lParam); 
			break;

		// This gets rid of unnecessary erasing of the window (eliminates flickering)
		case WM_ERASEBKGND:
			return 1;

		// This message is sent when the window is destroyed
		case WM_DESTROY:									
			
			// Tell the window we need to close
			PostQuitMessage(0);							
			break;
			
		// Process default messages
		default:
			return DefWindowProc (hWnd, iMsg, wParam, lParam);		
	}													

	// Return zero if we handle message
	return 0;															
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
// code is what was formerly called Tile.cpp in the Map Editor.
// 
// All we did in this Main.cpp file is create an Init() function to load
// and setup our map and player.  We then created an object to check for
// input from the user (CInput).  Next, we create a function to handle that
// input for the main game loop (movement of the character...), which is
// HandleGameInput().  This isn't all of the input, but the main game
// movement.  We use other functions for menu and shopping input.
// 
// We then have our MainLoop() function which has our main game loop.  The
// loop will go until we either quit with ESC, File->Quit, or die, which is 
// impossible in this tutorial, since there are no creatures to fight.
//
// We sure to note the new functions we added in Bitmap.cpp for doing
// opaque blits of bitmaps.  This allows us to do see through effects for
// things such as menus.  It gives our project a more professional, sharp look.
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
