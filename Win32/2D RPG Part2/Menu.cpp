//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 2)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

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
	// This function allows us to create a button with some text, the width
	// of the button (the string length * kFontWidth usually), the position of the 
	// button, and item that is possibly associated with that button (like for shops
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
	if(m_menuImage)
	{
        DeleteObject(m_menuImage);
		m_menuImage = NULL;
	}

	if(m_fontImage)
	{
        DeleteObject(m_fontImage);
		m_fontImage = NULL;
	}
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
/////	This brings up a dialog box to choose a file to load
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
/////	This brings up a dialog box to input a file name for the save game
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
		// Next we get the x offset by mod'ing the index by the characters per line in the art.
		// We then multiply that by the width of the image's font.  To get the y offset we
		// divide instead of mod, then multiply by the font height.  If you are confused of 
		// when you should mod or divide, just know that you want the x offset to keep coming
		// back to 0 for every line, where the y value will never come back to 0, but increases.
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

	// Let's go over how this function works.  We break up the dialog box
	// art file into tiny 32x32 tiles so that we can make the box bigger or
	// smaller, depending on the side passed in.  The problem is, that since
	// we are going to draw the menus with an opacity, we don't want the tiles
	// to overlap when we make a box, or it will make a weird artifact.  The
	// tiles would overlap because what we do is draw the middle tiles that
	// grow, we draw them until they go past the width and height past in.
	// Then, we just plop the corner/end tiles overtop of the extruding middle
	// tiles so that we have an exact width and height.  We have to do this
	// because the tiles are 32x32.  So that they don't overlap, we subtract
	// the overlap size from the last middle tile drawn.  We draw the box in
	// 3 sections: The top section, the middle section, then the bottom section.

	///// *** TOP PORTION OF THE BOX *** /////

	// Using are new opaque drawing functions we draw the top left corner of the box
	g_Buffer.DisplayOpaqueBitmap(m_menuImage, x, y, kMenuOpacity, rTopLeft);

	// Now we want to grow the top part to fit the desired width passed in.  We keep
	// drawing a middle top tile until the go past the desired width.  Then we
	// subtract the overlap size so that it doesn't overlap with the top right tile.
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

	// Now that we are drawing the middle part of the box, we need 2 loops to draw
	// the middle tiles like normal, but also we need to have a loop on top of that
	// to draw middle sections until we reach the desired height.  We now need to
	// take care of the overlapping problem for the width and the height.
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
