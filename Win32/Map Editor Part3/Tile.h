#ifndef _TILE_H
#define _TILE_H

#include "main.h"

// This is the number of X tiles for our map
#define MAP_WIDTH			20
// This is the number of the Y tiles for our map
#define MAP_HEIGHT			15

// This is the width in pixels of each tile
#define TILE_WIDTH			32
// This is the height in pixels of each tile
#define TILE_HEIGHT			32

// The max characters for names
#define MAX_NAME_LEN		50

// The max message length for NPC's, etc... 
#define MAX_MESSAGE_LEN		1000

// Create a define for our window height
#define HEIGHT				(TILE_HEIGHT * MAP_HEIGHT)											
// Create a define for our window width
#define WIDTH				(TILE_WIDTH * MAP_WIDTH)	

// These defines hold our info file paths for tile types
#define MONSTER_INFO_FILE	"monsters\\monsters.txt"
#define ITEM_INFO_FILE		"items\\items.txt"
#define NPC_INFO_FILE		"npcs\\npcs.txt"

// These are our paths for each tile type
#define TILES_DIR			"Tiles\\"
#define ITEMS_DIR			"Items\\"
#define MONSTERS_DIR		"Monsters\\"
#define NPCS_DIR			"Npcs\\"

// This is the default tile to be loaded and display on the map
#define DEFAULT_TILE		"grass_f.bmp"

// The map/level file extension for our game
#define MAP_EXT				".map"

// <<////////////// TILE DEFINES ///////////////////>>

// The normal map tiles have these written in the file name
#define FLOOR_FILE			"_f"
#define WALL_FILE			"_w"
#define DOOR_FILE			"_d"

// The defines are stored in the CTile: m_type variable
#define FLOOR				'f'
#define WALL				'w'
#define DOOR				'd'
#define EXIT				'e'
#define MONSTER				'm'
#define ITEM				'i'
#define NPC					'n'

// These are used to set the possible types of lists
#define TILE_TYPE			0
#define ITEM_TYPE			1
#define MONSTER_TYPE		2
#define NPC_TYPE			3
#define EXIT_TYPE			4

// These are the types of items we can have, used for equipping items to a player
enum eItemTypes
{
	kItem = 0,
	kHead,
	kChest,
	kWeapon,
	kFeet
};


// Our <Base Class> for each tile type.  
// This is then inherited from to create other tile types.
class CTile
{
public:

	CTile();	// The constructor (initializing function)

	// Our standard "Set" functions for our protected data
	void SetBitmap(HBITMAP bmpNew)	{ m_bmpTile = bmpNew;			 }
	void SetFileName(char *szNew)	{ strcpy(m_szFile, szNew);		 }
	void SetType(char newType)		{ m_type = newType;				 } 
	void SetIndex(int x, int y)		{ m_index.x = x;  m_index.y = y; }

	// Our standard "Get" functions for the protected data
	HBITMAP GetBitmap()		{ return m_bmpTile; }
	char *GetFileName()		{ return m_szFile;  }
	char GetType()			{ return m_type;	}
	POINT GetIndex()		{ return m_index;   }

// Set it protected so that inherited classes can access if necessary
protected:	

	HBITMAP m_bmpTile;				// The handle to the tile's bitmap
	char m_szFile[MAX_PATH];		// The file name of the bitmap
	char m_type;					// The type of the tile
	POINT m_index;					// The index on the map of the tile
};


// This class inherits from <Base Class> CTile and stores our item information
class CItem : public CTile
{
public:

	CItem();			// The constructor

	// Our standard "Set" functions for private data
	void SetLifeInc(int newInc)			{ m_lifeInc = newInc;			}
	void SetStrengthInc(int newInc)		{ m_strengthInc = newInc;		}
	void SetProtectionInc(int newInc)	{ m_protectionInc = newInc;		}
	void SetName(char *szNew)			{ strcpy(m_szItem, szNew);		}
	void SetItemType(int type)			{ m_itemType = type;			}

	// Our standard "Get" functions for private data
	int GetItemType()		{ return m_itemType;		}
	int GetLifeInc()		{ return m_lifeInc;			}
	int GetStrengthInc()	{ return m_strengthInc;		}
	int GetProtectionInc()	{ return m_protectionInc;	}
	char *GetItemName()		{ return m_szItem;			}

private:

	int m_itemType;					// The type of item (kWeapon, kItem, kHead, kChest, kFeet)
	int m_lifeInc;					// The life increase for item
    int m_strengthInc;				// The strength increase for this item
	int m_protectionInc;			// The protection increase for this item
	char m_szItem[MAX_NAME_LEN];	// The name of the item
};


// This class inherits from <Base Class> CTile and stores our monster information
class CMonster : public CTile
{
public:

	CMonster();		// The constructor

	// Our standard "Set" functions for the private data
	void SetLife(int newLife)			{ m_life = newLife;				}
	void SetStrength(int newStrength)	{ m_strength = newStrength;		}
	void SetName(char *szNew)			{ strcpy(m_szMonster, szNew);	}
	void SetSpeed(int newSpeed)			{ m_speed = newSpeed;			}

	// Our standard "Get" functions for the private data
	int GetLife()				{ return m_life;		}
	int GetStrength()			{ return m_strength;	}
	int GetSpeed()				{ return m_speed;		}
	char *GetName()				{ return m_szMonster;	}

private:

	int m_life;						// The life of the monster
    int m_strength;					// The strength of the monster
	int m_speed;					// The speed of the monster
	char m_szMonster[MAX_NAME_LEN]; // The name of the monster
};


// This class inherits from <Base Class> CTile and stores our Npc information
class CNpc : public CTile
{
public:

	CNpc();			// The constructor

	// Our standard "Set" functions for the private data
	void SetLife(int newLife)			{ m_life = newLife;			}
	void SetStrength(int newStrength)	{ m_strength = newStrength;	}
	void SetName(char *szNew)			{ strcpy(m_szName, szNew);	}
	void SetMessage(char *szNew)		{ strncpy(m_szMessage, szNew, MAX_MESSAGE_LEN);	}
	void SetSpeed(int newSpeed)			{ m_speed = newSpeed;		}

	// Our standard "Get" functions for the private data
	int GetLife()				{ return m_life;		}
	int GetStrength()			{ return m_strength;	}
	int GetSpeed()				{ return m_speed;		}
	char *GetName()				{ return m_szName;		}
	char *GetMessage()			{ return m_szMessage;	}

private:

	int m_life;						// The life of the Npc
    int m_strength;					// The strength of the Npc
	int m_speed;					// The speed of the Npc
	char m_szName[MAX_NAME_LEN];	// The name of the Npc
	char m_szMessage[MAX_MESSAGE_LEN];	// The message that the Npc says when you talk to them
};


// This class inherits from <Base Class> CTile and stores our exit information
class CExit : public CTile
{
public:

	CExit();			// The constructor

	// Our standard "Set" functions for the private data
	void SetCurrentMap(char *szCurrent)		{ strcpy(m_szCurrentMap, szCurrent);	}
	void SetNewMap(char *szNew)				{ strcpy(m_szNewMap, szNew);			}
	void SetNewPos(POINT position)			{ m_newPosition = position;				}

	// Our standard "Get" functions for the private data
	char *GetCurrentMap()		{ return m_szCurrentMap;	 }
	char *GetNewMap()			{ return m_szNewMap;		 }
	POINT GetNewPos()			{ return m_newPosition;		 }

private:

	char m_szCurrentMap[MAX_PATH];	// The current map name of this exit
	char m_szNewMap[MAX_PATH];		// The destination map's name
	POINT m_newPosition;			// The new position on the next map to exit to
};


// This class handles most of our map functionality
class CMap
{
public:

	CMap();			// The constructor

	// This resets the map to green grass
	void SetDefault();

	// This draws our map
	void Draw();

	// These set and get the map name
	void SetMapName(char *szName)	{ strcpy(m_szMapName, szName); }
	char *GetMapName()				{ return m_szMapName;			 }

	// These set and get the current tile type being used
	void SetCurrentType(int type)	{ m_currentTypeFlag = type;    }
	int GetCurrentType()			{ return m_currentTypeFlag;	 }
	
	// These get the current global list's size and specific tile
	int GetCurrentTypeSize();
	CTile *GetCurrentTypeTile(int index);

	// These get the current map list's size and specific tile
	int GetCurrentListSize();
	CTile *GetCurrentListTile(int index);

	// This inserts a tile from the currently selected tile on the map
	void InsertTile(CTile *pTile, int x, int y);

	// This deletes a tile from the map depending on the x and y position
	void DeleteTile(int mapX, int mapY);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This returns a handle to a bitmap from the global lists for the same file name
	HBITMAP FindHBitmap(char *szFile, int type);

	// This loads a file
	void Load(char *szFile);

	// This saves a file
	void Save(char *szFile);

	// This loads the info files associated with the different tile types
	void SetTileInfo(CTile *pTile, char *szFile);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


private:

	char m_szMapName[MAX_PATH];		// The name of the map
	int m_currentTypeFlag;			// The flag that determines the list type in the tool bar

	vector<CTile> m_vTiles;			// The normal map tiles list on our map
	vector<CItem> m_vItems;			// The item list for our map
	vector<CMonster> m_vMonsters;	// The monster list for our map
	vector<CNpc>  m_vNpcs;			// The npc list for our map
	vector<CExit> m_vExits;			// The exit list for our map
};

// This loads the tiles for the global tile lists and returns the size for the scroll bar
int LoadTiles();


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added FindHBitmap(), Load(), Save() and SetTileInfo() to this file.  
// These functions allow us to load and save maps.  In the next 
// tutorial we will add the exit code.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
