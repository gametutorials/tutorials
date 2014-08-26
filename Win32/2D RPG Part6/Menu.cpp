//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 6)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

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
	memset(&m_position, 0, sizeof(POINT));
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
	m_position.x = x; m_position.y = y;
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
	m_menuImage = g_Buffer.LoadABitmap((LPSTR)kMenuImage);
	m_fontImage = g_Buffer.LoadABitmap((LPSTR)kMenuFont);
}


///////////////////////////////// ~CMENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the deconstructor for our CMenu class
/////
///////////////////////////////// ~CMENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMenu::~CMenu()
{
	// Free the menu image if needed
	if(m_menuImage)
	{
        DeleteObject(m_menuImage);
		m_menuImage = NULL;
	}

	// Free the font image if needed
	if(m_fontImage)
	{
        DeleteObject(m_fontImage);
		m_fontImage = NULL;
	}
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
	g_Menu.DrawString(szStats, (int)strlen(szStats), kStatsMenuX + kTileWidth, 
													 kStatsMenuY + kTileHeight-3, NULL);
	
	// Now let's display the current player's stats.  Let's create the string
	sprintf(szStats, "Hp: %d  Str: %d  Ptc: %d", 
			g_Player.GetHealth(), g_Player.GetStrength(), g_Player.GetProtection());

	// Draw the player's stats in the top left of the inventory menu
	g_Menu.DrawString(szStats, (int)strlen(szStats), kPlrStatsMenuX, kPlrStatsMenuY, NULL);
}

////////////////////////////// ITEM PROMPT USE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
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
	// Redraw the inventory screen and the map
	g_Map.SetDrawFlag(true);
	g_Map.Draw();
	DrawInventoryScreen();

	// Let's reset the buttons and the menu choice since we entered a new menu
	g_Menu.ResetMenuChoice();
	g_Menu.ResetButtons();

	// Draw the surrounding dialog box for the menu
	g_Menu.DrawBox(kMenuWidth, kTileHeight*4, kMenuX, kMenuY);
	
	// Create buttons for each option
	g_Menu.DrawString("Equip/Use", 9, kMenuX + kTileWidth, kMenuY + kTileHeight-3,  NULL, ItemPromptUse);
	g_Menu.DrawString("Drop",      4, kMenuX + kTileWidth, kMenuY + kTileHeight+29, NULL, ItemPromptDrop);
	g_Menu.DrawString("Cancel",    6, kMenuX + kTileWidth, kMenuY + kTileHeight+61, NULL, ItemPromptCancel);
	
	// Swap the backbuffers to display the bitmaps to the screen
	g_Buffer.SwapBackBuffer(FALSE);
	Sleep(100);

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

	// Let's redraw the map so it destroys the previous menu artifacts.  We then
	// want to draw new dialog boxes to house the inventory and stats information.
	g_Map.SetDrawFlag(true);
	g_Map.Draw();

	// Draw the 3 boxes that will make up the inventory screen: TopLeft, TopRight, Bottom
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
	g_Menu.DrawString(szHead,   (int)strlen(szHead),   kEqMenuX + kTileWidth, kEqMenuY + 115,  pHead);
	g_Menu.DrawString(szChest,  (int)strlen(szChest),  kEqMenuX + kTileWidth, kEqMenuY + 147,  pChest);
	g_Menu.DrawString(szWeapon, (int)strlen(szWeapon), kEqMenuX + kTileWidth, kEqMenuY + 179, pWeapon);
	g_Menu.DrawString(szFeet,   (int)strlen(szFeet),   kEqMenuX + kTileWidth, kEqMenuY + 211, pFeet);

	// Let's draw the stats screen that goes at the bottom of the inventory menu
	DrawStatsString();

	// Go through all of the items in our inventory and draw them
	for(int i = 0; i < g_Player.GetInventorySize(); i++)
	{
		// Get the current item and set it's default position
		CItem *pInvItem = g_Player.GetItem(i);
		int xOffset = kTileWidth, yOffset = kTileHeight + i * kFontHeight;

		// If the current column has more items than it should hold we need to
		// go to the next column on the right and start drawing the next items.
        if(i >= kMaxItemHeight)
		{
			// Increase the x and y position accordingly for each item
			xOffset += (kMaxItemWidth * (i / kMaxItemHeight)) * kTileWidth;
			yOffset = (i - ((i/kMaxItemHeight) * kMaxItemHeight)) * kFontHeight + kTileHeight;
		}

		// Get the item's name and create a button for it, then display it to the screen
		sprintf(szItem, "%s", pInvItem->GetItemName());
		g_Menu.DrawString(szItem, (int)strlen(szItem), kInvMenuX + xOffset, kInvMenuY + yOffset, pInvItem);
	}

	// After the items are drawn, let's load and draw a small picture of the player
	HBITMAP hPlayerBust = g_Buffer.LoadABitmap((LPSTR)kPlayerBust);
	g_Buffer.DisplayBitmap(hPlayerBust, kPlayerBustX, kPlayerBustY);

	// Swap the backbuffers to display the bitmaps to the screen
	g_Buffer.SwapBackBuffer(FALSE);
	Sleep(100);

	// Since the picture is drawn, we don't need it anymore, let's delete it
	DeleteObject(hPlayerBust);
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

		if(g_Menu.EscapePressed())
			break;

		Sleep(100);

		// Check if the item isn't the same as the last item
		if(g_Menu.GetSelectedItem() && pItem != g_Menu.GetSelectedItem())
		{
			// Get the selected item and redraw the inventory screen
			pItem = g_Menu.GetSelectedItem();
			g_Map.SetDrawFlag(true);
			g_Map.Draw();
			DrawInventoryScreen();
			g_Menu.ResetMenuChoice();
			g_Buffer.SwapBackBuffer(FALSE);
		}
		// If the item is the same, let's display a prompt to the user
		else if(pItem && pItem == g_Menu.GetSelectedItem())
		{
			// Display a prompt, which will do a new input loop
			ShowItemPrompt(pItem);
			pItem = NULL;	// Reset the last clicked on item
		}
	}

	// Let's redraw the main menu after we have chosen to exit from the inventory menu
	g_Menu.RedrawMenu();
}


///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a dialog box to choose a file to load
/////
///////////////////////////////// SHOW LOAD MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowLoadMenu()
{	


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Have windows bring up the Open File dialog box and Load the file chosen
	if(GetOpenFileName(&g_OpenInfo))
		g_SaveLoad.Load(g_OpenInfo.lpstrFile);	

	g_Menu.RedrawMenu();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This brings up a dialog box to input a file name for the save game
/////
///////////////////////////////// SHOW SAVE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ShowSaveMenu()
{


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// If haven't given our map a name yet, do a "SaveAs" and bring up the save dialog box
	if(GetSaveFileName(&g_OpenInfo))
		g_SaveLoad.Save(g_OpenInfo.lpstrFile);

	g_Menu.RedrawMenu();

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
	PostQuitMessage(0);
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
	g_Buffer.SwapBackBuffer(FALSE);
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
	DrawString("Inventory", 9, kMenuX + kTileWidth, kMenuY + 30,  NULL, ShowInventoryMenu);
	DrawString("Load Game", 9, kMenuX + kTileWidth, kMenuY + 70,  NULL, ShowLoadMenu);
	DrawString("Save Game", 9, kMenuX + kTileWidth, kMenuY + 110, NULL, ShowSaveMenu);
	DrawString("Quit",      4, kMenuX + kTileWidth, kMenuY + 150, NULL, Quit);

	// Swap the backbuffers to display the bitmaps to the screen
	g_Buffer.SwapBackBuffer(FALSE);
	Sleep(100);

	// Loop until the user either makes a choice or hits escape
	while(!m_bEscapePressed && !m_menuChoice)
	{
		// Check for user input
		HandleInput();
	}

	Sleep(100);

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
	int length = (int)strlen(szMessage) * kFontWidth + kTileWidth*2;
	int xPos = (kMapWidth*kTileWidth) / 2 - length / 2;
		
	// Draw a simple dialog box for only 1 line of text
	DrawBox(length, kTileWidth*2, xPos, kMenuY);
	DrawString(szMessage, (int)strlen(szMessage), xPos + kTileWidth, kMenuY + kTileHeight-3);

	// Swap the backbuffer to display the drawn box
	g_Buffer.SwapBackBuffer(FALSE);
}


///////////////////////////////// DRAW STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a string to the screen with the option of creating a button
/////
///////////////////////////////// DRAW STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::DrawString(char *szText, int length, int x, int y, CItem *pItem, void (*function)())
{
	// This is where we use our font image to display text to the screen.  We need to
	// go through every character and index that character into our font image.
	for(int i = 0; i < length; i++)
	{
		// Here we get the index into our font image.  We minus 32 since our font image
		// isn't set up from ASCII characters 0-32, they are ommitted (smiley faces, etc...).
		int index = (int)(szText[i]-32);
		int xOffset = (index % kCharPerLine) * kFontWidth;
		int charY = (index / kCharPerLine) * kFontHeight;

		// Next we grab the RECT offset into the font bitmap, then display that character
		RECT rCharacter = {xOffset, charY, xOffset + kFontWidth, charY + kFontHeight};
		g_Buffer.DisplayTransparentBitmap(m_fontImage, x + i*kFontWidth, y, rCharacter);
	}

	// If there is no data for a button, don't create one and return
	if(!function && !pItem) return;

	// Create a button with an associated item or function for that button.
	// We then want to add it to our vector button list.
	CButton button;
	button.SetButton(szText, length * kFontWidth, x, y, pItem, function);
	m_vButtons.push_back(button);
}


///////////////////////////////// DRAW BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a stylish box for our dialog and message boxes
/////
///////////////////////////////// DRAW BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::DrawBox(int width, int height, int x, int y)
{	
	// Below are the offsets that make up a 32x32 tile for each part of the box
	RECT rTopLeft		= {0,   0, 32, 32};
	RECT rTopMiddle		= {32,  0, 64, 32};
	RECT rTopRight		= {64,  0, 96, 32};
	RECT rLeftMiddle	= {0,  32, 32, 64};
	RECT rMiddle		= {32, 32, 64, 64};
	RECT rRightMiddle	= {64, 32, 96, 64};
	RECT rBottomLeft	= {0,  64, 32, 96};
	RECT rBottomMiddle	= {32, 64, 64, 96};
	RECT rBottomRight	= {64, 64, 96, 96};
	
	// This stores the x position of the right side of the middle tile
	int middleRight		= rMiddle.right;

	///// *** TOP PORTION OF THE BOX *** /////

	// Using are new opaque drawing functions we draw the top left corner of the box
	g_Buffer.DisplayOpaqueBitmap(m_menuImage, x, y, kMenuOpacity, rTopLeft);

	// Draw the top middle columns
	for(int i = kTileWidth; i < width - kTileWidth; i += kTileWidth)
	{
		// Check if this current middle tile position is greater than width.
		// If so, we want to subtract the overlapping offset.
		if(i + kTileWidth >= width - kTileWidth)
			rTopMiddle.right -= (i + kTileWidth) - (width - kTileWidth);

		// Draw the current middle tile on the top of the box
		g_Buffer.DisplayOpaqueBitmap(m_menuImage, x + i, y, kMenuOpacity, rTopMiddle);
	}
	
	// Draw the last tile on the top for the top right corner
	g_Buffer.DisplayOpaqueBitmap(m_menuImage, x + width-kTileWidth, y, kMenuOpacity, rTopRight);

	///// *** MIDDLE PORTION OF THE BOX *** /////

	// Go through the middle rows and draw them
	for(int tall = kTileHeight; tall < height-kTileHeight; tall += kTileHeight)
	{
		// Here we make sure that the height isn't going over the desired box height
		if(tall + kTileWidth >= height - kTileHeight)
		{
			// If so, then we need to compensate for the overlapping by subtracting
			// that offset from the bottom of each tile on this last middle row of the box.
			rLeftMiddle.bottom  -= (tall + kTileHeight) - (height - kTileHeight);
			rMiddle.bottom		-= (tall + kTileHeight) - (height - kTileHeight);
			rRightMiddle.bottom -= (tall + kTileHeight) - (height - kTileHeight);
		}

		// Draw the left side tile of the middle rows for the box
		g_Buffer.DisplayOpaqueBitmap(m_menuImage, x, y + tall, kMenuOpacity, rLeftMiddle);

		// Just like on the top row, we need to draw middle tiles to fulfill the box width
		for(int i = kTileWidth; i < width-kTileWidth; i += kTileWidth)
		{
			// Check this tile for overlap and adjust accordingly
			if(i + kTileWidth >= width - kTileWidth)
				rMiddle.right -= (i + kTileWidth) - (width - kTileWidth);

			// Draw the current middle tile in the correct x and y position (tall is for the row).
			g_Buffer.DisplayOpaqueBitmap(m_menuImage, x + i, y + tall, kMenuOpacity, rMiddle);
		}

		// Draw the middle right tile for this row
		g_Buffer.DisplayOpaqueBitmap(m_menuImage, x + width-kTileWidth, 
												  y + tall, kMenuOpacity, rRightMiddle);

		// Reset the offset for the middle tile for the next row
		rMiddle.right  = middleRight;
	}

	///// *** BOTTOM PORTION OF THE BOX *** /////

	// Draw the bottom left corner tile of the box
	g_Buffer.DisplayOpaqueBitmap(m_menuImage, x, y + height-kTileWidth, kMenuOpacity, rBottomLeft);

	// Draw middle tiles for the bottom to make up the desired width
	for(int i = kTileWidth; i < width-kTileWidth; i += kTileWidth)
	{
		// Check for overlap with this tile and adjust accordingly
		if(i + kTileWidth >= width - kTileWidth)
			rBottomMiddle.right -= (i + kTileWidth) - (width - kTileWidth);

		// Draw the current middle tile for the bottom
		g_Buffer.DisplayOpaqueBitmap(m_menuImage, 
									 x + i, y + height-kTileWidth, kMenuOpacity, rBottomMiddle);
	}

	// Finally, draw the bottom right corner of the box
	g_Buffer.DisplayOpaqueBitmap(m_menuImage, x + width-kTileWidth, 
											  y + height-kTileWidth, kMenuOpacity, rBottomRight);
}


///////////////////////////////// HANDLE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks and handles our input for the menu
/////
///////////////////////////////// HANDLE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMenu::HandleInput()
{
	// Since there will be some loops when inside the menu code, we need to
	// allow windows messages to get processed.  If we get a quit message, return 1.
	if(g_Input.HandleWindowsMessages() == -1)
	{
		m_bEscapePressed = true;
		return;
	}

	// Set the escape key pressed flag as true if we hit escape
	if(g_Input.IsKeyDown(VK_ESCAPE))
		m_bEscapePressed = true;
	
	// If the left mouse button is pressed, let's check if we clicked on a button
	if(g_Input.IsLeftMousePressed())
		CheckForButton(g_cursorPos.x, g_cursorPos.y);
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

	// Go through all the buttons and search for a button that surrounds the x and y position
	for(int i = 0; i < (int)m_vButtons.size(); i++)
	{
		// Get the position and width of the current button
		POINT position = m_vButtons[i].GetPosition();
		int width = m_vButtons[i].GetWidth();

		// Check if the x and y position is between a text button by using the width
		if((y >= position.y) && (y <= position.y+kFontHeight) && (x >= position.x && x < position.x + width))
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
// We added a few lines of code in ShowLoadMenu() and ShowSaveMenu()
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
