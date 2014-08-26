//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 6)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

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

// Create our global variables for our open file dialog box
OPENFILENAME g_OpenInfo = {0};
char g_szFileName[MAX_PATH] = {0};

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object
CBuffer g_Buffer;								// Create a buffer for the main window
CAction g_ActionKeys;							// This is our global action key object
CShop g_Shop;									// This is our global shop object
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

	// Here we initialize our open file information to use for the save/open dialog boxes
	g_OpenInfo.lStructSize = sizeof(OPENFILENAME);			// Set the size of the structure
	g_OpenInfo.nMaxFile = MAX_PATH;							// Set the max characters for a file name
	g_OpenInfo.lpstrFile = g_szFileName;					// Give a string to store the file name
	g_OpenInfo.lpstrFilter = "Save Files (*.sav)\0*.sav";	// Only accept .sav files to load
	g_OpenInfo.hwndOwner = g_hWnd;							// Assign the window owner and give it desired flags
	g_OpenInfo.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// Now that have save and load code, let's make it so the window menu 
	// and the main game menu can allow this feature.  We use the normal
	// GetOpenFileName() and GetSaveFileName() to allow the user to select
	// the file they want to save or load.

	else if(menuID == ID_FILE_OPENUP)			// If the user chose File->Open
	{	
		// Have windows bring up the Open File dialog box and Load the file chosen
		if(GetOpenFileName(&g_OpenInfo))
			g_SaveLoad.Load(g_OpenInfo.lpstrFile);	
	}
	else if(menuID == ID_FILE_SAVEIT)			// If the user chose File->Save
	{
		// Pull up a dialog box to allow the user to type in their save game, then save it
		if(GetSaveFileName(&g_OpenInfo))
			g_SaveLoad.Save(g_OpenInfo.lpstrFile);
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// In this file we added the global g_SaveLoad object, as well as the
// windows menu support for saving and loading.  Either the player
// can use the windows menus, or hit escape and use the main game 
// menu that we created with bitmaps.  From here we can jump
// directly to Map.cpp to see the next small additions.  To use the
// windows dialog boxes for saving and loading, we needed to also add
// the g_OpenInfo object, and be sure to initialize it in Init().
//
// This is the end of the Rpg tutorial series.  The topics that
// were gone over in this tutorial series should have been simple
// enough to implement them in your own projects.  Of course, these
// are just simple ways to implement in a game, but do not reflect
// the most efficient or bug free.  To make the code more robust
// you will need to add the rest.  This should have been a HUGE push
// to get you on that road. The sky is the limit!
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
