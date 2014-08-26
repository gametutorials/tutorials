#ifndef _MAP_H
#define _MAP_H

// Include our main header file
#include "main.h"								

// This is the number of X tiles for our map
const int kMapWidth			= 80;
// This is the number of the Y tiles for our map
const int kMapHeight		= 45;

// The max characters for names
const int kMaxNameLen		= 50;

// The max message length for NPC's, etc... 
const int kMaxMessageLen	= 1000;

// The map/level file extension for our editor
const char kMapExt[]		= ".map";

// The directory where are maps are stored
const char kMapsDir[]		= "Maps\\";

// This is the first map that we load in at the beginning of the game
const char kStartMap[]		= "Default.map";

// We include the text file that holds all the items in the game because we
// will use this when doing shops that the user can buy and sell from.  These are 
// not the same items as the ones in the map editor tutorial, but the same format.
const char kItemInfoFile[]	= "items.txt";

// These are the tile types that we use to know which tile type to cast a tile to
enum eTileTypes
{
	kTileType = 0,
	kItemType,
	kMonsterType,
	kNpcType,
	kExitType
};

// These are the types than an item can be.  If it's not equipment, it's a normal item.
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
	void SetChar(CHAR_INFO charNew)	{ m_charTile = charNew;			 }
	void SetType(char newType)		{ m_type = newType;				 } 
	void SetIndex(int x, int y)		{ m_index.X = x;  m_index.Y = y; }

	// Our standard "Get" functions for the protected data
	CHAR_INFO GetChar()		{ return m_charTile; }
	char GetType()			{ return m_type;	 }
	COORD GetIndex()		{ return m_index;    }
	bool IsSolid()			{ return m_bSolid;   }

	// The set function for the tile class
	void SetSolidValue(bool newValue)		{ m_bSolid = newValue;		}
	void SetActionKey(int actionKey)		{ m_actionKey = actionKey;	}
	int GetActionKey()						{ return m_actionKey;		}

// Set it protected so that inherited classes can access if necessary
protected:	

	CHAR_INFO m_charTile;			// The handle to the tile's bitmap
	char m_type;					// The type of the tile
	COORD m_index;					// The index on the map of the tile
	bool m_bSolid;					// This tells us if we should collide with the character
	int m_actionKey;				// This stores the tiles action key for your game event triggers
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

	int m_itemType;					// This tells us what type this item is
	int m_lifeInc;					// The life increase for item
    int m_strengthInc;				// The strength increase for this item
	int m_protectionInc;			// The protection increase for this item
	char m_szItem[kMaxNameLen];		// The name of the item
};


// This class inherits from <Base Class> CTile and stores our monster information
class CMonster : public CTile
{
public:

	CMonster();		// The constructor

	// Our standard "Set" functions for the private data
	void SetLife(int newLife)			{ m_life = newLife;				}
	void SetStrength(int newstrength)	{ m_strength = newstrength;		}
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
	char m_szMonster[kMaxNameLen];  // The name of the monster
};


// This class inherits from <Base Class> CTile and stores our Npc information
class CNpc : public CTile
{
public:

	CNpc();			// The constructor

	// Our standard "Set" functions for the private data
	void SetLife(int newLife)			{ m_life = newLife;			}
	void SetStrength(int newstrength)	{ m_strength = newstrength;	}
	void SetName(char *szNew)			{ strcpy(m_szName, szNew);	}
	void SetMessage(char *szNew)		{ strncpy(m_szMessage, szNew, kMaxMessageLen);	}
	void SetSpeed(int newSpeed)			{ m_speed = newSpeed;		}

	// Our standard "Get" functions for the private data
	int GetLife()				{ return m_life;		}
	int GetStrength()			{ return m_strength;	}
	int GetSpeed()				{ return m_speed;		}
	char *GetName()				{ return m_szName;		}
	char *GetMessage()			{ return m_szMessage;	}

private:

	int m_life;							// The life of the Npc
    int m_strength;						// The strength of the Npc
	int m_speed;						// The speed of the Npc
	char m_szName[kMaxNameLen];			// The name of the Npc
	char m_szMessage[kMaxMessageLen];	// The message that the Npc says when you talk to them
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

	// This loads a file
	void Load(const char *szFile);

	// This assigns image2's background color to image1 and returns a new CHAR_INFO with that info
	CHAR_INFO AddNewBackground(CHAR_INFO image1, CHAR_INFO image2);

	// This inserts a tile from the currently selected tile on the map
	void InsertTile(CTile *pTile, int type, int x, int y);

	// This deletes a tile from the map depending on the x and y position
	void DeleteTile(int type, int mapX, int mapY);

	// This returns if the player collided or not with a solid tile
	bool IsACollision(int x, int y);

	// This checks if the player walked into an exit, then takes the exit if so
	void CheckForExit(int x, int y);
	
	// This tells the Draw() function to draw the map again
	void SetDrawFlag(bool bDraw)	{ m_bDrawScreen = bDraw; }
	
private:
									// This is our screen buffer that holds the color ASCII characters
	CHAR_INFO m_screenBuffer[kMapWidth * kMapHeight];

	char m_szMapName[MAX_PATH];		// The name of the map
	bool m_bDrawScreen;				// The draw screen flag
	
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
// You will notice that much of this code is from the map editor tutorial series, but
// we changed the #defines to actual constant variables so that we can view them in
// our debugger when debugging the code.  You can't view #defines in the debugger,
// which makes it really frustrating sometimes to debug your code.  In larger projects
// it should be something that you try and steer away from, since you will most likely
// be doing a great deal of debugging for your game.
//
// We define many of the map's data constants and the different tile type classes
// for items, tiles, npcs and monsters.  We also define the important CMap class
// for handling most of our map functionality.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials   
