#ifndef _PLAYER_H
#define _PLAYER_H

// Include the main header file
#include "main.h"			

// This is our starting position for our player on the map
const int kPlayerStartX		= 10;
const int kPlayerStartY		= 10;

// These are the images for our game characters in the game
const char kPlayerImage[]	= "Npcs\\Player.bmp";
const char kTrendorImage[]	= "Npcs\\Trendor.bmp";

// This is the direction constants that we use for moving the character
enum eDirections
{
	kUp = 0,
	kDown,
	kLeft,
	kRight
};

// This stores the constants for the types of items to equip
enum eEquipment
{
	kEqHead = 0,
	kEqChest,
	kEqWeapon,
	kEqFeet
};


// This is our player class
class CPlayer 
{
public:
	
	// The constructor and deconstructor
	CPlayer();
	~CPlayer();

	// This inits the player with an image and a position
	void Init(HBITMAP image, int x, int y);

	// This draws the player to the screen
	void Draw();

	// This moves the player according to the desired direction (kUp, kDown, kLeft, kRight)
	bool Move(int direction);

	// This restores the players previous position
	void MovePlayerBack()						{ m_position = m_lastPosition; }

	// These functions set the player's data
	void SetName(char *szPlayerName)			{ strcpy(m_szName, szPlayerName); }
	void SetHealth(int playerHealth)			{ m_health = playerHealth; }
	void SetHealthMax(int maxHealth)			{ m_healthMax = maxHealth; }
	void SetStrength(int playerStrength)		{ m_strength = playerStrength; }
	void SetProtection(int protection)			{ m_protection = protection; }
	void SetExperience(int experience)			{ m_experience = experience; }
	void SetLevel(int level)					{ m_level = level; }
	void SetGold(int gold)						{ m_gold = gold; }
	
	// These function retrieve info about the player
	int GetStrength()							{ return m_strength; }
	int GetHealth()								{ return m_health; }
	int GetHealthMax()							{ return m_healthMax; }
	int GetProtection()							{ return m_protection; }
	int GetExperience()							{ return m_experience; }
	int GetLevel()								{ return m_level; }
	int GetGold()								{ return m_gold; }
	char *GetName()								{ return m_szName; } 
	
	// These get and set the player's image
	HBITMAP GetImage()							{ return m_image; }
	void SetImage(HBITMAP newImage)				{ m_image = newImage; }

	// These get and set the players position
	POINT GetPosition()							{ return m_position; }
	void SetPosition(POINT newPosition)			{ m_position = newPosition; }

	// These get and set the players last position
	POINT GetLastPosition()						{ return m_lastPosition; }
	void SetLastPosition(POINT lastPosition)	{ m_lastPosition = lastPosition; }

	// This tells us if the player is still alive or not
	bool IsAlive()								{ return (m_health > 0); }

	// This adds an item to our inventory
	void AddItem(const CItem &newItem);

	// This drops an item from our inventory
	void DropItem(CItem *pItem);

	// This uses and equips (if necessary) an item to the player
	void UseItem(CItem *pItem);

	// This equips an item to the player according to it's type
	void SetEquipment(CItem *pItem, int type);

	// These are get functions for our inventory
	int GetInventorySize()				{ return (int)m_vItems.size(); }
	CItem *GetItem(int index)			{ return &m_vItems[index];     }
	CItem *GetItem(char *szName);
	CItem *GetEquipment(int type);
	int GetItemIndex(CItem *pItem);

	// This safely erases an item from the inventory
	void EraseItemFromInventory(CItem *pItem);

	// This returns a pointer to our inventory list
	vector<CItem> *GetInventory()		{ return &m_vItems; }

	// These are get functions for our party management
	int GetPartySize()							{ return (int)m_vParty.size(); }
	CPlayer *GetPartyMember(int index)			{ return m_vParty[index]; }

	// This adds a player to our current part
	void AddPlayerToParty(CPlayer *pPlayer)		{ m_vParty.push_back(pPlayer); }

	// This draws all the party members and checks if they are still breathing
	void UpdateParty();

	// This handles the following of party members when the player moves
	void MoveParty();

	// This attacks a monster
	void Attack(CMonster *pMonster);

private:	

	HBITMAP m_image;				// The player's image
	POINT m_position;				// The player's position
	POINT m_lastPosition;			// The player's last position
	char m_szName[kMaxNameLen];		// The player's name
	int m_health;					// The player's current health
	int m_strength;					// The player's strength
	int m_protection;				// The player's protection
	int m_experience;				// The player's experience points
	int m_level;					// The player's level
	int m_gold;						// The player's gold

	int m_healthMax;				// The player's max health

	CItem *m_pHead;					// The current equipment on the player's head
	CItem *m_pChest;				// The current equipment on the player's chest
	CItem *m_pWeapon;				// The current player's weapon
	CItem *m_pFeet;					// The current equipment on the player's feet

	vector<CPlayer*> m_vParty;		// The list of players in your party
	vector<CItem> m_vItems;			// The inventory list for the player
};


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Nothing new was added to this file.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com   
