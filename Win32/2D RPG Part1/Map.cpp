//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 1)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "main.h"			// Include our main header file
#include "Map.h"			// Include our tile header file which holds the classes and defines


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This file defines the basic starting functions for our map class, as well as handles
// the initializing of our other classes of tile types by their constructors.  We have
// code for loading the maps, drawing the maps, collision and handling exits.  Much of
// this code comes from the code in our Map Editor.
// 
// The only thing really new is the function CheckForExit(), which looks for an exit
// and then opens the new map attached to that exit if one is found.  This small, but
// important code is what allows us to move from map to map when we hit an exit.  We
// also added IsACollision() which checks to see if the player collided or not, along
// with a function to free our bitmaps so that we don't have memory leaks: FreeResources().
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

	// Just like in the map editor, we need to load the file format in
	// the same order as it was saved in the editor.  Then we can draw
	// the map, once everything is loaded into our CMap object, g_Map.

	// Since we have a lot of maps, we put them in a Maps\ directory.
	// This makes it cleaner to work with.  We want to usually keep the
	// resource files away from the main code files so we don't get confused.
	// Below we check if we need to add the "Maps\" part to our map name so 
	// it will load correctly.  If we can't find the Maps\ directory in the
	// file name, then we will add it to the file name.
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
	// In order for the character to walk to different maps, we need to check
	// for the exits, then load the next map that the player should go to, moving
	// the character to the new position on the new map.

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


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we defined the code to load maps, draw the maps, then
// check collision and exit tiles on the map.  Much of the code was
// taken from our map editor code, so it should be very familiar to you. 
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
