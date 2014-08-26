//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii Map Editor (Part 2)						 //
//																		 //
//		$Description:	A simple ASCII text based map editor			 //
//																		 //
//***********************************************************************//

#include "main.h"			// Include our main header file
#include "tile.h"			// Include our tile header file which holds the classes and defines


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this tutorial version we add 4 new functions:
// 
// InsertTile(), DeleteTile(), DrawTileInfo() and SetTileInfo().
// 
// These allow us to insert and delete tiles from the map, as well
// as display the tile info and read in the tile data text files.
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CTile class.  Initializes all data
/////
///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile::CTile():m_type(0)
{
	memset(&m_charTile, 0, sizeof(COORD));
}


///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CItem class.  Initializes all data
/////
///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CItem::CItem():m_lifeInc(0),m_strengthInc(0),m_protectionInc(0)
{
	memset(m_szItem, 0, MAX_NAME_LEN);
}


///////////////////////////////// CMONSTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CMonster class.  Initializes all data
/////
///////////////////////////////// CMONSTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMonster::CMonster():m_life(0),m_strength(0),m_speed(0)
{
	memset(m_szMonster, 0, MAX_NAME_LEN);
}


///////////////////////////////// CNPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CNpc class.  Initializes all data
/////
///////////////////////////////// CNPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CNpc::CNpc():m_life(0),m_strength(0),m_speed(0)
{
	memset(m_szName, 0, MAX_NAME_LEN);
	memset(m_szMessage, 0, MAX_MESSAGE_LEN);
}


///////////////////////////////// CEXIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CExit class.  Initializes all data
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
/////	This is the constructor for the CMap class.  Initializes all data
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
	// Here we move the bits to the left by 12 bits and back 12 to reset the background color
	image1.Attributes = image1.Attributes << 12;
	image1.Attributes = image1.Attributes >> 12;

	// Here we reset the foreground color by moving the bits to the right 4 and back to the left 4
	image2.Attributes = image2.Attributes >> 4;
	image2.Attributes = image2.Attributes << 4;

	// Now the magic of binary OR's.  We or the 2 color values together to get a mix
	// of the desired new background in image2, and the desired old foreground color in image1.
	image1.Attributes = image1.Attributes | image2.Attributes;
	
	// Let's now return the new image with it's cool new background!  (Isn't math fun! :)
	return image1;
}


///////////////////////////// ADD NEW FOREGROUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This does the binary math to extract and apply the foreground color of a tile
/////
///////////////////////////// ADD NEW FOREGROUND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CHAR_INFO CMap::AddNewForeground(CHAR_INFO image1, CHAR_INFO image2)
{
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////// DRAW TILE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a prompt that displays the current tile's name
/////
///////////////////////////// DRAW TILE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DrawTileInfo()
{
	// In this function we check which type of tile the cursor has, then we display
	// the name of that tile (if applicable) to the screen so we know which tile we have.

	// Below, we first check if there is a valid tile attached to the cursor.  We then
	// check if the tile type is a normal tile or an exit tile.  If these are true, stop.
	// We only want to display tile names if they are a monster, npc or item.
	if(g_pCursorTile == NULL) return;
	if(g_Map.GetCurrentType() == TILE_TYPE || g_pCurrentMap->GetCurrentType() == EXIT_TYPE)
		return;
	
	// Create a variable to hold the name of the tile
	char szName[255] = {0};

	if(g_Map.GetCurrentType() == ITEM_TYPE)				// If the tile is an item
	{
		// Cast the cursor tile as an item, then extract it's name
		CItem *pItem = (CItem*)g_pCursorTile;
		strcpy(szName, pItem->GetItemName());
	}
	else if(g_Map.GetCurrentType() == MONSTER_TYPE)		// If the tile is a monster
	{
		// Cast the cursor tile as a monster, then extract it's name
		CMonster *pMonster = (CMonster*)g_pCursorTile;
		strcpy(szName, pMonster->GetName());
	}
	else if(g_Map.GetCurrentType() == NPC_TYPE)			// If the tile is a npc
	{
		// Cast the cursor tile as a npc, then extract it's name
		CNpc *pNpc = (CNpc*)g_pCursorTile;
		strcpy(szName, pNpc->GetName());
	}

	// Next, we just grab an output handle, then set the cursor position to display
	// the tile information.  We can just use a simple cout to display the information
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// Display our prompt message
	cout << "(Space Bar Cycles) Current tile: " << szName;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
													// Get an OUTPUT handle to our screen
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// Now that we are inserting tiles into the map, we need to draw them into the map
	// when the screen redraws.  This is primarily because we have a cursor that moves over
	// the characters and we need to redraw what's behind it when it moves.  it also allows us
	// to be the most current of what's on our map.  All we do is go through all of our lists
	// and then insert each tile into our map.  Finally, we draw the whole screen with the buffer.

	// Go through all of the exits and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vExits.size(); i++)
	{
		// Grab the position of the exit and insert it into the map's screen buffer
		COORD index = m_vExits[i].GetIndex();
		int slot = index.X + index.Y * MAP_WIDTH;
		m_screenBuffer[slot] = m_vExits[i].GetChar();
	}

	// Go through all of the items and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		// Grab the position of the item and insert it into the map's screen buffer
		COORD index = m_vItems[i].GetIndex();
		int slot = index.X + index.Y * MAP_WIDTH;
		m_screenBuffer[slot] = AddNewBackground(m_vItems[i].GetChar(), m_screenBuffer[slot]);
	}

	// Go through all of the npcs and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		// Grab the position of the npc and insert it into the map's screen buffer
		COORD index = m_vNpcs[i].GetIndex();
		int slot = index.X + index.Y * MAP_WIDTH;
		m_screenBuffer[slot] = AddNewBackground(m_vNpcs[i].GetChar(), m_screenBuffer[slot]);
	}

	// Go through all of the monsters and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vMonsters.size(); i++)
	{
		// Grab the position of the monster and insert it into the map's screen buffer
		COORD index = m_vMonsters[i].GetIndex();
		int slot = index.X + index.Y * MAP_WIDTH;
		m_screenBuffer[slot] = AddNewBackground(m_vMonsters[i].GetChar(), m_screenBuffer[slot]);
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Here we actually draw all the map and editor tiles to our screen
	WriteConsoleOutput(hOutput, m_screenBuffer, bufferSize, zeroZero, &drawRect);

	// If we have a tile attached to our cursor, let's draw it!
	if(g_pCursorTile != NULL)
	{
		// Get the current tile attached to our cursor
		CHAR_INFO cursorTile = g_pCursorTile->GetChar();
		
		// Add the tile character to our string (We need a string for WriteConsoleOutputCharacter()
		sprintf(szCursor, "%c", cursorTile.Char);

		// Finally, we draw our cursor's tile character, and then draw it's color
		WriteConsoleOutputCharacter(hOutput, szCursor, 1, g_cursorPos, &dwResult);
		WriteConsoleOutputAttribute(hOutput, &cursorTile.Attributes, 1, g_cursorPos, &dwResult);
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	DrawTileInfo();			// Draw our prompt to display the tiles info
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// SET TILE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the info files associated with items, monsters and npcs.
/////
///////////////////////////////// SET TILE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::SetTileInfo(int type, char *szFile)
{
	// Create a variable to store the tile names
	char szName[MAX_PATH] = {0};	

	// This function goes through a text file stored in our projects root directory
	// and reads in all the data associated with each separate tile type.  We have
	// text files for monsters, items and npcs.  Each line stores info for one tile
	// tile.  We can then cycle through these tiles and place them on the map.
	// We only load file at a time, so this function is called 3 times for each file.

	// Open our info file in "read" mode.
	FILE *fp = fopen(szFile, "r");

	// Make sure we found the file, if not report the error
	if(!fp)
	{
		// Display an error message if we couldn't find the file
		sprintf(szFile, "Unable to find tile info in %s", szFile);
		MessageBox(NULL, szFile, "Error", MB_OK);
		return;
	}

	// Keep going through the file until we reach the end of it
	while(!feof(fp))
	{
		if(type == ITEM_TYPE)					   // If the tile type is an item
		{
			CItem tempItem;						   // Create a temp item tile
			int life = 0, str = 0, protection = 0; // Create variables for read data and item's image
			CHAR_INFO image = {'I', FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};

			// To read in the data from the file we use fscanf().  It works just like the
			// C function, printf(), where you can print things in a specified format, but
			// this reads in data in a specified format.  Since we know the format of our
			// tiles in each respective info file, we set that up here with fscanf() so that
			// we get all the data in the correct variable.  This function goes through and
			// reads the name, skips the '>' character, skips the "lifeInc:" part, then reads
			// in the life increase for the particular item.  Then skips over "strengthInc:"
			// and reads in the strength increase associated with the item, and so on.  You
			// have to make sure that in order to do this, you must spell everything correctly,
			// and have spaces between each data setup in the text file, otherwise it will fail.
			// The spacing isn't so important as to how many spaces, since fscanf() searches
			// between spaces until it finds the next characters, but just be exact as possible :)

			// Read in the current line for this item in the info file
			fscanf(fp, "%s > lifeInc: %d strengthInc: %d protectionInc: %d\n", szName, &life, &str, &protection);
			
			// Set the info read in for this tile
			tempItem.SetChar(image);
			tempItem.SetLifeInc(life);
			tempItem.SetStrengthInc(str);
			tempItem.SetProtectionInc(protection);
			tempItem.SetName(szName);

			// Set this tile type as an item, then add it to our global items list to choose from
			tempItem.SetType(ITEM_TYPE);
			g_vItems.push_back(tempItem);
		}
		else if(type == MONSTER_TYPE)			// If the tile type is an monster
		{
			CMonster tempMonster;				// Create a temp monster tile
			int life = 0, str = 0, speed = 0;	// Create variables for data and the monster's color
			CHAR_INFO image = {0, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};

			// Read in the current line for this monster in the info file
			fscanf(fp, "%s > image: %c life: %d strength: %d speed: %d\n", 
														  szName, &image.Char, &life, &str, &speed);

			// Set the info read in for this tile
			tempMonster.SetChar(image);
			tempMonster.SetLife(life);
			tempMonster.SetStrength(str);
			tempMonster.SetSpeed(speed);
			tempMonster.SetName(szName);

			// Set the tile type as a monster, then add it to our global monster list to choose from
			tempMonster.SetType(MONSTER_TYPE);
			g_vMonsters.push_back(tempMonster);
		}
		else if(type == NPC_TYPE)					// If the tile type is an npc
		{
			CNpc tempNpc;							// Create a temp npc tile
			char szMessage[MAX_MESSAGE_LEN] = {0};	// This will store the npc's message
			int life = 0, str = 0, speed = 0;		// Create variables for data and the npc's image
			CHAR_INFO image = {1, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE};

			// Read in the current line for this npc in the info file
			fscanf(fp, "%s > life: %d strength: %d speed: %d Message: ", szName, &life, &str, &speed);

			// Now that we read in the initial data for this NPC, we want to then grab it's
			// message that it says in the game, but we can't just read it in as a string like
			// the name, we need to grab a whole sentence.  We use fgets() to read a whole sentence.
			// Get a whole line of text for the npc's message
			fgets(szMessage, MAX_MESSAGE_LEN, fp);

			// Get rid of a weird character read in from fgets()
			if( strlen(szMessage) > 1 )
				szMessage[strlen(szMessage) - 1] = '\0';

			// Set the info read in for this tile then quit the function
			tempNpc.SetChar(image);
			tempNpc.SetLife(life);
			tempNpc.SetStrength(str);
			tempNpc.SetSpeed(speed);
			tempNpc.SetName(szName);
			tempNpc.SetMessage(szMessage);

			// Set this tile type as a npc, then add it to our global npc list to choose from
			tempNpc.SetType(NPC_TYPE);
			g_vNpcs.push_back(tempNpc);
		}
	}

	// Close the file, a success!
	fclose(fp);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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

	// We actually change our window size from the normal size of (80, 15} to {80, 50}.
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

	// Now we want to fill in the map with a default grass tile to start the editor with
	CTile defaultTile;
	CHAR_INFO defaultImage = {'#', FOREGROUND_GREEN};

	// Set the default tile to a green grass image
	defaultTile.SetChar(defaultImage);

	// Next, we want to set up the map with all of the default grass images, so we go through
	// and fill in the map with these images, but we stop before we get to the editor's place.
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

				// Not we do some binary math to get the background color
				tileColor = (WORD)(i-255) << 4;
			}			
		}
		
		// Set the tile character and color to the current index in our screen buffer (editor section)
		m_screenBuffer[index].Char.AsciiChar = tile;
		m_screenBuffer[index].Attributes = tileColor;

		// Create a temp tile to be added to our tile list for our editor tiles
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

	// Add this cursor tile slot to our global tile list and set the cursor tile pointer
	g_vTiles.push_back(cursorTile);
	g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This is where we load in all of the text info files for items, monsters and npcs
	SetTileInfo(ITEM_TYPE, ITEM_INFO_FILE);
	SetTileInfo(MONSTER_TYPE, MONSTER_INFO_FILE);
	SetTileInfo(NPC_TYPE, NPC_INFO_FILE);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// INSERT TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function inserts a tile from the cursor and saves it on the map
/////
///////////////////////////////// INSERT TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::InsertTile(CTile *pTile, int x, int y)
{
	char type = 'e';						// Set a type for exit since exit's don't have tiles

	// This is where we take a tile (pTile) and a position and add it to our map.
	// Depending on the type of the tile, we add it to our map's list of tiles.
	// You will notice that we use .back() to access the very LAST index in our lists.
	// This could also be changed to an index into the list with [(list).size() - 1].

	// If the current tile is not for exits, then set the type accordingly
	if(g_pCurrentMap->GetCurrentType() != EXIT_TYPE)
		type = pTile->GetType();

	if(type == ITEM_TYPE)						// If we are inserting an item
	{
		// Cast the current tile to an item, add it to the list, then set it's index
		m_vItems.push_back(*(CItem*)pTile);
		m_vItems.back().SetIndex(x, y);
	}
	else if(type == MONSTER_TYPE)				// If we are inserting an monster
	{
		// Cast the current tile to an monster, add it to the list, then set it's index
		m_vMonsters.push_back(*(CMonster*)pTile);
		m_vMonsters.back().SetIndex(x, y);
	}
	else if(type == NPC_TYPE)					// If we are inserting an npc
	{
		// Cast the current tile to an npc, add it to the list, then set it's index
		m_vNpcs.push_back(*(CNpc*)pTile);
		m_vNpcs.back().SetIndex(x, y);
	}
	else if(type == EXIT_TYPE)				// If we are inserting an exit
	{
		CHAR_INFO image = {'E', FOREGROUND_RED};
		CExit exit;							// Create a temporary exit

		// Since we use the g_vTiles list to store the exit tile on the cursor,
		// we need to create an exit tile and assign it an image.

		exit.SetChar(image);				// Set the exit's image with an 'E' and red color
		m_vExits.push_back(exit);			// Add it to our list of exits
		m_vExits.back().SetIndex(x, y);		// Set it's index and then store the map name	
		m_vExits.back().SetCurrentMap(g_pCurrentMap->GetMapName());
	}
	else									// Otherwise it must be a normal map tile
	{
		// Now if the tile is just a normal tile, then we don't need to add a 
		// new tile, since we already do that in SetDefault() when we first 
		// start the application.  In this case, we just find the tile that 
		// has the same position on the map, then just replace it's image with 
		// the new image.  There is no addition to the list, just substitution.  

		// Go through all of the tiles and check for the desired index and change the data.
		for(int i = 0; i < (int)m_vTiles.size(); i++)
		{
			// Get the current exit of the tile we are checking
			COORD index = m_vTiles[i].GetIndex();

			// If this tile is under our cursor, we can then replace it's data
			if(index.X == x && index.Y == y)
			{
				// Set the data of the tile on the map with the new inserted tile data
				m_vTiles[i] = *pTile;
				m_vTiles[i].SetIndex(index.X, index.Y);
				break;
			}
		}
	}
}


///////////////////////////////// DELETE TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function deletes a tile from the map depending on the current type
/////
///////////////////////////////// DELETE TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DeleteTile(int mapX, int mapY)
{
	// In this function we take a position of a tile and then go through the
	// currently selected tile list and see if there is a tile with that same
	// position.  If there is, we delete it.  This check is first started
	// from the back of the list, so that it deletes the last tile placed
	// on the map.  Since you can have many tiles in the same position, you
	// can delete as many as you put down on that spot, you just have to
	// keep right clicking the mouse to do so.

	if(m_currentTypeFlag == ITEM_TYPE)				// If the current tile type is an item
	{
		// Go through all of the items on our map from the back to the front of the list
		for(int i = (int)m_vItems.size() - 1; i >= 0; i--)
		{
			// Check if the current item is the same position as our cursor
			if(m_vItems[i].GetIndex().X == mapX && m_vItems[i].GetIndex().Y == mapY)
			{
				// Erase the item we are checking using the STL vector list erase() function.
				// By adding i to .begin(), we get the current index that we want to erase.
				m_vItems.erase(m_vItems.begin() + i);
				break;
			}
		}
	}
	else if(m_currentTypeFlag == MONSTER_TYPE)		// If the current tile type is an monster
	{
		// Go through all of the monsters on our map from the back of the list to the front
		for(int i = (int)m_vMonsters.size() - 1; i >= 0; i--)
		{
			// Check if the current monster is the same position as our cursor
			if(m_vMonsters[i].GetIndex().X == mapX && m_vMonsters[i].GetIndex().Y == mapY)
			{
				// Erase the current monster
				m_vMonsters.erase(m_vMonsters.begin() + i);
				break;
			}
		}
	}
	else if(m_currentTypeFlag == NPC_TYPE)			// If the current tile type is an npc
	{
		// Go through all of the npcs on our map from the back of the list to the front
		for(int i = (int)m_vNpcs.size() - 1; i >= 0; i--)
		{
			// Check if the current npc is the same position as our cursor
			if(m_vNpcs[i].GetIndex().X == mapX && m_vNpcs[i].GetIndex().Y == mapY)
			{
				// Erase this npc
				m_vNpcs.erase(m_vNpcs.begin() + i);
				break;
			}
		}
	}
	else if(m_currentTypeFlag == EXIT_TYPE)			// If the current tile type is an exit
	{
		// Go through all of the exits on our map from the back of the list to the front
		for(int i = (int)m_vExits.size() - 1; i >= 0; i--)
		{
			// Check if the current exit is the same position as our cursor
			if(m_vExits[i].GetIndex().X == mapX && m_vExits[i].GetIndex().Y == mapY)
			{
				// Erase this exit
				m_vExits.erase(m_vExits.begin() + i);
				break;
			}
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added 4 new functions to this file.  These functions are:
// InsertTile(), DeleteTile(), DrawTileInfo() and SetTileInfo().
//
// The first 2 allow us to draw on the map and erase tiles from the map.  
// In the next tutorial we will add loading and saving functionality.
//
// DrawTileInfo() displays a prompt which tells us the tile's name.
//
// Finally, SetTileInfo() reads in all the text files which store the
// monster, item and npc data: monsters.txt, items.txt and npcs.txt.
//
// In the next tutorial we will add the functions to load and save maps.
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials       
