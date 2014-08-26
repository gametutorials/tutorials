//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 2)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

#pragma comment(lib, "Winmm.lib")				// Include this windows library for PlaySound()
#pragma comment(lib, "fmodvc.lib")				// Include the Fmod library for music

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


#include "main.h"								// Include our main header file


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this second tutorial we will jump right into more game code.  We will plug in Menu code,
// moving Npcs, dialog and music and sound.  The SoundFX won't really be used until later
// tutorials, but we will at least put in the code for playing a sound when the player
// dies.  The only sounds we will use will be an item pick up sound and a death sound.
// These all can't be coded yet since we won't probably any of that until later tutorials.  
// We can, however, add the cool music in the game immediately :)
//
// In this tutorial, the character can walk up to an Npc and talk to them.  This is done
// by running into them.  The user can then press the space key or escape to skip through 
// the dialog.  Next, you can press escape and toggle a menu.  The menu has options for 
// inventory, saving, loading and quitting, yet only the quitting option does anything.  
// It.... quits :)  The next tutorial will allow for inventory menus.  You must use the 
// mouse to choose options from the menu.  We create a menu and a button class.  This allows 
// us to create menus with buttons.  You can assign some text to a button, and when the user 
// clicks on it with the mouse, it will be directed to a function that handles the action for
// that button.  We do this by function pointers.
//
// *User input for this tutorial*
//
// Arrow Keys - Move the player
// Esc - Toggle the main menu (Or escapes a menu) and escapes/continues dialog
// Space Bar - Escapes/continues the dialog
// Left Mouse Button - This chooses the options from the main menu
//
// Play the sweet game music and let it inspire you as you check out this tutorial :)
// (Note: you do not need to look at Fmod files, as there is nothing intended to be learned
// from them.  Fmod is a music library created by FireLight Multimedia (www.Fmod.org))
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


CMap g_Map;										// This stores our global map object
CPlayer g_Player;								// This stores our global player object
CInput g_Input;									// This is the global input object
POINT g_cursorPos = {0};						// The current mouse cursor position
CBuffer g_Buffer;								// Create a buffer for the main window


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

CMenu g_Menu;									// This is our global menu object
CFmod g_Music;									// This is our global music object

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Here we load then play the starting background music
	if(!g_Music.Init("techno.mod"))
		exit(0);

	g_Music.PlaySong();

	// we need to seed our random number generator (rand()) for moving npcs.
	srand(GetTickCount());

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// Just like the g_Map.Draw() command, we will attempt to draw the menu, but it
			// will only be draw if it's draw flag is set to true.  This is the main menu.
			g_Menu.Draw();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


			// Swap the backbuffers to display the bitmaps to the screen
			g_Buffer.SwapBackBuffer(FALSE);
		} 


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// We want the Npcs to freely walk around so we need to move them every frame
		g_Map.MoveNpcs();
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Now that we have menu code, we can display a main menu when ESC is pressed
	if(g_Input.IsKeyDown(VK_ESCAPE))		// If escape was pressed
		g_Menu.BringUpMenu();
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			
			// If we run into an NPC, we need to know so we can display dialog.
			// The dialog code will be called from this function CheckForNpc().
			g_Map.CheckForNpc(curPosition.x, curPosition.y);

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
