#ifndef _MENU_H
#define _MENU_H


// Include the main header file
#include "Main.h"				

// Main Menu Defines
const int kMenuWidth = 152;
const int kMenuHeight = 200;
const int kMenuX = 250;
const int kMenuY = 100;

// Font Menu Defines
const int kFontWidth	= 8;
const int kFontHeight	= 14;
const int kCharPerLine  = 32;
const int kFontLines    = 4;

// EQ Menu Defines
const int kEqMenuX = 50;
const int kEqMenuY = 50;
const int kEqMenuWidth = 252;
const int kEqMenuHeight = 250;

// Inventory Menu Defines
const int kInvMenuX = kEqMenuX + kEqMenuWidth;
const int kInvMenuY = kEqMenuY;
const int kInvMenuWidth = 300;
const int kInvMenuHeight = kEqMenuHeight;

// Stats Menu Defines
const int kStatsMenuX = kEqMenuX;
const int kStatsMenuY = kEqMenuY + kEqMenuHeight;
const int kStatsMenuWidth = kEqMenuWidth + kInvMenuWidth;
const int kStatsMenuHeight = kTileHeight * 2;
const int kPlrStatsMenuX = kEqMenuX + kTileWidth;
const int kPlrStatsMenuY = kEqMenuY + kTileHeight-3;

// Inventory Defines
const int kMaxItemHeight = 14;
const int kMaxItemWidth  = 3;
const int kMaxItems		 = kMaxItemHeight * 3;

const char kMenuImage[]  = "Tiles\\Menu.bmp";
const char kMenuFont[]   = "Tiles\\Font.bmp";
const char kPlayerBust[] = "Tiles\\PlayerBust.bmp";

const int kPlayerBustX	 = kEqMenuX + kTileWidth;
const int kPlayerBustY	 = kEqMenuY + 45;

// This is the menu's opacity (see through value), 0 through 255, 255 being no opacity
const int kMenuOpacity	 = 200;	


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

	void SetPosition(int x, int y)	{ m_position.x = x; m_position.y = y; }
	POINT GetPosition()				{ return m_position; }

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

	POINT m_position;			// The position of the button
	string m_strText;			// The text in the button
	int m_width;				// The width of the button

	CItem *m_pItem;				// The item associated with the function
};


// This is our class for our menu object
class CMenu
{
public:

	// Our standard constructor and deconstructor
	CMenu();
	~CMenu();

	// This sets a flag to tell the menu to draw
	void BringUpMenu()	{ m_bShowMenu = true; }

	// This tells us if the menu is currently open
	bool IsMenuOpen()	{ return m_bMenuOpen; }

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

	HBITMAP m_menuImage;
	HBITMAP m_fontImage;
	bool m_bShowMenu;					// Stores the flag which determines if we draw or not
	bool m_bMenuOpen;					// Stores the flag which tells us if the menu is open
	bool m_bEscapePressed;				// Stores if escape was pressed or not
	CItem *m_pItem;						// Stores a selected item if any (for inventory, etc...)
	CButton *m_menuChoice;				// This stores a button we clicked
	vector<CButton> m_vButtons;			// This is our list of buttons for the current menu
};


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// These below are prototyped to allow the shop menu to use them.

// This does nothing, but acts as a filler for a function pointer
void ItemPromptCancel();

// We put a prototype for this function up top because it is used by ShowItemPrompt()
void DrawInventoryScreen();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added 2 prototypes to this file so that the shop menu can use them.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
