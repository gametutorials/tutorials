#ifndef _MENU_H
#define _MENU_H

// Include the main header file
#include "Main.h"				

// Main Menu Defines
const int kMenuWidth = 20;
const int kMenuHeight = 15;
const int kMenuX = 30;
const int kMenuY = 15;

// EQ Menu Defines
const int kEqMenuX = 4;
const int kEqMenuY = 5;
const int kEqMenuWidth = 32;
const int kEqMenuHeight = 20;

// Inventory Menu Defines
const int kInvMenuX = 36;
const int kInvMenuY = 5;
const int kInvMenuWidth = 40;
const int kInvMenuHeight = 20;

// Stats Menu Defines
const int kStatsMenuX = 4;
const int kStatsMenuY = 26;
const int kStatsMenuWidth = 72;
const int kStatsMenuHeight = 4;
const int kPlrStatsMenuX = kEqMenuX + 3;
const int kPlrStatsMenuY = kEqMenuY + 2;

// Inventory Defines
const int kMaxItemHeight = 17;
const int kMaxItemWidth  = 13;
const int kMaxItems		 = kMaxItemHeight * 3;


// This is our class for buttons used in our menus
class CButton
{
public:

	// The standard constructor
	CButton();

	// Below is our set and get functions for our button's data
	void SetText(string strText)	{ m_strText = strText; }
	string GetText()				{ return m_strText;    }

	void SetWidth(int width)		{ m_width = width; }
	int GetWidth()					{ return m_width;  }

	void SetPosition(int x, int y)	{ m_position.X = x; m_position.Y = y; }
	COORD GetPosition()				{ return m_position; }

	// This sets the function pointer
	void SetFunction(void (*function)())	{ m_function = function; }

	// The function pointer to be called publicly
	void (*m_function)();

	void SetTile(CItem *pItem)		{ m_pItem = pItem; }
	CItem *GetItem()				{ return m_pItem;  }

	// This creates a button with a string, width, x and y position, and the choice of
	// an associated item or function.  If not specified, the default is NULL.
	void SetButton(string strText, int width, int x, int y, CItem *pItem = NULL, void (*function)() = NULL);

private:

	COORD m_position;			// The position of the button
	string m_strText;			// The text in the button
	int m_width;				// The width of the button

	CItem *m_pItem;				// The item associated with the function
};


// This is our class for our menu object
class CMenu
{
public:

	// Our standard constructor
	CMenu();

	// This sets a flag to tell the menu to draw
	void BringUpMenu()	{ m_bShowMenu = true; }

	// This is a set and get function for the currently selected item (for inventory, etc...)
	CItem *GetSelectedItem()			{ return m_pItem;  }
	void SetSelectedItem(CItem *pItem)	{ m_pItem = pItem; }

	// This resets flags and the menu choice and redraws the map and menu
	void RedrawMenu();
	
	// This draws the main menu and puts you into a loop for checking input
	void Draw();

	// This draws a simple plain dialog box with a simple message inside
	void DrawMessageBox(char *szMessage);
	
	// This creates and draws a button to the screen
	void DrawString(char *szText, int length, int x, int y, CItem *pItem = NULL, void (*function)() = NULL);
	
	// This draws a dialog box frame
	void DrawBox(int width, int height, int x, int y);

	// This handles the main menu input
	void HandleInput();

	// This checks for a button under out mouse cursor when we clicked the left mouse button
	void CheckForButton(int x, int y);

	// These function set, get and reset flags and data
	bool EscapePressed()	{ return m_bEscapePressed;	}
	CButton *ChoiceMade()	{ return m_menuChoice;		}
	void ResetEscapeFlag()	{ m_bEscapePressed = false; }
	void ResetMenuChoice()  { m_menuChoice = NULL;		}
	void ResetItemChoice()  { m_pItem = NULL;			}
	void ResetButtons()		{ m_vButtons.clear();		}
	
protected:	// We do protected since we will inherit from this later for shops

	bool m_bShowMenu;					// Stores the flag which determines if we draw or not
	bool m_bEscapePressed;				// Stores if escape was pressed or not
	CItem *m_pItem;						// Stores a selected item if any (for inventory, etc...)
	CButton *m_menuChoice;				// This stores a button we clicked
	vector<CButton> m_vButtons;			// This is our list of buttons for the current menu
};

#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Nothing new was added to this file.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
