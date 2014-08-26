#ifndef _MAIN_H
#define _MAIN_H

#include <vector>							// This is needed for the STL vector class
#include <iostream>							// Include our basic C++ header file
#include <string>							// This is included to use the STL string class
#include <windows.h>						// This is for the windows console functions
#include <conio.h>							// This is included for getch()
#include <mmsystem.h>						// This is included for PlaySound()
using namespace std;						// We are using the standard namespace for C++

#include "Map.h"							// Include our map information for our variables
#include "Player.h"							// Include the player information
#include "CFmod.h"							// Include our Fmod engine code
#include "Menu.h"							// Include our menu class info


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

#include "ShopKeeper.h"						// Include the shop keeper menu data

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


#include "ActionKeys.h"						// Include the action keys class (CAction)


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

extern CShop g_Shop;						// Make the global shop object available

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// Create an externed map class variable for handling our maps
extern CMap g_Map;

// Allow other files to access the player's data by creating a global externed player object
extern CPlayer g_Player;

// Here we extern our global music and menu object
extern CFmod g_Music;
extern CMenu g_Menu;

// Let's make our action keys object global
extern CAction g_ActionKeys;

// Create an extern to our global variable that stores our current cursor position
extern COORD g_cursorPos;

// This is our simple class that stores the information about user input
class CInput
{
public:

	// The basic constructor
	CInput();

	// This function checks to see if there were any input from the user and stores it
	bool CheckInput();

	// This stores the mouse input that was done by the user
	void CheckMouseInput();

	// These are our simple GET functions for retrieving information about input
	bool IsLeftMousePressed()		{ return m_bLeftMousePressed;	}
	bool IsRightMousePressed()		{ return m_bRightMousePressed;	}
	bool IsMouseEvent()				{ return (m_inputRecord.EventType == MOUSE_EVENT);  }
	bool IsKeyboardEvent()			{ return (m_inputRecord.EventType == KEY_EVENT);	}
	WORD GetKeyCode()				{ return m_virtualKeyCode;		}
	BOOL IsKeyDown()				{ return m_bKeyDown;			}

	// These reset the stored data in our input object for mouse buttons
	void ResetLeftMouse()			{ m_bLeftMousePressed = false;	}
	void ResetRightMouse()			{ m_bRightMousePressed = false; }

	// This holds all the input information from the user
	INPUT_RECORD m_inputRecord;

private:

	bool m_bLeftMousePressed;		// This tells if the left mouse button is down or not
	bool m_bRightMousePressed;		// This tells if the right mouse button is down or not
	WORD m_virtualKeyCode;			// This is the key the user pressed (VK_LEFT, 'A', etc...)
	BOOL m_bKeyDown;				// This tells if a key is down or not (BOOL for conformity)
};

// We extern the global input object after we defined the class
extern CInput g_Input;

#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added the header file for our shop class/data and also
// an extern to the global g_Shop object.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
