//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 2)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "main.h"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This file holds the code that makes up our menus.  
//
//
//


///////////////////////////////// CBUTTON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for our CButton class
/////
///////////////////////////////// CBUTTON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CButton::CButton():m_width(0), m_pItem(NULL)
{
	memset(&m_position, 0, sizeof(COORD));
	m_strText = "";
}


///////////////////////////////// SET BUTTON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a button for text that the mouse can click on in a menu
/////
///////////////////////////////// SET BUTTON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CButton::SetButton(string strText, int width, int x, int y, CItem *pItem, void (*function)())
{
	// This function allows us to create a button with some text, the width
	// of the button (the string length usually), the position of the button,
	// and item that is possibly associated with that button (like for shops
	// or inventory, and finally a function that will be executed when we click
	// on the button.  This is done with function pointers.  If you are still
	// fuzzy on what those are, it's basically a way to pass in a function
	// that is associated with this button.  It makes it modular.  For instance,
	// if you go to the main menu in the game, you'll notice that there is
	// 4 options to choose with the mouse, Inventory, Load, Save and Quit.
	// Well, we created 4 functions for each of those, and instead of doing
	// a bunch of checks for each button to see which function we should call,
	// we just set it up so we can automatically assign a function to a button.
	// That is the beauty of function pointers.  It's very modular and less code.

	// Below we fill in the info for this button
	m_strText = strText;
	m_width = width;
	m_position.X = x; m_position.Y = y;
	m_pItem = pItem;
	m_function = function;
}


///////////////////////////////// CMENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for our CMenu class
/////
///////////////////////////////// CMENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMenu::CMenu():m_bShowMenu(false),m_bEscapePressed(false)
{
	m_menuChoice = NULL;
}


////////////////////////////// SHOW INVENTORY MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This displays the inventory screen of the player
/////
////////////////////////////// SHOW INVENTORY MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowInventoryMenu()
{
	// We currently haven't programmed the inventory so we just redraw the menu
	// when this option is chosen in the main menu.
	g_Menu.RedrawMenu();
}


///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a load prompt to input a saved file to load
/////
///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowLoadMenu()
{	
	// We currently haven't programmed the saving and loading so we just 
	// redraw the menu when this option is chosen in the main menu.
	g_Menu.RedrawMenu();
}


///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a prompt to input a file name for the save game
/////
///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowSaveMenu()
{
	// We currently haven't programmed the saving and loading so we just 
	// redraw the menu when this option is chosen in the main menu.
	g_Menu.RedrawMenu();
}


///////////////////////////////// QUIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This quits the game
/////
///////////////////////////////// QUIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Quit()
{
	// Quit the game if we choose quit from the main menu
	exit(0);
}


///////////////////////////////// REDRAW MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This resets the menu and redraws it
/////
///////////////////////////////// REDRAW MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::RedrawMenu()
{
	ResetEscapeFlag();
	ResetMenuChoice();
	ResetItemChoice();
	ResetButtons();
	BringUpMenu();
	g_Map.SetDrawFlag(true);
	g_Map.Draw();
	g_Menu.Draw();
}


///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the menu and checks for input
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::Draw()
{
	// If we don't need to draw the menu, quit
	if(!m_bShowMenu) return;

	// Draw the main menu with it's preset dimensions
	DrawBox(kMenuWidth, kMenuHeight, kMenuX, kMenuY);

	// Create and draw 4 buttons for the main menu.  We don't set an item associated
	// with these buttons, but we do assign a function to them, which will execute
	// when the button is clicked on (see CheckForButton() function below).
	DrawString("Inventory", 9, kMenuX + 5, kMenuY + 3, NULL,  ShowInventoryMenu);
	DrawString("Load Game", 9, kMenuX + 5, kMenuY + 6, NULL,  ShowLoadMenu);
	DrawString("Save Game", 9, kMenuX + 5, kMenuY + 9, NULL,  ShowSaveMenu);
	DrawString("Quit",      4, kMenuX + 5, kMenuY + 12, NULL, Quit);

	// Loop until the user either makes a choice or hits escape
	while(!m_bEscapePressed && !m_menuChoice)
	{
		// Check for user input
		HandleInput();
	}

	// Reset some member variables and redraw the screen before executing a function pointer
	m_bEscapePressed = false;
	m_bShowMenu = false;

	g_Map.SetDrawFlag(true);
	g_Map.Draw();

	// If there is a menu choice, let's call the function associated with that button
	if(m_menuChoice)
		m_menuChoice->m_function();
}


////////////////////////////// DRAW MESSAGE BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a centered message box to the screen with no typewriter effect
/////
////////////////////////////// DRAW MESSAGE BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::DrawMessageBox(char *szMessage)
{
	// Get the length of the string and then calculate a centered x position
	int length = (int)strlen(szMessage);
	int xPos = kMapWidth / 2 - length / 2;
		
	// Draw a simple dialog box for only 1 line of text
	DrawBox(length + 4, 4, xPos, kMenuY);
	DrawString(szMessage, length, xPos + 2, kMenuY + 2);
}


///////////////////////////////// DRAW STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a string to the screen with the option of creating a button
/////
///////////////////////////////// DRAW STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::DrawString(char *szText, int length, int x, int y, CItem *pItem, void (*function)())
{
	// Set the string colors to white text and a blue background
	WORD dialogColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY |
					   BACKGROUND_BLUE | BACKGROUND_INTENSITY;

	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwResult = 0;

	COORD position = {x, y};

	// Draw the text to the screen and fill the text with it's attribute
	WriteConsoleOutputCharacter(hOutput, szText, length, position, &dwResult);
	FillConsoleOutputAttribute(hOutput, dialogColor, length, position, &dwResult);

	// If there is no data for a button, don't create one and return
	if(!function && !pItem) return;

	// Create a button with an associated item or function for that button.
	// We then want to add it to our vector button list.
	CButton button;
	button.SetButton(szText, length, x, y, pItem, function);
	m_vButtons.push_back(button);
}


///////////////////////////////// DRAW BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a stylish box for our dialog and message boxes
/////
///////////////////////////////// DRAW BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::DrawBox(int width, int height, int x, int y)
{
	char szTop[81]	  = {0}; //"+----------------------------------------------------------+";
	char szMiddle[81] = {0}; //"¦                                                          ¦";
	char szBottom[81] = {0}; //"+----------------------------------------------------------+";

	// Since there are some weird Ascii characters that we wanted to use for this box,
	// we can't type them in, but we need to use their Ascii number and convert it to
	// a character.  This is what we do in this function.

	// GO through and create the top part of the dialog box and store it in string
	for(int i = 0; i < width; i++)
		sprintf(&szTop[i], "%c", 205);

	// Save the bottom part as the same as the top since they look alike except corners
	strcpy(szBottom, szTop);

	// Now we need to add the special corners to our dialog box, along with the middle section
	szTop[0]		 = (char)201;		// + 
	szTop[width - 1] = (char)187;		// +

	szMiddle[0]			= (char)186;	// ¦
	szMiddle[width - 1] = (char)186;	// ¦

	szBottom[0]			= (char)200;	// +
	szBottom[width - 1] = (char)188;	// +
	
	// Draw the top part of the dialog box
	DrawString(szTop, width, x, y);

	// Since the dialog box can have different heights, we need to basically draw
	// the middle part as many times as there is lines of dialog, plus padding.
	for(int i = 0; i < height; i++)
		DrawString(szMiddle, width, x, y + 1 + i);

	// Finally, once we are done with the top and middle parts, draw the bottom section
	DrawString(szBottom, width, x, y + height);
}


///////////////////////////////// HANDLE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks and handles our input for the menu
/////
///////////////////////////////// HANDLE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::HandleInput()
{
	// If there is no input events, return
	if(!g_Input.CheckInput())
		return;

	// If there is a keyboard event and the key is being pressed down...
	if(g_Input.IsKeyboardEvent() && g_Input.IsKeyDown())
	{
		// Set the escape key pressed flag as true if we hit escape
		if(g_Input.GetKeyCode() == VK_ESCAPE)
			m_bEscapePressed = true;
	}

	// If the left mouse button is pressed, let's check if we clicked on a button
	if(g_Input.IsLeftMousePressed())
	{
		g_Input.ResetLeftMouse();
		CheckForButton(g_cursorPos.X, g_cursorPos.Y);
	}
}

///////////////////////////////// CHECK FOR BUTTON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a position and checks if that position is a button position
/////
///////////////////////////////// CHECK FOR BUTTON \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::CheckForButton(int x, int y)
{
	// Reset the menu choice which holds the button chosen
	m_menuChoice = NULL;

	// Go through all the buttons and search for a button that matches the x and y position
	for(int i = 0; i < (int)m_vButtons.size(); i++)
	{
		// Get the position and width of the current button
		COORD position = m_vButtons[i].GetPosition();
		int width = m_vButtons[i].GetWidth();

		// Check if the x and y position is between a text button by using the width
		if((position.Y == y) && (x >= position.X && x < position.X + width))
		{
			// If there is an item associated with this button, store that item for later
			if(m_vButtons[i].GetItem())
			{
				CItem *pTest = m_vButtons[i].GetItem();
				SetSelectedItem(m_vButtons[i].GetItem());
			}

			// Store a pointer to the button that we clicked in our menu choice variable
			m_menuChoice = &m_vButtons[i];
			break;	// Quit looking for buttons
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// This file holds most of our functions that deal with our menus.
// We have 2 class defined, one is CButtons, which can be used to
// allow the user to choose things with the mouse.  We also added
// CMenu to handle the main drawing and input for our menus.  We
// have other functions that aren't part of a class, but act as
// function pointers to be executed when the user clicks on a text
// button.  Buttons are created by giving a length and position
// of the button.  We then check the mouse coordinates and see if
// the position of the mouse is between the button's dimensions.
// Just because we don't have graphics doesn't mean we can have buttons! :)
// We also can assign items and functions to these buttons.
//
// In the next tutorial we will add the inventory system, which
// will deal a great deal with the menu code.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials   
