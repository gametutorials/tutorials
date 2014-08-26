//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 6)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "main.h"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// We added 2 lines of code in ShowLoadMenu() and ShowSaveMenu() below.
//
// g_SaveLoad.PromptForFile(true);
// g_SaveLoad.PromptForFile(false);
//
// When we pass in "true" it means we are loading a file, and "false" means
// that we want to save a file.  After the user enters a file name at the prompt, 
// depending on these boolean inputs, it will either load or save that file.
//
// 

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


///////////////////////////// DRAW STATS STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the players stats and item stats in the inventory menu screen
/////
///////////////////////////// DRAW STATS STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawStatsString()
{
	// Let's get the current selected item from the user
	CItem *pItem = g_Menu.GetSelectedItem();
	char szStats[kStatsMenuWidth]  = {0};

	// If there is an item that is selected, let's display its stats
	if(pItem)
	{
		sprintf(szStats, "LifeInc: %d          strengthInc: %d         ProtectionInc: %d", 
						  pItem->GetLifeInc(), pItem->GetStrengthInc(), pItem->GetProtectionInc());
	}
	else	// Otherwise display blank stats
	{
		sprintf(szStats, "LifeInc:             strengthInc:            ProtectionInc:  ");
	}

	// Draw the stats in the correct menu box at the bottom
	g_Menu.DrawString(szStats, (int)strlen(szStats), kStatsMenuX + 3, kStatsMenuY + 2, NULL);

	// Now let's display the current player's stats.  Let's create the string
	sprintf(szStats, "Hp: %d  Str: %d  Ptc: %d", 
			g_Player.GetHealth(), g_Player.GetStrength(), g_Player.GetProtection());

	// Draw the player's stats in the top left of the inventory menu
	g_Menu.DrawString(szStats, (int)strlen(szStats), kPlrStatsMenuX, kPlrStatsMenuY, NULL);
}


/////////////////////////////// ITEM PROMPT USE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is the function pointer for when the user wants to use an item
/////
/////////////////////////////// ITEM PROMPT USE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ItemPromptUse()
{
	// Grab the current item selected by the user with the mouse
	CItem *pItem = g_Menu.GetSelectedItem();

	// We want to check first to see if the item is equipment or just
	// a normal item.  If it's not a normal item, let's equip it.
	if(pItem->GetItemType() != kItem)
		g_Player.SetEquipment(pItem, pItem->GetItemType());
	
	// Regardless of if the item is equipment or not, we still need to
	// apply it's stats to the player's stats.
    g_Player.UseItem(pItem);
}


/////////////////////////////// ITEM PROMPT DROP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is the function pointer for when the user wants to drop an item
/////
/////////////////////////////// ITEM PROMPT DROP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ItemPromptDrop()
{
	// Get the selected item the user clicked, then drop the item
	CItem *pItem = g_Menu.GetSelectedItem();
	g_Player.DropItem(pItem);
}


/////////////////////////////// ITEM PROMPT CANCEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is the function pointer for when the user wants to cancel
/////
/////////////////////////////// ITEM PROMPT CANCEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ItemPromptCancel()
{
	// Do nothing but go back to the previous menu
}


/////////////////////////////// SHOW ITEM PROMPT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function shows the menu that allows the user to choose item options
/////
/////////////////////////////// SHOW ITEM PROMPT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowItemPrompt(CItem *pItem)
{
	// Let's reset the buttons and the menu choice since we entered a new menu
	g_Menu.ResetMenuChoice();
	g_Menu.ResetButtons();

	// Draw the surrounding dialog box for the menu
	g_Menu.DrawBox(kMenuWidth, kMenuHeight - 3, kMenuX, kMenuY);
	
	// Create buttons for each option in the prompt menu
	g_Menu.DrawString("Equip/Use", 9, kMenuX + 5, kMenuY + 3, NULL,  ItemPromptUse);
	g_Menu.DrawString("Drop",      4, kMenuX + 5, kMenuY + 6, NULL,  ItemPromptDrop);
	g_Menu.DrawString("Cancel",    6, kMenuX + 5, kMenuY + 9, NULL,  ItemPromptCancel);

	// Let's start the menu loop, which waits for input.  It will continue until 
	// the user hits escape or chooses an option from the menu.
	while(!g_Menu.EscapePressed() && !g_Menu.ChoiceMade())
	{
		// Check input from the user to know what to do in the menu
		g_Menu.HandleInput();
	}

	// If there was a choice made in the menu, let's call that buttons function pointer
	if(g_Menu.ChoiceMade())
		g_Menu.ChoiceMade()->m_function();

	// Regardless of the option, we still need to redraw the inventory screen
	DrawInventoryScreen();
}


/////////////////////////////// DRAW INVENTORY SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the main function to draw the players inventory and associated menus
/////
/////////////////////////////// DRAW INVENTORY SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawInventoryScreen()
{
	char szItem[25]   = {0};
	char szHead[80]   = {0};
	char szChest[80]  = {0};
	char szWeapon[80] = {0};
	char szFeet[80]   = {0};

	// Reset all the menu flags to start over on a new menu
	g_Menu.ResetButtons();
	g_Menu.ResetEscapeFlag();
	g_Menu.ResetMenuChoice();
	g_Menu.ResetItemChoice();

	// Let's redraw the map so it destroys the previous menu artifacts.  We then
	// want to draw new dialog boxes to house the inventory and stats information.
	g_Map.Draw();
	g_Menu.DrawBox(kEqMenuWidth, kEqMenuHeight, kEqMenuX, kEqMenuY);
	g_Menu.DrawBox(kInvMenuWidth, kInvMenuHeight, kInvMenuX, kInvMenuY);
	g_Menu.DrawBox(kStatsMenuWidth, kStatsMenuHeight, kStatsMenuX, kStatsMenuY);

	// If the user has anything equipped, let's get pointers to those items
	CItem *pHead   = g_Player.GetEquipment(kHead);
	CItem *pChest  = g_Player.GetEquipment(kChest);
	CItem *pWeapon = g_Player.GetEquipment(kWeapon);
	CItem *pFeet   = g_Player.GetEquipment(kFeet);

	// We there is an item equipped then we want to insert it's name in the EQ menu
	if(pHead)		sprintf(szHead,   "Head: %s", pHead->GetItemName());
	else			sprintf(szHead,   "Head: ");
	if(pChest)		sprintf(szChest,  "Chest: %s", pChest->GetItemName());
	else			sprintf(szChest,  "Chest: ");
	if(pWeapon)		sprintf(szWeapon, "Weapon: %s", pWeapon->GetItemName());
	else			sprintf(szWeapon, "Weapon: ");
	if(pFeet)		sprintf(szFeet,   "Feet: %s", pFeet->GetItemName());
	else			sprintf(szFeet,	  "Feet: ");

	// Now we draw the current player's equipment and pass in the item pointers for clicking on
	g_Menu.DrawString(szHead,   (int)strlen(szHead),   kEqMenuX + 3, kEqMenuY + 5,  pHead);
	g_Menu.DrawString(szChest,  (int)strlen(szChest),  kEqMenuX + 3, kEqMenuY + 9,  pChest);
	g_Menu.DrawString(szWeapon, (int)strlen(szWeapon), kEqMenuX + 3, kEqMenuY + 13, pWeapon);
	g_Menu.DrawString(szFeet,   (int)strlen(szFeet),   kEqMenuX + 3, kEqMenuY + 17, pFeet);

	// Let's draw the stats screen that goes at the bottom of the inventory menu
	DrawStatsString();

	// Go through all of the items in our inventory and draw them
	for(int i = 0; i < g_Player.GetInventorySize(); i++)
	{
		// Get the current item and set it's default position
		CItem *pInvItem = g_Player.GetItem(i);
		int xOffset = 3, yOffset = i + 2;

		// If the current column has more items than it should hold we need to
		// go to the next column on the right and start drawing the next items.
        if(i >= kMaxItemHeight)
		{
			// Increase the x offset to a new column and reset the y 
			// offset to start at the top of the next column.  
			xOffset += kMaxItemWidth * (i / kMaxItemHeight);
			yOffset = i - ((i/kMaxItemHeight) * kMaxItemHeight) + 2;
		}

		// Get the item's name and create a button for it, then display it to the screen
		sprintf(szItem, "%s", pInvItem->GetItemName());
		g_Menu.DrawString(szItem, (int)strlen(szItem), kInvMenuX + xOffset, kInvMenuY + yOffset, pInvItem);
	}
}


////////////////////////////// SHOW INVENTORY MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This displays the inventory screen of the player
/////
////////////////////////////// SHOW INVENTORY MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowInventoryMenu()
{
	// Draw the dialog boxes and associated data inside them
	DrawInventoryScreen();

	// Store an item pointer which is set every time the user clicks on a new item
	CItem *pItem = NULL;

	// Let's loop until we get some valid input from the user
	while(!g_Menu.EscapePressed())
	{
		// Strange, but let's loop again :)  This loop handles prompts
		while(!g_Menu.EscapePressed() && !g_Menu.ChoiceMade())
			g_Menu.HandleInput();

		// Check if the item isn't the same as the last item
		if(g_Menu.GetSelectedItem() && pItem != g_Menu.GetSelectedItem())
		{
			// Get the selected item and draw the stats for that item at the bottom
			pItem = g_Menu.GetSelectedItem();
			DrawStatsString();
			g_Menu.ResetMenuChoice();
		}
		// If the item is the same, let's display a prompt to the user
		else if(pItem && pItem == g_Menu.GetSelectedItem())
		{
			// Display a prompt, which will do a new input loop.  After that is done
			// it will come back here and we will redraw the inventory menu again.
			ShowItemPrompt(pItem);
			DrawInventoryScreen();
			pItem = NULL;	// Reset the last clicked on item
		}
	}

	// Let's redraw the main menu after we have chosen to exit from the inventory menu
	g_Menu.RedrawMenu();
}


///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a load prompt to input a saved file to load
/////
///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowLoadMenu()
{	


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// We pass in "true" to our prompt function to say that we will load a game file
	g_SaveLoad.PromptForFile(true);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a prompt to input a file name for the save game
/////
///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowSaveMenu()
{


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// We pass in "false" to our prompt function to say that we will save a game file
	g_SaveLoad.PromptForFile(false);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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

	// Create and draw 4 buttons for the main menu
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

	// If the left mouse button is pressed, let's check if we click on a button
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


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added 2 lines of code in ShowLoadMenu() and ShowSaveMenu()
// to prompt the user to load or save a file.  Now when the user
// clicks on the load or save option in the main menu they will
// be able to be prompted for a file name to either load or save.
//
// Next, how about we take a look at the Player.cpp file for the
// actual code that will save the player's data.  We can then
// finally go to SaveLoad.cpp, which handles the main save/load
// controller functions.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials   
