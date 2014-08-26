//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 5)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

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
// We just stick that text file in our Items\ directory.  Then, when ever
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
// in your own project.  In this tutorial you can find the shop keeper right
// next to the Governor's house.  Previously you couldn't take to him because
// we hadn't written code for shopping yet.
//
// To create the shop menu, we inherit from the CMenu class, since most of
// the functionality is the same.  This allows for good code re-use.  We
// added 2 new files for the shop code, ShopKeeper.cpp and ShopKeeper.h.
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Toggle the main menu (Or escapes a menu) and escapes/continues the dialog
// Left Mouse Button - This chooses the options from the main menu
//					   - If you click twice on an item in the inventory menu it will
//					     bring up a prompt to use, drop or cancel.
// Space Bar - This toggles on and off the stats screen in the bottom left of the map.
//			   This will also escape/continue the dialog.
// To Attack - Run into a monster with the arrow keys
//
// There is nothing really new in this file except the declaration of the global 
// CShop object, so you can immediately jump over to ShopKeeper.cpp to go over 
// the rest of the tutorial.
//
//

// This is our global shop object for handling our shopping
CShop g_Shop;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object
CBuffer g_Buffer;								// Create a buffer for the main window
CAction g_ActionKeys;							// This is our global action key object
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

	// Now that we have a party, we need to add the normal player to that party
	g_Player.AddPlayerToParty(&g_Player);

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

		// Move the monsters randomly around the map  
		g_Map.MoveMonsters();
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
		// Update the party positions according to the player position
		g_Player.MoveParty();

		// We want to draw the screen again if the character moved
		g_Map.SetDrawFlag(true);

		// Since we need to pick up items, we need to check for them just like exits, etc.
		g_Map.CheckForItem(curPosition.x, curPosition.y);

		// If we run into an NPC, we need to know so we can display dialog
		g_Map.CheckForNpc(curPosition.x, curPosition.y);

		// Check if we ran into a monster
		g_Map.CheckForMonster();

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
