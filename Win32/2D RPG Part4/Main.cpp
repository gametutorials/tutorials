//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 4)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

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
// with the actual game.  The first cut scene,or long dialog scene, happens when we go
// into our house.  Our friend Trendor and our sick brother are there.  When we talk
// Trendor we go through a bunch of dialog that tells us the point of the game.  Trendor
// says that he would like to help, but you first have to go find and talk to the
// Governor to see if you can get his permission to cross the bridge to enter the new
// world.  That is where you can find the sage who has a potion that can possibly cure
// your brother's disease.  If you don't have the permission,the guard won't let you pass.
// After you talk with the Governor, you can go talk to Trendor again and he will join
// your party, which will help you fight creatures more effectively.  Eventually, you
// get enough experience (100) to prove to the Governor that you can defend yourself in
// the new world.  The guard will then let you pass and you can find the sage.  When you
// get to the sage, there is a long cut scene.  Quinn will receive the potion and upon
// coming home to his brother there is a cut scene that shows Quinn walk up to his brother
// and give him the potion.  There is a small dialog scene and the game is over.  All of
// this story is done using action keys.  Love'em, learn'em, marry'em.  Okay,... maybe
// hold off on the engagement, but you know what I mean :)
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
// Trendor to be added to our party, we give him an action key in our map editor when
// we placed Trendor to the map as an npc.  We then check the action keys of each npc
// and see if we have a special case that needs to be handles.  This also works
// for shop keepers.  We will use the action key 555 to signal our code that this
// npc is a special npc which sells goods.  We can then program a shop for that npc
// when the player talks to them.  This will be in the next tutorial.  Not just action
// keys will signify this, but we also do checks every time we open a new map.  We do
// a special check that sees if we need to do a cut scene (like the sick brother scene).
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
// Esc - Toggle the main menu (Or escapes a menu) and escape/continue dialog
// Left Mouse Button - This chooses the options from the main menu
//					   - If you click twice on an item in the inventory menu it will
//					     bring up a prompt to use, drop or cancel.
// Space Bar - This toggles on and off the stats screen in the bottom left of the map.
//			   We can also use this key to escape/continue dialog.
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
CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object
CBuffer g_Buffer;								// Create a buffer for the main window
POINT g_cursorPos = {0};						// The current mouse cursor position

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Now that we have a party, we need to add the normal player to that party
	g_Player.AddPlayerToParty(&g_Player);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Init, load and draw the first map file
	g_Map.Load(kStartMap);
	g_Map.SetDrawFlag(true);

	// Here we load then play the starting background music
	if(!g_Music.Init("techno.mod"))
		exit(0);

	g_Music.PlaySong();

	// we need to seed our random number generator (rand()) for moving npcs.
	srand(GetTickCount());

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

			// Just like the g_Map.Draw() command, we will attempt to draw the menu, but it
			// will only be draw if it's draw flag is set to true.  This is the main menu.
			g_Menu.Draw();

			// Swap the backbuffers to display the bitmaps to the screen
			g_Buffer.SwapBackBuffer(FALSE);
		} 

		// We want the Npcs to freely walk around so we need to move them every frame
		g_Map.MoveNpcs();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// Move the monsters randomly around the map.  The monsters will also move
		// towards the players if they are in a determined radius around them.
		g_Map.MoveMonsters();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	}

	// If the player died because they were killed, let's display a death sound
	if(!g_Player.IsAlive())
	{
		// We use PlaySound() to play our .wav files
		PlaySound("Death.wav", NULL, SND_FILENAME | SND_ASYNC);
		g_Menu.DrawMessageBox("You have perished!");
		Sleep(1000);
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

	if(g_Input.IsKeyDown(VK_ESCAPE))		// If escape was pressed
		g_Menu.BringUpMenu();				// Bring up the main menu
	else if(g_Input.IsKeyDown(VK_UP))		// If up arrow key was pressed
		g_Player.Move(kUp);					// Move the character up
	else if(g_Input.IsKeyDown(VK_DOWN))		// If down arrow key was pressed
		g_Player.Move(kDown);				// Move the character down
	else if(g_Input.IsKeyDown(VK_LEFT))		// If left arrow key was pressed
		g_Player.Move(kLeft);				// Move the character left
	else if(g_Input.IsKeyDown(VK_RIGHT))	// If right arrow key was pressed
		g_Player.Move(kRight);				// Move the character right
		else if(g_Input.IsKeyDown(VK_SPACE)) // If space bar was pressed
			g_Map.ToggleStatsFlag();		// Turn on and off the stats menu

	// Store the new current position of the player
	POINT curPosition = g_Player.GetPosition();

	// Check if the player moved by testing the current position against the last position
	if(lastPosition.x != curPosition.x || lastPosition.y != curPosition.y)
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


		// We want to draw the screen again if the character moved
		g_Map.SetDrawFlag(true);

		// Since we need to pick up items, we need to check for them just like exits, etc.
		g_Map.CheckForItem(curPosition.x, curPosition.y);

		// If we run into an NPC, we need to know so we can display dialog
		g_Map.CheckForNpc(curPosition.x, curPosition.y);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Now that we have monsters moving around we need to check to see if we
		// ran into them so we know when to attack.  Notice that this takes no
		// parameters.  This is because we do a loop inside that goes through all
		// of our party members and checks if they ran into the monster.
		g_Map.CheckForMonster();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Set the previous position of the player
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
