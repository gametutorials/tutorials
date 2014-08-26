#ifndef _OBJECT_H
#define _OBJECT_H

// Include our main header file
#include "main.h"								

// This is the number of X tiles for our map
#define MAP_WIDTH			80
// This is the number of the Y tiles for our map
#define MAP_HEIGHT			50

// This is the height in characters for the editor below the map
#define EDITOR_HEIGHT		5

// The max characters for names
#define MAX_NAME_LEN		50

// The max message length for NPC's, etc... 
#define MAX_MESSAGE_LEN		1000

// The map/level file extension for our editor
#define MAP_EXT				".map"

// These defines hold our info files for tile types
#define MONSTER_INFO_FILE	"monsters.txt"
#define ITEM_INFO_FILE		"items.txt"
#define NPC_INFO_FILE		"npcs.txt"

// These are used to set the possible types of lists
#define TILE_TYPE			0
#define ITEM_TYPE			1
#define MONSTER_TYPE		2
#define NPC_TYPE			3
#define EXIT_TYPE			4

// Our <Base Class> for each tile type.  
// This is then inherited from to create other tile types.
class CTile
{
public:

	CTile();	// The constructor (initializing function)

	// Our standard "Set" functions for our protected data
	void SetChar(CHAR_INFO charNew)	{ m_charTile = charNew;			 }
	void SetType(char newType)		{ m_type = newType;				 } 
	void SetIndex(int x, int y)		{ m_index.X = x;  m_index.Y = y; }

	// Our standard "Get" functions for the protected data
	CHAR_INFO GetChar()		{ return m_charTile; }
	char GetType()			{ return m_type;	}
	COORD GetIndex()		{ return m_index;   }

// Set it protected so that inherited classes can access if necessary
protected:	

	CHAR_INFO m_charTile;			// The handle to the tile's bitmap
	char m_type;					// The type of the tile
	COORD m_index;					// The index on the map of the tile
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

	// Our standard "Get" functions for private data
	int GetLifeInc()		{ return m_lifeInc;			}
	int GetStrengthInc()	{ return m_strengthInc;		}
	int GetProtectionInc()	{ return m_protectionInc;	}
	char *GetItemName()		{ return m_szItem;			}

private:

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
	void SetNewPos(COORD position)			{ m_newPosition = position;				}

	// Our standard "Get" functions for the private data
	char *GetCurrentMap()		{ return m_szCurrentMap;	 }
	char *GetNewMap()			{ return m_szNewMap;		 }
	COORD GetNewPos()			{ return m_newPosition;		 }

private:

	char m_szCurrentMap[MAX_PATH];	// The current map name of this exit
	char m_szNewMap[MAX_PATH];		// The destination map's name
	COORD m_newPosition;			// The new position on the next map to exit to
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

	// This returns the current map's name
	char *GetMapName()				{ return m_szMapName; }

	// These set and get the current tile type being used
	void SetCurrentType(int type)	{ m_currentTypeFlag = type;    }
	int GetCurrentType()			{ return m_currentTypeFlag;	 }
	
	// This loads a file
	void Load(char *szFile);

	// This saves a file
	void Save(char *szFile);

	// This returns the current index into the screen buffer according to screen coordinates
	CHAR_INFO GetEditorSelection(int x, int y) { return m_screenBuffer[x + y * MAP_WIDTH]; }

	// This assigns image2's background color to image1 and returns a new CHAR_INFO with that info
	CHAR_INFO AddNewBackground(CHAR_INFO image1, CHAR_INFO image2);

	// This assigns image2's foreground color to image1 and returns a new CHAR_INFO with that info
	CHAR_INFO AddNewForeground(CHAR_INFO image1, CHAR_INFO image2);

	// This displays a prompt with the current tile's name or any other desired info
	void DrawTileInfo();

	// This inserts a tile from the currently selected tile on the map
	void InsertTile(CTile *pTile, int x, int y);

	// This deletes a tile from the map depending on the x and y position
	void DeleteTile(int mapX, int mapY);

	// This loads the info files associated with the different tile types
	void SetTileInfo(int type, char *szFile);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This gets the current selected list's size
	int GetCurrentListSize();

	// This returns a pointer to a tile in a map's list, according to the passed in index
	CTile *GetCurrentListTile(int index);

	// This goes through all empty exits and fills them in
	void SetExits(char *szMap, CExit *pExit);

	// This deletes all empty exits that were not filled in correctly
	void DeleteBlankExits();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


private:
									// This is our screen buffer that holds the color ASCII characters
	CHAR_INFO m_screenBuffer[MAP_WIDTH * MAP_HEIGHT];

	char m_szMapName[MAX_PATH];		// The name of the map
	int m_currentTypeFlag;			// The flag that determines the list type being used (item,...)

	vector<CTile> m_vTiles;			// The normal map tiles list on our map
	vector<CItem> m_vItems;			// The item list for our map
	vector<CMonster> m_vMonsters;	// The monster list for our map
	vector<CNpc>  m_vNpcs;			// The npc list for our map
	vector<CExit> m_vExits;			// The exit list for our map
};


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added 4 functions to our CMap class:
//
// GetCurrentListSize(), GetCurrentListTile(), SetExits(), DeleteBlankExits()
//
// These are for handling the exits.  The first 2 are just data access
// functions for private data, and the next 2 actually setup our exits
// and delete all the blank ones that are setup if we escaped right in
// the middle of setting exits.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials   
