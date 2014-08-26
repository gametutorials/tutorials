//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 3)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "main.h"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this version we added the code that handles the inventory menu screen.
// When the user accesses the main menu by ESC, they can then click on the
// Inventory option with the mouse and bring up the inventory screen.  That
// code is in this file.
//
//

// We put a prototype for this function up top because it is used by 
// ShowItemPrompt(), which is defined before DrawInventoryScreen() is defined.
void DrawInventoryScreen();

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////// DRAW STATS STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws the players stats and item stats in the inventory menu screen
/////
///////////////////////////// DRAW STATS STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawStatsString()
{
	// These stats will show up in our inventory menu on the left side.
	// This is so the user can see the player's stats while they change
	// their equipment.  It also displays the stats of an item that is
	// clicked on.  These stats will be on the bottom of the inventory menu.

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
	// In this menu the user will see a menu popup and have 3 options to
	// either equip or use an item, drop an item, or just cancel.

	// Let's reset the buttons and the menu choice since we entered a new menu
	g_Menu.ResetMenuChoice();
	g_Menu.ResetButtons();

	// Draw the surrounding dialog box for the menu
	g_Menu.DrawBox(kMenuWidth, kMenuHeight - 3, kMenuX, kMenuY);
	
	// Create buttons for each option.  We pass in 3 functions as function pointers
	// as the last parameter.  This will make it so when they user chooses that option,
	// it will call that function associated with that option.  Function pointers are great :)
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

	// Regardless of the option, we still need to redraw the inventory screen since
	// the user made an option or cancel and wants to go back to the main inventory menu.
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

	// Now we get to the more complicated issue of display all of the player's items.
	// We have a cap on max items, so we only will draw about 3 columns of items.
	// After that runs up, we just won't be able to pick up any items.  So, below
	// we just go through and create buttons for each item, then display them in
	// their order in our inventory dialog box on the top right.

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
			// Increase the x offset to a new column and reset the y offset to start
			// at the top of the next column.  To find the x offset we just find out
			// how many times the max item height divides into the current item index (i),
			// then times that by the max item width to move over to a new column.  To
			// get the y, get the same info with the index divided by the max height,
			// then times that by the max height.  We then subtract that value from the
			// current index (i), with a padding of 2.  If it's hard at first to wrap
			// your head around math like this, it's okay.  Even before programming things
			// like this we need to sit and think about it for a bit to see what we need
			// as well :)  Just throw some test values in their and you will understand
			// why it formats the way we want it to.
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
	// This is the function that is the main controller for the inventory menu.
	// This is passed in as a function pointer in the Draw() function of our main menu.
	// What we want to do here is display the different dialog boxes with their data
	// inside, then check and handle the input the user does.  This could either be
	// clicking on an item, or escaping back to a previous menu with ESC.

	// Draw the dialog boxes and associated data inside them
	DrawInventoryScreen();

	// Store an item pointer which is set every time the user clicks on a
	// new item.  We use this to see if the user clicked the same item and wants
	// to do an action with that item.
	CItem *pItem = NULL;

	// Let's loop until we get some valid input from the user. There is actually
	// another loop inside this loop because we want to continue to show the
	// inventory menu until we escape from it, not just choosing to drop or
	// use an item.  We need to create another loop to handle the prompts
	// regarding the clicks of an item from the user.
	while(!g_Menu.EscapePressed())
	{
		// Strange, but let's loop again :)  This loop handles prompts
		while(!g_Menu.EscapePressed() && !g_Menu.ChoiceMade())
			g_Menu.HandleInput();

		// In order to check for a second click from the user, we store
		// an item pointer and check it against the current item that is
		// clicked on.  If they are the same we want to display a prompt to
		// to user.  If they aren't the same, then we just display the items stats.

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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a load prompt to input a saved file to load
/////
///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowLoadMenu()
{	
	// We currently haven't programmed the inventory so we just redraw the menu
	// when this option is chosen in the main menu.
	g_Menu.RedrawMenu();
}


///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a prompt to input a file name for the save game
/////
///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowSaveMenu()
{
	// We currently haven't programmed the inventory so we just redraw the menu
	// when this option is chosen in the main menu.
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
// As you can see we added quite a bit of functions for our inventory menu.
// We used function pointers attached to our special buttons, which allow
// us to set up functions that will run when the user chooses a certain
// menu options.
// 
// Here is the current flow of input:
// 
// Main Menu:  Draw()
//		Inventory:	ShowInventoryMenu()
//				Clicked An Item: ShowItemPrompt()
//						Equip/Use: ItemPromptUse()
//						Drop: ItemPromptDrop()
//						Cancel: ItemPromptCancel()	// Does nothing
//		Load Game:	ShowLoadMenu()	// Currently does nothing
//		Save Game:	ShowSaveMenu()	// Current does nothing
//		Quit:	Quit()
//
// This is the flow of input for our menu system.  When you click on the main
// menu options, it runs the function stored with that button, and so forth.
// The cancel functions never need to do anything because it will just take you
// back to the previous menu.
//
// In a later tutorial we will add the save and load features for our main menu.
// From here you can skip over to Player.cpp and see what we have done to allow
// items to be added and taken away from the player.  
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials   
