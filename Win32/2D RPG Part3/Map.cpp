//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 3)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "main.h"			// Include our main header file
#include "Map.h"			// Include our tile header file which holds the classes and defines


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// We added functionality for picking up items and displaying stats on the screen.
// 
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CTile class.  Initializes all data
/////
///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile::CTile():m_bmpTile(0),m_type(0),m_actionKey(0),m_bSolid(false)
{
	memset(m_szFile, 0, MAX_PATH);
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
	m_newPosition.x = m_newPosition.y = 0;
}


///////////////////////////////// CMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CMap class.  Initializes all data
/////
///////////////////////////////// CMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMap::CMap():m_bDrawScreen(false),m_bShowStats(true)
{
	memset(m_szMapName, 0, MAX_PATH);
}


///////////////////////////////// ~CMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the deconstructor for the CMap class
/////
///////////////////////////////// ~CMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CMap::~CMap()
{
	FreeResources();
}


///////////////////////////////// FREE RESOURCES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through all of our map bitmaps and makes sure they are freed
/////
///////////////////////////////// FREE RESOURCES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::FreeResources()
{
	// Go through all of our tiles and see if the memory for their image needs to be freed
	for(int i = 0; i < (int)m_vTiles.size(); i++)
	{
		if(m_vTiles[i].GetBitmap())
            DeleteObject(m_vTiles[i].GetBitmap());

		m_vTiles[i].SetBitmap(NULL);
	}

	// Go through all of our items and see if the memory for their image needs to be freed
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		if(m_vItems[i].GetBitmap())
            DeleteObject(m_vItems[i].GetBitmap());

		m_vItems[i].SetBitmap(NULL);
	}

	// Go through all of our monsters and see if the memory for their image needs to be freed
	for(int i = 0; i < (int)m_vMonsters.size(); i++)
	{
		if(m_vMonsters[i].GetBitmap())
            DeleteObject(m_vMonsters[i].GetBitmap());

		m_vMonsters[i].SetBitmap(NULL);
	}

	// Go through all of our npcs and see if the memory for their image needs to be freed
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		if(m_vNpcs[i].GetBitmap())
            DeleteObject(m_vNpcs[i].GetBitmap());

		m_vNpcs[i].SetBitmap(NULL);
	}
}


///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads a map from a file
/////
///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Load(const char *szFile)
{
	// Create a variable to store the full path of the maps
	char szMapFile[MAX_PATH] = {0};
	FILE *fp = NULL;

	// If we can't find the Maps\ directory in the file name, add it to the file name.
	if(!strstr(szFile, kMapsDir))
	{
		// Below we add the "Maps\" part to our map name so it will load correctly.
		strcpy(szMapFile, kMapsDir);
		strcat(szMapFile, szFile);
		
		// Open our map that we want to load in "read" and "binary" mode
		fp = fopen(szMapFile, "rb");
	}
	else
		fp = fopen(szFile, "rb");

	// If we couldn’t load the file report an error message
	if(!fp)
	{
		MessageBox(NULL, "Can't Find File", "Error", MB_OK);
		return;
	}

	// Free the current images stored
	FreeResources();

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
	
	// Load and assign bitmap handles for each of the tiles, except for exits
	for(int i = 0; i < tileSize; i++)
		m_vTiles[i].SetBitmap( g_Buffer.LoadABitmap(m_vTiles[i].GetFileName()) );
	for(int i = 0; i < itemSize; i++)
		m_vItems[i].SetBitmap( g_Buffer.LoadABitmap(m_vItems[i].GetFileName()) );
	for(int i = 0; i < monsterSize; i++)
		m_vMonsters[i].SetBitmap( g_Buffer.LoadABitmap(m_vMonsters[i].GetFileName()) );
	for(int i = 0; i < npcSize; i++)
		m_vNpcs[i].SetBitmap( g_Buffer.LoadABitmap(m_vNpcs[i].GetFileName()) );
	for(int i = 0; i < exitSize; i++)
		m_vExits[i].SetBitmap( 0 );

	// Close our file that we opened, a success!
	fclose(fp);

	// Set the name of this map, NOT using the path (szMapName - Path, szFile - Name)
	strcpy(m_szMapName, szFile);
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

	// Here we set the portion to draw for our transparent tiles
	RECT portion = {0, 0, kTileWidth, kTileHeight};
	int x = 0, y = 0;

	// Go through and draw all the normal map tiles
	for(int i = 0; i < (int)m_vTiles.size(); i++)
	{
		x = m_vTiles[i].GetIndex().x * kTileWidth;
		y = m_vTiles[i].GetIndex().y * kTileHeight;
		g_Buffer.DisplayBitmap(m_vTiles[i].GetBitmap(), x, y);
	}

	// Go through and draw all the items on the map
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		x = m_vItems[i].GetIndex().x * kTileWidth;
		y = m_vItems[i].GetIndex().y * kTileHeight;
		g_Buffer.DisplayTransparentBitmap(m_vItems[i].GetBitmap(), x, y, portion);
	}

	// Go through and draw all the monsters on the map
	for(int i = 0; i < (int)m_vMonsters.size(); i++)
	{
		x = m_vMonsters[i].GetIndex().x * kTileWidth;
		y = m_vMonsters[i].GetIndex().y * kTileHeight;
		g_Buffer.DisplayTransparentBitmap(m_vMonsters[i].GetBitmap(), x, y, portion);
	}

	// Go through and draw all the non player characters on the map
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		x = m_vNpcs[i].GetIndex().x * kTileWidth;
		y = m_vNpcs[i].GetIndex().y * kTileHeight;
		g_Buffer.DisplayTransparentBitmap(m_vNpcs[i].GetBitmap(), x, y, portion);
	}

	// After the map is drawn, let's draw the player over top of the map
	g_Player.Draw();


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This is where we will draw our stats screen, which is at the bottom of the screen.
	// Since we didn't always want to show that, we setup a boolean to check if the user
	// wants to see it or not (space bar toggles this).
	if(m_bShowStats)
	{
		char szStats[80] = {0};

		// Here we just fill in a string with the players information
		sprintf(szStats, "Hp: %d  Lvl: %d  Exp: %d  $: %d ", 
			g_Player.GetHealth(), g_Player.GetLevel(), g_Player.GetExperience(), g_Player.GetGold());

		// Here we draw a box surrounding the stats with a padding of the tile width of 2 for
		// height and width, which we then draw the string in the middle of that box.
		g_Menu.DrawBox((int)strlen(szStats) * kFontWidth + kTileWidth*2, kTileWidth*2, kMapStatsX, kMapStatsY);
		g_Menu.DrawString(szStats, (int)strlen(szStats), kMapStatsX + kTileWidth, kMapStatsY + kTileHeight-3);
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
			POINT index = m_vTiles[i].GetIndex();

			// If this tile is under our cursor, we can then replace it's data
			if(index.x == x && index.y == y)
			{
				// Set the data of the tile on the map with the new inserted tile data
				m_vTiles[i] = *pTile;
				m_vTiles[i].SetIndex(index.x, index.y);
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
			if(m_vItems[i].GetIndex().x == mapX && m_vItems[i].GetIndex().y == mapY)
			{
				if(m_vItems[i].GetBitmap())
					DeleteObject(m_vItems[i].GetBitmap());
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
			if(m_vMonsters[i].GetIndex().x == mapX && m_vMonsters[i].GetIndex().y == mapY)
			{
				if(m_vMonsters[i].GetBitmap())
					DeleteObject(m_vMonsters[i].GetBitmap());
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
			if(m_vNpcs[i].GetIndex().x == mapX && m_vNpcs[i].GetIndex().y == mapY)
			{
				if(m_vNpcs[i].GetBitmap())
					DeleteObject(m_vNpcs[i].GetBitmap());

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
			if(m_vExits[i].GetIndex().x == mapX && m_vExits[i].GetIndex().y == mapY)
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
		if(x == m_vTiles[i].GetIndex().x && y == m_vTiles[i].GetIndex().y)
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
		if(x == pExit->GetIndex().x && y == pExit->GetIndex().y)
		{
			char szNewMap[255] = {0};

			// Draw the new map, and due a little pause with the windows Sleep() function.
			// This is so it shows that we actually walked on that spot before we change maps.
			SetDrawFlag(true);
			Draw();
			g_Buffer.SwapBackBuffer(FALSE);
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
		if(x == pNpc->GetIndex().x && y == pNpc->GetIndex().y)
		{
			// Move the player back so it's not bumpin' and grinding with the npc
			g_Player.MovePlayerBack();

			// Display the message of the npc and pause.  Notice that 
			// we add the npc's name to the string so we know who's talking.
			char szDialog[5000] = {0};
			sprintf(szDialog, "%s: %s", pNpc->GetName(), pNpc->GetMessage());
			DisplayDialog(szDialog);
			break;
		}
	}
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// CHECK FOR ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks if the user is over an item and should pick it up or not
/////
///////////////////////////////// CHECK FOR ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::CheckForItem(int x, int y)
{
	// This is simple and identical to the other check functions.  We want
	// to go through each item in the list and see if the position matches
	// the position passed in.  If it does, we then want to pick the item
	// up and then erase it from the map.  
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		CItem *pItem = &m_vItems[i];

		// Check if the player position is the same as the current item
		if(x == pItem->GetIndex().x && y == pItem->GetIndex().y)
		{
			// Create a new item and then copy the current item's data to the new item
			CItem newItem;
			memcpy(&newItem, pItem, sizeof(CItem));

			// If our inventory isn't full, let's add it, otherwise do nothing
			if(g_Player.GetInventorySize() < kMaxItems)
			{
				// Let's add the new item and then delete it from the map
				g_Player.AddItem(newItem);
				m_vItems.erase(m_vItems.begin() + i);
			}
			return;
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// DISPLAY DIALOG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a string and displays it in a dialog box with some formating
/////
///////////////////////////////// DISPLAY DIALOG \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DisplayDialog(char *szDialog)
{
	// Get the dialog length and calculate the number of lines of dialog
	int dialogLength = (int)strlen(szDialog);
	int dialogLines = dialogLength / kDlgLineLength;

	// We will start the dialog text at the position below
	POINT dlgPosition = {100 + kTileWidth, 100 + kTileHeight-3};
		
	// We will draw the actual box around the dialog at (100,100)
	g_Menu.DrawBox(kDlgLineLength * kFontWidth + kTileWidth*2, kTileHeight*2 + (dialogLines * kFontHeight), 100, 100);

	// Go through all of the characters in the message and draw them
	for(int i = 0; i < dialogLength; i++)
	{
		// Draw the current character to the screen at the current location
		g_Menu.DrawString(&szDialog[i], 1, dlgPosition.x, dlgPosition.y);

		// Here we do some math to see if we have reached the end of the dialog box
		if(!((i + 1) % kDlgLineLength))
		{
			// Put the x position back to the beginning of the dialog box and increase Y by 1 line
			dlgPosition.x -= (kDlgLineLength - 1) * kFontWidth;
			dlgPosition.y += kFontHeight;
		}	
		else
			dlgPosition.x += kFontWidth;	// Otherwise we just increase x until it gets to the end

		// Now that the current character is drawn, let's display it to the screen
		g_Buffer.SwapBackBuffer(FALSE);

		// We want a typewriter effect so we just put a simple delay between each character
		Sleep(20);
	}

	// If the user wants to stop the dialog, they can hit the space bar or
	// escape.  We use our input object to check for windows messages as well.
	while(!g_Input.IsKeyDown(VK_SPACE) && !g_Input.IsKeyDown(VK_ESCAPE))
	{
		// Check for standard windows messages and stop if there is a quit message
		if(g_Input.HandleWindowsMessages() == -1)
			return;
	}

	// Pause for a tenth of a second after the dialog is done
	Sleep(100);
}


///////////////////////////////// MOVE NPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This moves all the npcs in a random direction
/////
///////////////////////////////// MOVE NPC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::MoveNpcs()
{
	// Go through all of the npcs
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		// Only move the npc if they have a speed
		if(m_vNpcs[i].GetSpeed() <= 0)
			continue;

		// If the random number is 2500, then try to move the npc
		if((rand() % 50000000) == 2500)
		{
			// Get the current npc's position
			POINT position = m_vNpcs[i].GetIndex();
			
			// Move the npcs, according to their spead
			switch(rand() % (500 / m_vNpcs[i].GetSpeed()))
			{
				case 0:	position.x += 1;	break;
				case 1:	position.x -= 1;	break;
				case 2:	position.y += 1;	break;
				case 3:	position.y -= 1;	break;
			}

			// Before we assign the new position, we check to see if it's a collision
			if(!IsACollision(position.x, position.y))
			{
				// If no collision, let's position the npc and draw their new position
				m_vNpcs[i].SetIndex(position.x, position.y);
				SetDrawFlag(true);
				g_Map.Draw();
				g_Buffer.SwapBackBuffer(FALSE);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we added a function for checking for items and also
// added the code in Draw() in order to display the stats bar at the
// bottom of the screen.  This is toggled by the space bar.
// We added some more variables in the constructor to initialize also.
// You can maybe take a peek at Map.h to see the added prototypes and
// then jump over to Menu.cpp to see the beef of the inventory code.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
