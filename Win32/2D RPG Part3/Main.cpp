//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 3)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

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
// You can find some items inside the game in your house.  When you pick them up and leave
// the room, they will reappear again if you go back into the house.  This will be fixed later.
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
// the way of game play, so you wouldn't always want it on.  
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Toggle the main menu (Or escapes a menu) and escapes/continues dialog
// Left Mouse Button - This chooses the options from the main menu
//					   - If you click twice on an item in the inventory menu it will
//					     bring up a prompt to use, drop or cancel.
// Space Bar - This toggles on and off the stats screen in the bottom left of the map
//			   Space bar also escapes/continues dialog.
//
// Let's go pick up some items!  (Note that when you pick up an item and leave the map
// it's back there on the map.  When we get into saving and loading of maps we will discuss
// how to get rid of this)
//
//

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
	}

	// If the player died because they were killed, let's display a death sound.
	// This of course can't be heard until we start fighting monsters :)
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// If we hit space let's toggle on and off the stats at the botton of the screen
		else if(g_Input.IsKeyDown(VK_SPACE))
			g_Map.ToggleStatsFlag();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Store the new current position of the player
	POINT curPosition = g_Player.GetPosition();

	// Check if the player moved by testing the current position against the last position
	if(lastPosition.x != curPosition.x || lastPosition.y != curPosition.y)
	{
		// We want to draw the screen again if the character moved
		g_Map.SetDrawFlag(true);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Since we need to pick up items, we need to check for them just like exits, etc.
		g_Map.CheckForItem(curPosition.x, curPosition.y);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			

		// If we run into an NPC, we need to know so we can display dialog
		g_Map.CheckForNpc(curPosition.x, curPosition.y);

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
