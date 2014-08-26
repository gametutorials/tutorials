#ifndef _MAIN_H				// If we haven't included this file							
#define _MAIN_H				// Set a flag saying we included it

#include <vector>			// Include the STL vector class information
#include <iostream>			// Include the standard C++ file
#include <string>			// Include this for the string class
#include <windows.h>		// We need to include windows.h for win32 calls
#include <mmsystem.h>		// This is included for when we will use PlaySound() later
#include "bitmap.h"			// Include the bitmap function prototypes first
#include "resource.h"		// Include our resource file for our menu
using namespace std;						// We are using the standard namespace for C++

#include "Map.h"							// Include our map information for our variables
#include "Player.h"							// Include the player information

const int kScreenWidth  = 640;				// We want our screen width 800 pixels
const int kScreenHeight = 480;				// We want our screen height 600 pixels

const int kMenuOffset	= 16;				// The pixel offset for the menu bar

// Create an externed map class variable for handling our maps
extern CMap g_Map;

// Allow other files to access the player's data by creating a global externed player object
extern CPlayer g_Player;

extern HWND  g_hWnd;									// This is the handle for the window
extern HINSTANCE g_hInstance;							// This holds our window hInstance

// Create a double buffer class variable for both our windows
extern CBuffer g_Buffer;

// Create an extern to our global variable that stores our current cursor position
extern POINT g_cursorPos;


// This is our simple class that stores the information about user input
class CInput
{
public:

	// These are our simple GET functions to check for input.  Instead of
	// depending on the windows message loop to check for input message, we
	// check them manually by using GetAsyncKeyState().  With this function we
	// can check the input for keyboard and for the mouse immediately.  We just
	// pass in the virtual key code and do a binary "and" (&) to return a bool-like result.
	int IsLeftMousePressed()			{ return GetAsyncKeyState(VK_LBUTTON) & 0x8000; }
	int IsRightMousePressed()			{ return GetAsyncKeyState(VK_RBUTTON) & 0x8000;	}
	int IsKeyDown(int keyCode)			{ return GetAsyncKeyState(keyCode) & 0x8000;	}	

	// Since we will have some loops in the game that don't allow for windows to check
	// messages every frame, we created a function to call within those loops to still
	// allow windows to keep track of basic windows events, like closing/moving the window.
	int HandleWindowsMessages();
};

// We extern the global input object after we defined the class
extern CInput g_Input;


// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// This function is our call back window procedure for our tool bar window
LRESULT CALLBACK WndProcPopup(HWND, UINT, WPARAM, LPARAM);

// This initializes our main application specific data
void Init(HWND hWnd);

// This handles the input from the user
void HandleGameInput();

// This controls our main program loop
WPARAM MainLoop();

// This frees memory and quits the program
void DeInit();

#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added some extern variables and also a class that stores the
// information regarding input from the user, CInput.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
