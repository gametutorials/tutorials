//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		MapEditor5										 //
//																		 //
//		$Description:	A simple tile based map editor					 //
//																		 //
//***********************************************************************//

#include "main.h"			// Include our main header file
#include "tile.h"			// Include our tile header file which holds the classes and defines


///////////////////////////////////////////////////////////////////////////////////
//
// There wasn't much added in this file, but we added a new function, GetTile()
// which returns a pointer to a tile for attaching to the cursor.  We also
// added the variables m_actionKey and m_bSolid to our CTile constructor.
// That way they also get initialized with the other variables.
//
//

///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for the CTile class.  Initializes all data
/////
///////////////////////////////// CTILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile::CTile():m_bmpTile(0),m_type(0),m_actionKey(0),m_bSolid(false)  // *** NEW ***
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


///////////////////////////////// FIND HBITMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns a bitmap handle for a tile type when loading a map
/////
///////////////////////////////// FIND HBITMAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

HBITMAP CMap::FindHBitmap(char *szFile, int type)
{
	if(type == ITEM_TYPE)					// If the current type is an item
	{
		// Go through all of the items in the global item list
		for(int i = 0; i < (int)g_vItems.size(); i++)
		{
			// If the file name is the same then return it's bitmap handle
			if(strcmp(szFile, g_vItems[i].GetFileName()) == 0)
				return g_vItems[i].GetBitmap();
		}
	}
	else if(type == MONSTER_TYPE)			// If the current type is a monster
	{
		// Go through all of the monsters in the global monster list
		for(int i = 0; i < (int)g_vMonsters.size(); i++)
		{
			// If the file name is the same then return it's bitmap handle
			if(strcmp(szFile, g_vMonsters[i].GetFileName()) == 0)
				return g_vMonsters[i].GetBitmap();
		}
	}
	else if(type == NPC_TYPE)				// If the current type is a npc
	{
		// Go through all of the npc in the global npc list
		for(int i = 0; i < (int)g_vNpcs.size(); i++)
		{
			// If the file name is the same then return it's bitmap handle
			if(strcmp(szFile, g_vNpcs[i].GetFileName()) == 0)
				return g_vNpcs[i].GetBitmap();
		}
	}
	else									// Otherwise it must be a normal map tile
	{
		// Go through all of the items in the global item list
		for(int i = 0; i < (int)g_vTiles.size(); i++)
		{
			// If the file name is the same then return it's bitmap handle
			if(strcmp(szFile, g_vTiles[i].GetFileName()) == 0)
				return g_vTiles[i].GetBitmap();
		}
	}

	// Return default 0
	return 0;
}


///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads a map from a file
/////
///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Load(char *szFile)
{
	// Open our map that we want to load in "read" and "binary" mode
	FILE *fp = fopen(szFile, "rb");

	// If we couldn’t load the file report an error message
	if(!fp)
	{
		MessageBox(g_hWnd, "Can't Find File", "Error", MB_OK);
		return;
	}

	// Reset all of our map list data before we load more in
	m_vTiles.clear();
	m_vItems.clear();
	m_vMonsters.clear();
	m_vNpcs.clear();
	m_vExits.clear();

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
	
	// Assign bitmap handles to each of the tiles
	for(int i = 0; i < tileSize; i++)
		m_vTiles[i].SetBitmap( FindHBitmap(m_vTiles[i].GetFileName(), TILE_TYPE) );
	for(int i = 0; i < itemSize; i++)
		m_vItems[i].SetBitmap( FindHBitmap(m_vItems[i].GetFileName(), ITEM_TYPE) );
	for(int i = 0; i < monsterSize; i++)
		m_vMonsters[i].SetBitmap( FindHBitmap(m_vMonsters[i].GetFileName(), MONSTER_TYPE) );
	for(int i = 0; i < npcSize; i++)
		m_vNpcs[i].SetBitmap( FindHBitmap(m_vNpcs[i].GetFileName(), NPC_TYPE) );
	for(int i = 0; i < exitSize; i++)
		m_vExits[i].SetBitmap( 0 );

	// Close our file that we opened, a success!
	fclose(fp);

	// Let's create some string variables to store just the file name, not including the path
	char szTemp[255] = {0};
	char szFileName[255] = {0};
	LPSTR pFileName = szFileName;

	// Here we use a good win32 function to extract just the file name from the path.
	// The file name will be stored in pFileName.  This way it's not hard coded to a path.
	GetFullPathName(szFile, 255, (LPSTR)szTemp, &pFileName);

	// Set the name of this map
	strcpy(m_szMapName, pFileName);
}


///////////////////////////////// SAVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function saves a map to a file
/////
///////////////////////////////// SAVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::Save(char *szFile)
{
	FILE *fp = NULL;

	// Check if the file doesn't have a .map extension.  If not, add it at no extra charge :)
	if(!strstr(szFile, MAP_EXT))
		strcat(szFile, MAP_EXT);

	// Open our file to "write" and in "binary".
	fp = fopen(szFile, "wb");

	// If we couldn’t create or open the file report an error
	if(!fp)
	{
		MessageBox(g_hWnd, "Can't Write File", "Error", MB_OK);
		return;
	}

	// Store all the sizes for each of the map's lists
	int tileSize		= (int)m_vTiles.size();
	int itemSize		= (int)m_vItems.size();
	int monsterSize		= (int)m_vMonsters.size();
	int npcSize			= (int)m_vNpcs.size();
	int exitSize		= (int)m_vExits.size();

	// Save the sizes for each of the map lists
	fwrite(&tileSize, sizeof(int), 1, fp);
	fwrite(&itemSize, sizeof(int), 1, fp);
	fwrite(&monsterSize, sizeof(int), 1, fp);
	fwrite(&npcSize, sizeof(int), 1, fp);
	fwrite(&exitSize, sizeof(int), 1, fp);

	// Go through all the lists and save each tile to disk
	for(int i = 0; i < tileSize; i++)
		fwrite(&m_vTiles[i], sizeof(CTile), 1, fp);
	for(int i = 0; i < itemSize; i++)
		fwrite(&m_vItems[i], sizeof(CItem), 1, fp);
	for(int i = 0; i < monsterSize; i++)
		fwrite(&m_vMonsters[i], sizeof(CMonster), 1, fp);
	for(int i = 0; i < npcSize; i++)
		fwrite(&m_vNpcs[i], sizeof(CNpc), 1, fp);
	for(int i = 0; i < exitSize; i++)
		fwrite(&m_vExits[i], sizeof(CExit), 1, fp);

	// Close the file, the file is saved!
	fclose(fp);

	// Set the file name for this map that we just saved
	strcpy(m_szMapName, szFile);
}


///////////////////////////////// SET TILE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads the info files associated with items, monsters and npcs.
/////
///////////////////////////////// SET TILE INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::SetTileInfo(CTile *pTile, char *szFile)
{
	// Open our info file in "read" mode.
	FILE *fp = fopen(szFile, "r");
	char szFileName[MAX_PATH] = {0};
	char szName[MAX_PATH] = {0};
	char szType[MAX_PATH] = {0};

	// Make sure we found the file, if not report the error
	if(!fp)
	{
		sprintf(szFile, "Unable to find tile info in %s", szFile);
		MessageBox(g_hWnd, szFile, "Error", MB_OK);
		return;
	}

	// Keep going through the file until we reach the end of it
	while(!feof(fp))
	{
		if(pTile->GetType() == ITEM)				// If the tile type is an item
		{
			// Cast the tile to an item tile
			CItem *pItem = (CItem*)pTile;
			int life = 0, str = 0, protection = 0;

			// Read in the current line for this item in the info file
			fscanf(fp, "%s > lifeInc: %d strengthInc: %d protectionInc: %d type: %s\n", szName, &life, &str, &protection, szType);

			// Store the actual file name of the tile
			strcpy(szFileName, szName);
			strcat(szFileName, ".bmp");

			// Store the item type in an STL string object
			string strType = szType;
			int type = kItem;

			// Here we check to see which type the item is, and set a numeric value for the type
			if(strType == "head") type = kHead;
			else if(strType == "chest")  type = kChest;
			else if(strType == "weapon") type = kWeapon;
			else if(strType == "feet")   type = kFeet;

			// Check if the tile we are searching for is this line we just read in
			if(strstr(_strlwr(pItem->GetFileName()), _strlwr(szFileName)))
			{
				// Set the info read in for this tile then quit the function
				pItem->SetLifeInc(life);
				pItem->SetStrengthInc(str);
				pItem->SetProtectionInc(protection);
				pItem->SetItemType(type);
				pItem->SetName(szName);
				break;
			}
		}
		else if(pTile->GetType() == MONSTER)		// If the tile type is an monster
		{
			// Cast the tile passed in to a monster tile
			CMonster *pMonster= (CMonster*)pTile;
			int life = 0, str = 0, speed = 0;

			// Read in the current line for this monster in the info file
			fscanf(fp, "%s > life: %d strength: %d speed: %d\n", szName, &life, &str, &speed);

			// Store the actual file name of the tile
			strcpy(szFileName, szName);
			strcat(szFileName, ".bmp");

			// Check if the tile we are searching for is this line we just read in
			if(strstr(_strlwr(pMonster->GetFileName()), _strlwr(szFileName)))
			{
				// Set the info read in for this tile then quit the function
				pMonster->SetLife(life);
				pMonster->SetStrength(str);
				pMonster->SetSpeed(speed);
				pMonster->SetName(szName);
				break;
			}
		}
		else if(pTile->GetType() == NPC)			// If the tile type is an npc
		{
			// Cast the tile passed in to a npc
			CNpc *pNpc= (CNpc*)pTile;
			char szMessage[MAX_MESSAGE_LEN] = {0};
			int life = 0, str = 0, speed = 0;

			// Read in the current line for this npc in the info file
			fscanf(fp, "%s > life: %d strength: %d speed: %d Message: ", szName, &life, &str, &speed);

			// Get a whole line of text for the npc's message
			fgets(szMessage, MAX_MESSAGE_LEN, fp);

			// Get rid of a weird character read in from fgets()
			szMessage[strlen(szMessage) - 1] = '\0';

			// Store the actual file name of the tile
			strcpy(szFileName, szName);
			strcat(szFileName, ".bmp");

			// Check if the tile we are searching for is this line we just read in
			if(strstr(_strlwr(pNpc->GetFileName()), _strlwr(szFileName)))
			{
				// Set the info read in for this tile then quit the function
				pNpc->SetLife(life);
				pNpc->SetStrength(str);
				pNpc->SetSpeed(speed);
				pNpc->SetName(szName);
				pNpc->SetMessage(szMessage);
				break;
			}
		}
	}

	// Close the file, a success!
	fclose(fp);
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


///////////////////////////////// INSERT TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function inserts a tile from the cursor and saves it on the map
/////
///////////////////////////////// INSERT TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::InsertTile(CTile *pTile, int x, int y)
{
	char type = 'e';						// Set a type for exit since exit's don't have tiles

	// If the current tile is not for exits, then set the type accordingly
	if(g_pCurrentMap->GetCurrentType() != EXIT_TYPE)
		type = pTile->GetType();

	if(type == ITEM)						// If we are inserting an item
	{
		CItem newItem;
		newItem = *(CItem*)pTile;

		// Cast the current tile to an item, add it to the list, then set it's index
  		m_vItems.push_back(newItem);
		m_vItems.back().SetIndex(x, y);
	}
	else if(type == MONSTER)				// If we are inserting an monster
	{
		CMonster newMonster;
		newMonster = *(CMonster*)pTile;

		// Cast the current tile to an monster, add it to the list, then set it's index
		m_vMonsters.push_back(newMonster);
		m_vMonsters.back().SetIndex(x, y);
	}
	else if(type == NPC)					// If we are inserting an npc
	{
		CNpc newNpc;
		newNpc = *(CNpc*)pTile;

		// Cast the current tile to an npc, add it to the list, then set it's index
		m_vNpcs.push_back(newNpc);
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
/////	This function deletes a tile from the map depending on the current type
/////
///////////////////////////////// DELETE TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DeleteTile(int mapX, int mapY)
{
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

///////////////////////////// GET TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a tile pointer from the map at mapX,mapY
/////
///////////////////////////// GET TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CTile *CMap::GetTile(int mapX, int mapY)
{
	// Go through all of the tiles on our map
	for(int i = (int)m_vTiles.size() - 1; i >= 0; i--)
	{
		// Check if the current tile is the same position as our cursor
		if(m_vTiles[i].GetIndex().x == mapX && m_vTiles[i].GetIndex().y == mapY)
		{
			// return a pointer to the found tile
			return &m_vTiles[i];
		}
	}

	// Return no pointer since we couldn't find a tile at the specified position
	return NULL;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////// DELETE BLANK EXITS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through all the blank exits and deletes them
/////
///////////////////////////// DELETE BLANK EXITS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::DeleteBlankExits()
{
	// Get the current size of the list
	int size = (int)m_vExits.size();

	// Go through all of the exits
	for(int i = size - 1; i >= 0; i--)
	{
		// If the file name isn't a valid file name
		if(strlen(m_vExits[i].GetNewMap()) <= 1)
		{
			// Use our STL vector function erase() to erase this exit from the list
			m_vExits.erase(m_vExits.begin() + i);
		}
	}
}


///////////////////////////// SET EXITS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This goes through all the exits and fills in their information
/////
///////////////////////////// SET EXITS \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CMap::SetExits(char *szMap, CExit *pExit)
{
	// Go through all of the exits
	for(int i = 0; i < (int)m_vExits.size(); i++)
	{
		// If this exit doesn't have a file name...
		if(strlen(m_vExits[i].GetNewMap()) <= 1)
		{
			// Fill in this exit's data (destination name and position)
			m_vExits[i].SetNewMap(szMap);
			m_vExits[i].SetNewPos(pExit->GetIndex());
		}
	}
}


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
					
					if(temp.GetType() == FLOOR)
						temp.SetSolidValue(false);
					else
						temp.SetSolidValue(true);

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
	{
		g_vItems[i].SetBitmap(g_Buffer.LoadABitmap(g_vItems[i].GetFileName()));
		g_Map.SetTileInfo(&g_vItems[i], ITEM_INFO_FILE);		// Load the tile's info from the info file
	}
	for(int i = 0; i < (int)g_vMonsters.size(); i++)
	{
		g_vMonsters[i].SetBitmap(g_Buffer.LoadABitmap(g_vMonsters[i].GetFileName()));
		g_Map.SetTileInfo(&g_vMonsters[i], MONSTER_INFO_FILE);	// Load the tile's info from the info file
	}
	for(int i = 0; i < (int)g_vNpcs.size(); i++)
	{
		g_vNpcs[i].SetBitmap(g_Buffer.LoadABitmap(g_vNpcs[i].GetFileName()));
		g_Map.SetTileInfo(&g_vNpcs[i], NPC_INFO_FILE);			// Load the tile's info from the info file
	}

	// We return the tile's size because that is our default type.
	// We also subtract the MAP_HEIGHT - 1 to offset the scroll bar max position.
	return (int)g_vTiles.size() - MAP_HEIGHT - 1;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added a new function to our CMap class, GetTile().  This returns
// a pointer to a tile under the cursor's position.  This is used for
// copying tiles to the cursor when the user clicks the right mouse
// button.  We also a couple variables to the CTile constructor.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials       

