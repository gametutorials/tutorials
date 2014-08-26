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

#include "main.h"			// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// We added a few new functions down near the bottom that handle our dialog and movement
// with non-player characters (npcs).  We use a simple way to display dialog to the
// screen in a typewriter effect so it simulates talking more than just all the text
// at once.  We also do some simple random movement for our npcs on the map.
// 
// Go check out the code near the bottom of this file.
// 
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CTile class.  Initializes all data
/////
///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile::CTile():m_type(0),m_actionKey(0),m_bSolid(false)
{
	memset(&m_charTile, 0, sizeof(COORD));
}


///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CItem class.  Initializes all data
/////
///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CItem::CItem():m_lifeInc(0),m_strengthInc(0),m_protectionInc(0),m_itemType(0)
{
	memset(m_szItem, 0, kMaxNameLen);
}


///////////////////////////////// CMONSTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CMonster class.  Initializes all data
/////
///////////////////////////////// CMONSTER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMonster::CMonster():m_life(0),m_strength(0),m_speed(0)
{
	memset(m_szMonster, 0, kMaxNameLen);
}


///////////////////////////////// CNPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CNpc class.  Initializes all data
/////
///////////////////////////////// CNPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CNpc::CNpc():m_life(0),m_strength(0),m_speed(0)
{
	memset(m_szName, 0, kMaxNameLen);
	memset(m_szMessage, 0, kMaxMessageLen);
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

CMap::CMap()
{
	memset(m_szMapName, 0, MAX_PATH);
}


///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads a map from a .map file
/////
///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Load(const char *szFile)
{
	// Create a variable to store the full path of the maps
	char szMapFile[MAX_PATH] = {0};

	// Below we add the "Maps\" part to our map name so it will load correctly.
	strcpy(szMapFile, kMapsDir);
	strcat(szMapFile, szFile);

	// Open our map that we want to load in "read" and "binary" mode
	FILE *fp = fopen(szMapFile, "rb");

	// If we couldn’t load the file report an error message
	if(!fp)
	{
		MessageBox(NULL, "Can't Find File", "Error", MB_OK);
		return;
	}

	// Reset all of our map list data before we load more in
	m_vTiles.clear();
	m_vItems.clear();
	m_vMonsters.clear();
	m_vNpcs.clear();
	m_vExits.clear();

	// Here we initialize all of our variables for the header.
	int tileSize = 0, itemSize = 0, monsterSize = 0, npcSize = 0, exitSize = 0;

	// Read in the sizes of each of the lists (the tile count of each list)
	fread(&tileSize, sizeof(int), 1, fp);
	fread(&itemSize, sizeof(int), 1, fp);
	fread(&monsterSize, sizeof(int), 1, fp);
	fread(&npcSize, sizeof(int), 1, fp);
	fread(&exitSize, sizeof(int), 1, fp);

	// Depending on the list size, reallocate the memory for our map lists
	m_vTiles.resize(tileSize);
	m_vItems.resize(itemSize);
	m_vMonsters.resize(monsterSize);
	m_vNpcs.resize(npcSize);
	m_vExits.resize(exitSize);

	// Go through all the lists in our map and load their tile information
	for(int i = 0; i < tileSize; i++)
		fread(&m_vTiles[i], sizeof(CTile), 1, fp);
	for(int i = 0; i < itemSize; i++)
		fread(&m_vItems[i], sizeof(CItem), 1, fp);
	for(int i = 0; i < monsterSize; i++)
		fread(&m_vMonsters[i], sizeof(CMonster), 1, fp);
	for(int i = 0; i < npcSize; i++)
		fread(&m_vNpcs[i], sizeof(CNpc), 1, fp);
	for(int i = 0; i < exitSize; i++)
		fread(&m_vExits[i], sizeof(CExit), 1, fp);
	
	// Close our file that we opened, a success!
	fclose(fp);

	// Set the name of this map, NOT using the full path (szMapName - Full Path, szFile - Name)
	strcpy(m_szMapName, szFile);
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


///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws all the placed tile types on the map
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Draw()
{
	// If we don't need to update the screen, we just quit the function
	if(!m_bDrawScreen) return;

	// If we drawing, let's reset the flag so we don't draw again, unless needed
	m_bDrawScreen = false;

	SMALL_RECT drawRect = {0, 0, kMapWidth - 1, kMapHeight - 1}; // This is the drawing rectangle
	COORD bufferSize	= {kMapWidth , kMapHeight};	// This stores the size of our buffer
	COORD zeroZero		= {0, 0};					// This tells us the upper left corner to draw from
	DWORD dwResult		= 0;						// This stores the result of the draw operation
	char szCursor[2]	= "";						// This string stores the cursor tile character
													// Get an OUTPUT handle to our screen.
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);		

	// Here we go through our normal map tile list and add them to our drawing screen buffer
	for(int i = 0; i < (int)m_vTiles.size(); i++)
	{
		// Here we get the CHAR_INFO of the current tile in our list
		m_screenBuffer[i] = m_vTiles[i].GetChar();
	}

	// Go through all of the items and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		// Grab the position of the item and insert it into the map's screen buffer
		COORD index = m_vItems[i].GetIndex();
		int slot = index.X + index.Y * kMapWidth; // Change the background color to the map's
		m_screenBuffer[slot] = AddNewBackground(m_vItems[i].GetChar(), m_screenBuffer[slot]);
	}

	// Go through all of the npcs and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		// Grab the position of the npc and insert it into the map's screen buffer
		COORD index = m_vNpcs[i].GetIndex();
		int slot = index.X + index.Y * kMapWidth; // Change the background color to the map's
		m_screenBuffer[slot] = AddNewBackground(m_vNpcs[i].GetChar(), m_screenBuffer[slot]);
	}

	// Go through all of the monsters and draw their character/attribute to our map
	for(int i = 0; i < (int)m_vMonsters.size(); i++)
	{
		// Grab the position of the monster and insert it into the map's screen buffer
		COORD index = m_vMonsters[i].GetIndex();
		int slot = index.X + index.Y * kMapWidth; // Change the background color to the map's
		m_screenBuffer[slot] = AddNewBackground(m_vMonsters[i].GetChar(), m_screenBuffer[slot]);
	}

	// Here we actually draw all the map and editor tiles to our screen
	WriteConsoleOutput(hOutput, m_screenBuffer, bufferSize, zeroZero, &drawRect);

	// After the map is drawn, let's draw the player over top of the map
	g_Player.Draw();
}


///////////////////////////////// SET DEFAULT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the map and console settings to our desired defaults
/////
///////////////////////////////// SET DEFAULT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::SetDefault()
{														// This is the desired size of our window
	SMALL_RECT newSize = {0, 0, kMapWidth - 1, kMapHeight - 1};
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);	// Get a OUTPUT handle to our screen.
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);		// Get a INPUT handle to our screen.

	// We actually change our window size from the normal size of (80, 15} to {80, 45}.
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
}


///////////////////////////////// INSERT TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function inserts a tile and saves it on the map
/////
///////////////////////////////// INSERT TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::InsertTile(CTile *pTile, int type, int x, int y)
{
	if(type == kItemType)						// If we are inserting an item
	{
		// Cast the current tile to an item, add it to the list, then set it's index
		m_vItems.push_back(*(CItem*)pTile);
		m_vItems.back().SetIndex(x, y);
	}
	else if(type == kMonsterType)				// If we are inserting an monster
	{
		// Cast the current tile to an monster, add it to the list, then set it's index
		m_vMonsters.push_back(*(CMonster*)pTile);
		m_vMonsters.back().SetIndex(x, y);
	}
	else if(type == kNpcType)					// If we are inserting an npc
	{
		// Cast the current tile to an npc, add it to the list, then set it's index
		m_vNpcs.push_back(*(CNpc*)pTile);
		m_vNpcs.back().SetIndex(x, y);
	}
	else									// Otherwise it must be a normal map tile
	{
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
/////	This function deletes a tile from the map depending on the type
/////
///////////////////////////////// DELETE TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DeleteTile(int type, int mapX, int mapY)
{
	if(type == kItemType)				// If the current tile type is an item
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
	else if(type == kMonsterType)		// If the current tile type is an monster
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
	else if(type == kNpcType)			// If the current tile type is an npc
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
	else if(type == kExitType)			// If the current tile type is an exit
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


///////////////////////////////// IS A COLLISION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tests if the position passed in is over a solid tile
/////
///////////////////////////////// IS A COLLISION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CMap::IsACollision(int x, int y)
{
	// This simply goes through all of the tiles and finds the tile with
	// the same position that was passed in, returning if that tile is solid or not.
	for(int i = 0; i < (int)m_vTiles.size(); i++)
	{
		// Check if this tile is the same position passed in
		if(x == m_vTiles[i].GetIndex().X && y == m_vTiles[i].GetIndex().Y)
		{
			// Return whether or not the tile is solid
			return m_vTiles[i].IsSolid();
		}
	}

	// Return a default false
	return false;
}


///////////////////////////////// CHECK FOR EXIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks if the position passed in is an exit spot, then changes maps
/////
///////////////////////////////// CHECK FOR EXIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::CheckForExit(int x, int y)
{
	// Let's go through all of the exits and check if there is an exit to use
	for(int i = 0; i < (int)m_vExits.size(); i++)
	{
		// Grab the current exit being checked
		CExit *pExit = &m_vExits[i];

		// If this exit is the same position as the new players position passed in...
		if(x == pExit->GetIndex().X && y == pExit->GetIndex().Y)
		{
			char szNewMap[255] = {0};

			// Draw the new map, and due a little pause with the windows Sleep() function.
			// This is so it shows that we actually walked on that spot before we change maps.
			SetDrawFlag(true);
			Draw();
			Sleep(100);

			// Copy the new map's name to load, then set our player's positions accordingly
			strcpy(szNewMap, pExit->GetNewMap());
			g_Player.SetPosition(pExit->GetNewPos());
			g_Player.SetLastPosition(pExit->GetNewPos());

			// Load the new map, then we want to draw the map so we set the draw flag to true.
			// This will get draw from the main game loop when Draw() is called.  Then break.
			Load(szNewMap);
			SetDrawFlag(true);
			break;
		}
	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// CHECK FOR NPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks if the position passed in is a npc, then displays dialog
/////
///////////////////////////////// CHECK FOR NPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::CheckForNpc(int x, int y)
{
	// Go through all of the npcs and see if we have a match for the position
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		CNpc *pNpc= &m_vNpcs[i];

		// Check if this npc has the same position as the position passed in
		if(x == pNpc->GetIndex().X && y == pNpc->GetIndex().Y)
		{
			// If we get here, then we know that the player ran into a npc.
			// This means that we want to display some dialog since the player
			// wants to talk to this person.  We first move the player backwards
			// to it's last position so we don't smother the npc with too much love,
			// then grab the npc's message and display to the screen in a dialog box.
			g_Player.MovePlayerBack();

			// Display the message of the npc and pause for a second.  We then
			// want to flush the input buffer just in case the player pressed
			// a bunch of keys during the pause and we want to give them a fresh
			// chance to advance the dialog when they are finished reading.
			// Notice that we add the npc's name to the string so we know who's talking.
			char szDialog[5000] = {0};
			sprintf(szDialog, "%s: %s", pNpc->GetName(), pNpc->GetMessage());
			DisplayDialog(szDialog);
			Sleep(1000);
			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
			getch();
			break;
		}
	}
}


///////////////////////////////// DISPLAY DIALOG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a string and displays it in a dialog box with some formating
/////
///////////////////////////////// DISPLAY DIALOG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DisplayDialog(char *szDialog)
{
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwResult = 0;

	// The dialog color will have a white foreground and a blue background.  These
	// will be intense whites and blues so it looks more brilliant with color.
	WORD dialogColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY |
					   BACKGROUND_BLUE | BACKGROUND_INTENSITY;
	
	// Okay, let's talk about how this function works.  We are taking in possibly a huge
	// message that could be like 5 lines.  We don't want to just display the dialog
	// going straight across the screen, since it would probably go off the screen.  We
	// want to make the text wrap around to the next line if it goes past our predetermined
	// max dialog line length.  We do some calculations on the message's length of characters
	// and then depending on that and other small calculations, we draw it to the screen
	// wrapping around inside the dialog box.  The box will grow to however big the message is.

	// First we get the message string length, then find out how many lines the dialog
	// will be.  We just divide the length of the message in characters by the max
	// character length we allow for the dialog box per line (kDlgLineLength).
	int dialogLength = (int)strlen(szDialog);
	int dialogLines = dialogLength / kDlgLineLength;

	// We will start the dialog text at the screens coordinates (13, 13).
	COORD dlgPosition = {13, 13};
		
	// We will draw the actual box around the dialog at (10,10).  The width
	// will be the size of the max char length for a line plus a padding of 6.
	// The height will be the max lines with a padding of 6.  This DrawBox()
	// function is created in our Menu.cpp.
	g_Menu.DrawBox(kDlgLineLength + 6, dialogLines + 6, 10, 10);

	// Now that we have a box drawn and the dialog calculations mostly done,
	// let's start putting the characters on the screen.  We will have a slow
	// typewriter effect which will have a delay between each character that 
	// is drawn.  We only draw 1 character at a time to do this.
	for(int i = 0; i < dialogLength; i++)
	{
		// Draw the current character to the screen at the current location.
		// The DrawString() function is created in our menu class.  It takes
		// a string, the amount of characters to draw in that string, and a position.
		// When we pass in the address of an index in szDialog, it ignores the previous
		// characters and starts with a string from index i to the end.
		g_Menu.DrawString(&szDialog[i], 1, dlgPosition.X, dlgPosition.Y);

		// Here we do some math to see if we have reached the end of the dialog box.
		// we just add 1 (so 0 % length doesn't get set) and mod that by the max 
		// character length per line.  When it reaches the same number as the length,
		// the mod (%) will return 0, since there is no remainder, and if we ! (not)
		// that, it turns the 0 to a 1.  Otherwise, !(any number but 0) will be 0.
		if(!((i + 1) % kDlgLineLength))
		{
			// Put the x position back to the beginning of the dialog box and increase Y by 1
			dlgPosition.X -= kDlgLineLength - 1;
			dlgPosition.Y++;
		}	
		else
			dlgPosition.X++;	// Otherwise we just increase x until it gets to the end

		// We want a typewriter effect so we just put a simple delay between each character
		Sleep(20);
	}
}


///////////////////////////////// MOVE NPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This moves all the npcs in a random direction
/////
///////////////////////////////// MOVE NPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::MoveNpcs()
{
	// This function moves the npcs around in random positions.  We just do
	// some simple method of having the npc move only when our random number
	// happens to be 2500 (some trivial number).  That way they sometimes move
	// and sometimes not.  You can make this more smooth with some AI of course.
	// There is another random number that determines which direction they will move.

	// Go through all of the npcs
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		// If the random number is 2500, then move the npc
		if((rand() % 50000000) == 2500)
		{
			COORD position = m_vNpcs[i].GetIndex();
			
			// To make them move even less, we give them a 4/10 chance again :)
			// With the result of this random number they move a random direction.
			switch(rand() % 10)
			{
				case 0:	position.X++;	break;
				case 1:	position.X--;	break;
				case 2:	position.Y++;	break;
				case 3:	position.Y--;	break;
			}

			// Before we assign the new position, we check to see if it's a collision
			if(!IsACollision(position.X, position.Y))
			{
				m_vNpcs[i].SetIndex(position.X, position.Y);
				SetDrawFlag(true);
			}
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we added a few functions to handle the dialog and
// the moving of the npcs.  CheckForNpc(), MoveNpcs() and DisplayDialog().
// 
// Of course, you will want to make the dialog and the movement of the
// npcs more perfect, but this should get you started on the basics
// of such things.  You would maybe want to make the npcs go in a
// certain direction for a while so it doesn't look so random sometimes.
// You also would give them a boundary to not go out of (like for farmers).
// The dialog could use a better wrapping system that doesn't wrap in the
// middle of a word, but checks above and wraps the whole word together.
// This could only take a few lines to do.  The boundary and less random
// movement wouldn't be too much more code either.  Go for it!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials       
