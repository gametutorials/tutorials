//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii Map Editor (Part 1)						 //
//																		 //
//		$Description:	A simple ASCII text based map editor			 //
//																		 //
//***********************************************************************//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

#include "main.h"			// Include our main header file
#include "tile.h"			// Include our tile header file which holds the classes and defines


///////////////////////////////////////////////////////////////////////////////////
//
// This file includes all the class function definitions for our map and
// tile classes.  Check out Tile.h for the classes definitions of each of these.
// 
// In these functions we use some inheritance.  This helps us cut
// down on code so that we don't need to write functions for every type of tile.
// If you are unfamiliar with this, please refer to our tutorials in the C++ section.
// They are incredibly simple, yet very important and handy.
//
//


///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CTile class.  Initializes all data.
/////
///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile::CTile():m_type(0)
{
	memset(&m_charTile, 0, sizeof(COORD));
}


///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CItem class.  Initializes all data.
/////
///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CItem::CItem():m_lifeInc(0),m_strengthInc(0),m_protectionInc(0)
{
	memset(m_szItem, 0, MAX_NAME_LEN);
}


///////////////////////////////// CMONSTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CMonster class.  Initializes all data.
/////
///////////////////////////////// CMONSTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMonster::CMonster():m_life(0),m_strength(0),m_speed(0)
{
	memset(m_szMonster, 0, MAX_NAME_LEN);
}


///////////////////////////////// CNPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CNpc class.  Initializes all data.
/////
///////////////////////////////// CNPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CNpc::CNpc():m_life(0),m_strength(0),m_speed(0)
{
	memset(m_szName, 0, MAX_NAME_LEN);
	memset(m_szMessage, 0, MAX_MESSAGE_LEN);
}


///////////////////////////////// CEXIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CExit class.  Initializes all data.
/////
///////////////////////////////// CEXIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CExit::CExit()
{
	memset(m_szCurrentMap, 0, MAX_PATH);
	memset(m_szNewMap, 0, MAX_PATH);
	m_newPosition.X = m_newPosition.Y = 0;
}


///////////////////////////////// CMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CMap class.  Initializes all data.
/////
///////////////////////////////// CMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMap::CMap():m_currentTypeFlag(TILE_TYPE)
{
	memset(m_szMapName, 0, MAX_PATH);
}


///////////////////////////// ADD NEW BACKGROUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This does the binary math to extract and apply the background color of a tile
/////
///////////////////////////// ADD NEW BACKGROUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CHAR_INFO CMap::AddNewBackground(CHAR_INFO image1, CHAR_INFO image2)
{
	// Okay, here is some trick that allows us to take the background
	// color of image2 and apply it to image1.  Since the foreground and
	// background color are stored in 1 number, we need to do some binary 
	// math to extract out just the background color.  Let's look at the
	// defines for the background and foreground colors so that you can
	// understand why we do what we do.  These are stored in WinCon.h in
	// your compiler's include\ folder (In this case, Visual Studio .Net):
	//
	// #define FOREGROUND_BLUE      0x0001 // text color contains blue.
	// #define FOREGROUND_GREEN     0x0002 // text color contains green.
	// #define FOREGROUND_RED       0x0004 // text color contains red.
	// #define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
	// #define BACKGROUND_BLUE      0x0010 // background color contains blue.
	// #define BACKGROUND_GREEN     0x0020 // background color contains green.
	// #define BACKGROUND_RED       0x0040 // background color contains red.
	// #define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
	//
	// Notice that these are defined in hexadecimal, and the foreground are the 
	// last slot, whereas the background is the second to last slot in hex.  This
	// way we are able to distinguish the place where the different colors are stored.
	// If you have a scientific calculator (you can use the windows calculator), input
	// 0x0008 (or just 0x8, which is the same thing in hex) and then convert that to
	// decimal.  You will get 8.  Now, convert that to binary and you will see 1000.
	// This isn't one thousand, but the binary number for 8.  That means that the
	// first 4 slots (or bits) in binary store the foreground values.  Now, take
	// the highest one for the background, which is 0x0080.  That's 128 in decimal,
	// and 10000000, which is 8 bits.  If all the bits are turned on, it's 255.  That
	// is the max size for a "char" variable.  Well, we are using a WORD, which is 16
	// bits of data.  So, we have established that the first four bits (counting from
	// right to left as you should in binary) are for the foreground, and the next 4
	// are for the background.  So, let's get to the math for extracting the background.
	//
	// We can use the binary operator << or >> which moves the bits of a number to the
	// left (<<) or right (>>) by a given amount.  If 8 in binary is 1000, then 8 >> 4
	// would make it 0100 then 0010 then 0001 and finally, 0000.  That's right!  It just 
	// deleted the 1 bit that was originally there before.  That means we just destroyed
	// all the bits turned on (with 1), which changes the number.  Well, if we did
	// that to a color value, it would destroy the foreground color.  If we want to reset
	// the background color we can't just move 4 the other way, since we are working with
	// a 16 bit variable (WORD).  We actually need to move the bits 12 to the left.  This
	// will reset the bits for the background.  BUT WAIT!!  We need to move the bits back
	// to their original position, we can't just leave them there.  So, we move over 12 to
	// the left, and then move them back to the right 12.  This goes for the foreground, we
	// need to move the bits back to the left by 4.  
	//
	// So, now what??!?!?  Okay, now we can do a binary OR (|) between the 2 color values,
	// which will basically create a number that stores all the 1 values in binary between
	// the 2 other numbers (colors).  So, in effect, we want to reset image 1's background
	// color values since we are giving it new ones, then we want to reset the foreground
	// color values in the desired background color value so that we don't override the
	// foreground color of image1 (since we want those to stay).  Now, when we do a binary
	// OR between the 2, they will mix and create the new desired background and the old
	// desired foreground.  Hopefully that doesn't confuse you too much.  If you don't
	// understand how binary math works, you should really understand this if you want to
	// be a programmer.  You can check out our tutorials on binary math to brush up.
	
	// Here we move the bits to the left by 12 bits and back 12 to reset the background color
	image1.Attributes = image1.Attributes << 12;
	image1.Attributes = image1.Attributes >> 12;

	// Here we reset the foreground color by moving the bits to the right 4 and back to the left 4
	image2.Attributes = image2.Attributes >> 4;
	image2.Attributes = image2.Attributes << 4;

	// Now the magic of binary OR's.  We or the 2 color values together to get a mix
	// of the desired new background in image2, and the desired old foreground color in image1.
	image1.Attributes = image1.Attributes | image2.Attributes;
	
	// Let's now return the new image with its cool new background!  (Isn't math fun! :)
	return image1;
}


///////////////////////////// ADD NEW FOREGROUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This does the binary math to extract and apply the foreground color of a tile
/////
///////////////////////////// ADD NEW FOREGROUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CHAR_INFO CMap::AddNewForeground(CHAR_INFO image1, CHAR_INFO image2)
{
	// This function is almost exactly like the AddNewBackground() function, but
	// instead we switch the binary math to the different variable since we want
	// to add a new foreground to image1, not a new background.

	// Here we reset the foreground color by moving the bits to the right 4 and back to the left 4
	image1.Attributes = image1.Attributes >> 4;
	image1.Attributes = image1.Attributes << 4;

	// Here we move the bits to the left by 12 bits and back 12 to reset the background color
	image2.Attributes = image2.Attributes << 12;
	image2.Attributes = image2.Attributes >> 12;

	// With a binary OR operation we can mix the new foreground color with the old background color
	image1.Attributes = image1.Attributes | image2.Attributes;
	
	// Return our new image with it's mixed color
	return image1;
}


///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws all the placed tile types on the map
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Draw()
{
	SMALL_RECT drawRect = {0, 0, MAP_WIDTH - 1, MAP_HEIGHT - 1};	// This is the drawing rectangle
	COORD bufferSize	= {MAP_WIDTH , MAP_HEIGHT};	// This stores the size of our buffer
	COORD zeroZero		= {0, 0};					// This tells us the upper left corner to draw from
	DWORD dwResult		= 0;						// This stores the result of the draw operation
	char szCursor[2]	= "";						// This string stores the cursor tile character
													// Get an OUTPUT handle to our screen.
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);		

	// In this function we will handle all of our drawing to the screen.  In this version
	// we won't draw everything, but just the cursor's tile and of course the editor tiles
	// and the default map tiles.

	// Here we go through our normal map tile list and add them to our drawing screen buffer
	for(int i = 0; i < (int)m_vTiles.size(); i++)
	{
		// Here we get the CHAR_INFO of the current tile in our list
		m_screenBuffer[i] = m_vTiles[i].GetChar();
	}

	// Here we actually draw all the map and editor tiles to our screen
	WriteConsoleOutput(hOutput, m_screenBuffer, bufferSize, zeroZero, &drawRect);

	// If we have a tile attached to our cursor, let's draw it!
	if(g_pCursorTile != NULL)
	{
		// Get the current tile attached to our cursor
		CHAR_INFO cursorTile = g_pCursorTile->GetChar();
		
		// Add the tile character to our string (we need a string for WriteConsoleOutputCharacter())
		sprintf(szCursor, "%c", cursorTile.Char);

		// Finally, we draw our cursor's tile character, and then draw it's color
		WriteConsoleOutputCharacter(hOutput, szCursor, 1, g_cursorPos, &dwResult);
		WriteConsoleOutputAttribute(hOutput, &cursorTile.Attributes, 1, g_cursorPos, &dwResult);
	}
}


///////////////////////////////// SET DEFAULT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets a default map with just green grass
/////
///////////////////////////////// SET DEFAULT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::SetDefault()
{														// This is the desired size of our window
	SMALL_RECT newSize = {0, 0, MAP_WIDTH - 1, MAP_HEIGHT - 1};
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);	// Get a OUTPUT handle to our screen.
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);		// Get a INPUT handle to our screen.

	// This function is called in the beginning of the program just once.  It basically
	// initializes and sets up all of the map information.

	// We actually change our window size from the normal size of (80, 15} to {80, 50}.
	// This function below does that.  We needed more room for our map editor.  The TRUE
	// tells us that we want to use the absolute size of the window (counting from 0, 0}.
	SetConsoleWindowInfo(hOutput, TRUE, &newSize);

	// Next we want to be able to have mouse input and other keyboard input
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	// Make sure we clear and release all the data with our global lists
	m_vTiles.clear();
	m_vItems.clear();
	m_vMonsters.clear();
	m_vNpcs.clear();
	m_vExits.clear();

	// Reset the current map's name
	strcpy(m_szMapName, "");

	// Now we want to fill in the map with a default grass tile to start the editor with.
	CTile defaultTile;
	CHAR_INFO defaultImage = {'#', FOREGROUND_GREEN};

	// Set the default tile to a green grass image
	defaultTile.SetChar(defaultImage);

	// Next, we want to set up the map with all of the default grass images, so we go through
	// and fill in the map with these images, but we stop before we get to the editor's place.
	// That is why we minus the EDITOR_HEIGHT.
	for(int y = 0; y < MAP_HEIGHT - EDITOR_HEIGHT; y++)
	{
		// Fill in this row with our default grass image
		for(int x = 0; x < MAP_WIDTH; x++)
		{
			// Set the current index for the map and add it to the tile list
			defaultTile.SetIndex(x, y);
			m_vTiles.push_back(defaultTile);

			// Set the current index into our screen buffer to the default grass image
			m_screenBuffer[x + y * MAP_WIDTH] = defaultImage;
		}
	}

	// Now, we need to setup the editor tiles.  There is 255 ASCII characters, but on top
	// of that we have around 32 more characters for the foreground and background color tiles.

	// Go through all of our editor tiles and set them up
	for(int i = 0; i <= 286; i++)
	{
		// Here we create a default color of a black background and a white character
		WORD tileColor = {FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};
		// Grab the current index into our editor screen
		int index = i + (MAP_HEIGHT - EDITOR_HEIGHT + 1) * MAP_WIDTH;
		// Here we cast the current index i to a character for our editor window
		char tile = (char)i;

		// If the current index is over 255, that means we need to add color tiles
		if(i >= 255)
		{
			// If the current index is 270 or above, we create the foreground colors
			if(i <= 270)
			{
				// Use a default character for the foreground color tile
				tile = '@';

				// Next we just do some simple math to get each foreground color, since
				// all the foreground colors are stored from 0 to 15. (4 bits)
				tileColor = (WORD)(i-255);
			}
			else	// Background color tiles
			{
				// Set a default character for the background (space seems fitting) 
				tile = ' ';

				// Not we do some binary math to get the background color.  We do the
				// same thing we did for the foreground, but since the background color
				// values are stored 4 bits over from the foreground, let's move them up 4 bits.
				tileColor = (WORD)(i-255) << 4;
			}			
		}
		
		// Set the tile character and color to the current index in our screen buffer (editor section)
		m_screenBuffer[index].Char.AsciiChar = tile;
		m_screenBuffer[index].Attributes = tileColor;

		// Create a temp tile to be added to our tile list for our editor tiles.
		// These are drawn every frame with the map since we sometimes have a cursor
		// over top of them, we need to have tiles for these so that the screen will be updated
		// when the cursor goes over any of these characters.
		CTile tempTile;

		// Setup the tile with the image, index and type
		tempTile.SetChar(m_screenBuffer[index]);
		tempTile.SetIndex(index, 0);
		tempTile.SetType(TILE_TYPE);
		
		// Add the current editor tile to our list of global tiles
		g_vTiles.push_back(tempTile);
	}

	// Now we want to create a temp tile for our cursor tile (if we selected a tile)
	CTile cursorTile;
	
	// Just get a copy of the first index tile (the smiley face)
	cursorTile.SetChar(g_vTiles[1].GetChar());
	cursorTile.SetType(g_vTiles[1].GetType());

	// Add this cursor tile slot to our global tile list and set the 
	// cursor tile pointer accordingly.  The last tile in our list will
	// always be just designated for our cursor tile.
	g_vTiles.push_back(cursorTile);
	g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we set up the basics for our map editor.  We defined 
// most of our class information here.  This file holds most
// of the information that deals with our map and tile classes.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials       
