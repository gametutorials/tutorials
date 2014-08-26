//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		MapEditor2										 //
//																		 //
//		$Description:	A simple tile based map editor					 //
//																		 //
//***********************************************************************//

#include "main.h"			// Include our main header file
#include "tile.h"			// Include our tile header file which holds the classes and defines


///////////////////////////////////////////////////////////////////////////////////
//
// In this tutorial version we add 2 new functions that allow us to draw tiles
// on our map, as well as delete them: DeleteTile() - InsertTile().
// 
//

///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CTile class.  Initializes all data
/////
///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile::CTile():m_bmpTile(0),m_type(0)
{
	memset(m_szFile, 0, MAX_PATH);
}


///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CItem class.  Initializes all data
/////
///////////////////////////////// CITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CItem::CItem():m_lifeInc(0),m_strengthInc(0),m_protectionInc(0), m_itemType(0)
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
	m_newPosition.x = m_newPosition.y = 0;
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


///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws all the placed tile types on the map
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Draw()
{
	// Here we set the portion to draw for our transparent tiles
	RECT portion = {0, 0, TILE_WIDTH, TILE_HEIGHT};
	int x = 0, y = 0;

	// Go through and draw all the normal map tiles
	for(int i = 0; i < (int)m_vTiles.size(); i++)
	{
		x = m_vTiles[i].GetIndex().x * TILE_WIDTH;
		y = m_vTiles[i].GetIndex().y * TILE_HEIGHT;
		g_Buffer.DisplayBitmap(m_vTiles[i].GetBitmap(), x, y);
	}

	// Go through and draw all the items on the map
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		x = m_vItems[i].GetIndex().x * TILE_WIDTH;
		y = m_vItems[i].GetIndex().y * TILE_HEIGHT;
		g_Buffer.DisplayTransparentBitmap(m_vItems[i].GetBitmap(), x, y, portion);
	}

	// Go through and draw all the monsters on the map
	for(int i = 0; i < (int)m_vMonsters.size(); i++)
	{
		x = m_vMonsters[i].GetIndex().x * TILE_WIDTH;
		y = m_vMonsters[i].GetIndex().y * TILE_HEIGHT;
		g_Buffer.DisplayTransparentBitmap(m_vMonsters[i].GetBitmap(), x, y, portion);
	}

	// Go through and draw all the non player characters on the map
	for(int i = 0; i < (int)m_vNpcs.size(); i++)
	{
		x = m_vNpcs[i].GetIndex().x * TILE_WIDTH;
		y = m_vNpcs[i].GetIndex().y * TILE_HEIGHT;
		g_Buffer.DisplayTransparentBitmap(m_vNpcs[i].GetBitmap(), x, y, portion);
	}

	// Go through and draw all the exits on the map
	for(int i = 0; i < (int)m_vExits.size(); i++)
	{
		x = m_vExits[i].GetIndex().x * TILE_WIDTH;
		y = m_vExits[i].GetIndex().y * TILE_HEIGHT;
		Rectangle(g_Buffer.GetHDC(), x, y, x + TILE_WIDTH, y + TILE_HEIGHT);
	}
}


///////////////////////////////// SET DEFAULT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets a default map with just green grass
/////
///////////////////////////////// SET DEFAULT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::SetDefault()
{
	char szFile[MAX_PATH] = {0};

	// Here we setup the file to load as the default texture (grass)
	sprintf(szFile, "%s\\%s", TILES_DIR, DEFAULT_TILE);

	// Load the default tile file
	HBITMAP bmpDefault = g_Buffer.LoadABitmap(szFile);
	CTile temp;

	// Use a temporary tile for the default tile (Set it's type and name)
	temp.SetFileName(szFile);
	temp.SetType(FLOOR);

	// Make sure we clear and release all the data with our global lists
	m_vTiles.clear();
	m_vItems.clear();
	m_vMonsters.clear();
	m_vNpcs.clear();
	m_vExits.clear();

	// Reset the current map's name so we know for the Save As / Save menu items
	strcpy(m_szMapName, "");

	// Go through the whole map (make it like a 2D array) and assign the
	// map a tile for every spot (the default tile with grass).  Set it's index too.
	for(int y = 0; y < MAP_HEIGHT; y++)
	{
		for(int x = 0; x < MAP_WIDTH; x++)
		{
			// Set the current index for the map and add it to the tile list
			temp.SetIndex(x, y);
			m_vTiles.push_back(temp);
		}
	}

	// Go through all the tiles that were just assigned and load the default bitmap
	for(int i = 0; i < (int)m_vTiles.size(); i++)
		m_vTiles[i].SetBitmap( g_Buffer.LoadABitmap( m_vTiles[i].GetFileName() ) );
}


///////////////////////////// GET CURRENT TYPE SIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns the size of the current <global> list being used
/////
///////////////////////////// GET CURRENT TYPE SIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CMap::GetCurrentTypeSize()
{
	// Check the current type and depending on the type, return the list size (# of tiles)
	if(m_currentTypeFlag == ITEM_TYPE)
		return (int)g_vItems.size();
	else if(m_currentTypeFlag == MONSTER_TYPE)
		return (int)g_vMonsters.size();
	else if(m_currentTypeFlag == NPC_TYPE)
		return (int)g_vNpcs.size();
	else if(m_currentTypeFlag == EXIT_TYPE)
		return 0;	// Exits of course have no tiles so 0
	else			// Else default is a normal tile
		return (int)g_vTiles.size();
}


///////////////////////////// GET CURRENT LIST SIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns the size of the current <map> list being used
/////
///////////////////////////// GET CURRENT LIST SIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CMap::GetCurrentListSize()
{
	// Check the current type and depending on the type, return the list size (# of tiles)
	if(m_currentTypeFlag == ITEM_TYPE)
		return (int)m_vItems.size();
	else if(m_currentTypeFlag == MONSTER_TYPE)
		return (int)m_vMonsters.size();
	else if(m_currentTypeFlag == NPC_TYPE)
		return (int)m_vNpcs.size();
	else if(m_currentTypeFlag == EXIT_TYPE)
		return (int)m_vExits.size();	
	else			// Else default is a normal tile
		return (int)m_vTiles.size();
}


///////////////////////////// GET CURRENT TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns a tile from the current <global> list being used
/////
///////////////////////////// GET CURRENT TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile *CMap::GetCurrentTypeTile(int index)
{
	// Check the current type and depending on the type, return desired tile
	if(m_currentTypeFlag == ITEM_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)g_vItems.size())
			return NULL;
		else
			return &g_vItems[index];
	}
	else if(m_currentTypeFlag == MONSTER_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)g_vMonsters.size())
			return NULL;
		else
			return &g_vMonsters[index];
	}
	else if(m_currentTypeFlag == NPC_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)g_vNpcs.size())
			return NULL;
		else
			return &g_vNpcs[index];
	}
	else if(m_currentTypeFlag == EXIT_TYPE)
		return NULL;	// Exits have no tiles so return NULL
	else				// Else means it must be a normal tile
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)g_vTiles.size())
			return NULL;
		else
			return &g_vTiles[index];
	}
	
}


///////////////////////////// GET CURRENT LIST TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns a tile from the current <map> list being used
/////
///////////////////////////// GET CURRENT LIST TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile *CMap::GetCurrentListTile(int index)
{
	// Check the current type and depending on the type, return desired tile
	if(m_currentTypeFlag == ITEM_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)m_vItems.size())
			return NULL;
		else
			return &m_vItems[index];
	}
	else if(m_currentTypeFlag == MONSTER_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)m_vMonsters.size())
			return NULL;
		else
			return &m_vMonsters[index];
	}
	else if(m_currentTypeFlag == NPC_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)m_vNpcs.size())
			return NULL;
		else
			return &m_vNpcs[index];
	}
	else if(m_currentTypeFlag == EXIT_TYPE)
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)m_vExits.size())
			return NULL;
		else
			return &m_vExits[index];	// Exits are saved on the map so return a exit
	}
	else								// Else return a tile from the normal tile list
	{
		// Make sure index is legal
		if(index < 0 || index >= (int)m_vTiles.size())
			return NULL;
		else							
			return &m_vTiles[index];
	}
}

///////////////////////////////// SET TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a file name and returns it's type (i.e. wall, floor, etc...)
/////
///////////////////////////////// SET TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

char SetTileType(char *szFile)
{
	// See if the substring "*_FILE" is in the current file, then return type
	if(strstr(szFile, WALL_FILE))
		return WALL;
	else if(strstr(szFile, DOOR_FILE))
		return DOOR;
	else		
		return FLOOR;	// It must be a normal floor tile then
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

	// Since the CTile *pTile isn't always going to be a CTile, we need to
	// do some type casting depending on each type that we are inserting.
	// We first do a cast to the desired type, and then de-reference the tile
	// since the vector class member function push_back() does not take a pointer.

	// If the current tile is not for exits, then set the type accordingly
	if(g_pCurrentMap->GetCurrentType() != EXIT_TYPE)
		type = pTile->GetType();

	if(type == ITEM)						// If we are inserting an item
	{
		// Cast the current tile to an item, add it to the list, then set it's index
		m_vItems.push_back(*(CItem*)pTile);
		m_vItems.back().SetIndex(x, y);
	}
	else if(type == MONSTER)				// If we are inserting an monster
	{
		// Cast the current tile to an monster, add it to the list, then set it's index
		m_vMonsters.push_back(*(CMonster*)pTile);
		m_vMonsters.back().SetIndex(x, y);
	}
	else if(type == NPC)					// If we are inserting an npc
	{
		// Cast the current tile to an npc, add it to the list, then set it's index
		m_vNpcs.push_back(*(CNpc*)pTile);
		m_vNpcs.back().SetIndex(x, y);
	}
	else if(type == EXIT)					// If we are inserting an exit
	{
		CExit exit;							// Create a temporary exit

		m_vExits.push_back(exit);			// Add it to our list of exits
		m_vExits.back().SetIndex(x, y);		// Set it's index and then store the map name
		m_vExits.back().SetCurrentMap(g_pCurrentMap->GetMapName());
	}
	else									// Otherwise it must be a normal map tile
	{
		// Since every spot on our map has a tile, we don't need to add a tile,
		// but replace the current tile in the spot we chose to draw on.
		// So, go through all of the tiles and check for the desired index and change the data.
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
/////	This function deletes a tile from the map depending on the current type
/////
///////////////////////////////// DELETE TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DeleteTile(int mapX, int mapY)
{
	// Since we only delete tiles of the current type selected, we need to do a 
	// check for the current type of tile, then delete the tile under the cursor.
	// We don't delete normal map tiles so we don't check for TILE_TYPE.

	if(m_currentTypeFlag == ITEM_TYPE)				// If the current tile type is an item
	{
		// Go through all of the items on our map
		for(int i = (int)m_vItems.size() - 1; i >= 0; i--)
		{
			// Check if the current item is the same position as our cursor
			if(m_vItems[i].GetIndex().x == mapX && m_vItems[i].GetIndex().y == mapY)
			{
				// Erase the item we are checking using the STL vector list erase() function
				m_vItems.erase(m_vItems.begin() + i);
				break;
			}
		}
	}
	else if(m_currentTypeFlag == MONSTER_TYPE)		// If the current tile type is an monster
	{
		// Go through all of the monsters on our map
		for(int i = (int)m_vMonsters.size() - 1; i >= 0; i--)
		{
			// Check if the current monster is the same position as our cursor
			if(m_vMonsters[i].GetIndex().x == mapX && m_vMonsters[i].GetIndex().y == mapY)
			{
				// Erase the current monster
				m_vMonsters.erase(m_vMonsters.begin() + i);
				break;
			}
		}
	}
	else if(m_currentTypeFlag == NPC_TYPE)			// If the current tile type is an npc
	{
		// Go through all of the npcs on our map
		for(int i = (int)m_vNpcs.size() - 1; i >= 0; i--)
		{
			// Check if the current npc is the same position as our cursor
			if(m_vNpcs[i].GetIndex().x == mapX && m_vNpcs[i].GetIndex().y == mapY)
			{
				// Erase this npc
				m_vNpcs.erase(m_vNpcs.begin() + i);
				break;
			}
		}
	}
	else if(m_currentTypeFlag == EXIT_TYPE)			// If the current tile type is an exit
	{
		// Go through all of the exits on our map
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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////// LOAD TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a path and loads the tiles from that path (i.e. items)
/////
///////////////////////////// LOAD TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadTileType(char *szDir, int type)
{
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	char szFile[MAX_PATH] = {0};

	// We copy the file to a temp variable and add a * so that
	// FindFirstFile() knows to look in the desired directory at all files.
	strcpy(szFile, szDir);
	strcat(szFile, "*");

	// Find the first file in the directory (usually "." or "..")
	hFind = FindFirstFile(szFile, &findFileData);

	// Make sure we have a valid handle and a file was found
	if(hFind == INVALID_HANDLE_VALUE)
	{
		// Print an error message so we know we need to add files, then quit
		sprintf(szFile, "Unable to find tiles in %s", szDir);
		MessageBox(g_hWnd, szFile, "Error", MB_OK);
		PostQuitMessage(0);
	}
	else
	{
		// Continue to look in this directory until we run out of files
		while (FindNextFile(hFind, &findFileData) != 0) 
		{
			// Store the current file found with it's respective directory
			sprintf(szFile, "%s\\%s", szDir, findFileData.cFileName);

			// Check if it's a bmp file using a simple substring check
			if(strstr(findFileData.cFileName, ".bmp"))
			{				
				// If the path we are looking in is for items
				if(type == ITEM_TYPE)
				{
					// Initialize a temp Item and add it to our global list
					CItem temp;
					temp.SetFileName(szFile);
					temp.SetType( ITEM );
					g_vItems.push_back(temp);
				}
				// If the path we are looking in is for monsters
				else if(type == MONSTER_TYPE)
				{
					// Initialize a temp monster and add it to our global list
					CMonster temp;
					temp.SetFileName(szFile);
					temp.SetType( MONSTER );
					g_vMonsters.push_back(temp);
				}
				// If the path we are looking in is for Npcs
				else if(type == NPC_TYPE)
				{
					// Initialize a temp npc and add it to our global list
					CNpc temp;
					temp.SetFileName(szFile);
					temp.SetType( NPC );
					g_vNpcs.push_back(temp);
				}
				// Else the path must be for normal tiles
				else
				{
					// Initialize a temp tile and add it to our global list
					CTile temp;
					temp.SetFileName(szFile);
					temp.SetType( SetTileType( temp.GetFileName() ) );
					g_vTiles.push_back(temp);
				}
			}
		}

		// Destroy the handle and close the files
		FindClose(hFind);
	}
}


///////////////////////////////// LOAD TILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads all the tiles information and bitmaps
/////
///////////////////////////////// LOAD TILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int LoadTiles()
{
	// Here we load all the tile types from their respective directories
	LoadTileType(TILES_DIR, TILE_TYPE);
	LoadTileType(ITEMS_DIR, ITEM_TYPE);
	LoadTileType(MONSTERS_DIR, MONSTER_TYPE);
	LoadTileType(NPCS_DIR, NPC_TYPE);

	// Here we go through all of the global tiles and load their bitmaps
	for(int i = 0; i < (int)g_vTiles.size(); i++)
		g_vTiles[i].SetBitmap(g_Buffer.LoadABitmap(g_vTiles[i].GetFileName()));
	for(int i = 0; i < (int)g_vItems.size(); i++)
		g_vItems[i].SetBitmap(g_Buffer.LoadABitmap(g_vItems[i].GetFileName()));
	for(int i = 0; i < (int)g_vMonsters.size(); i++)
		g_vMonsters[i].SetBitmap(g_Buffer.LoadABitmap(g_vMonsters[i].GetFileName()));
	for(int i = 0; i < (int)g_vNpcs.size(); i++)
		g_vNpcs[i].SetBitmap(g_Buffer.LoadABitmap(g_vNpcs[i].GetFileName()));

	// We return the tile's size because that is our default type.
	// We also subtract the MAP_HEIGHT - 1 to offset the scroll bar max position.
	return (int)g_vTiles.size() - MAP_HEIGHT - 1;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added InsertTile() and DeleteTile() to this file.  These functions
// allow us to draw on the map and erase tiles from the map.  In the 
// next tutorial we will add loading and saving functionality.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials       
