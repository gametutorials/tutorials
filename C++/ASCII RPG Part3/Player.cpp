//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 3)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Nothing new is in this file since the last tutorial.  Just kidding :)  Hopefully
// I didn't scare away anyone... :)  In this file we added quite a bit of functions to
// handle the player's inventory.  We needed support for adding, dropping and equipping
// inventory.  It's a lot easier than you might think, so let's jump down to the code
// and see how we chose to simply implement this.
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CPLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the constructor for our CPlayer class
/////
///////////////////////////////// CPLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CPlayer::CPlayer():m_health(0),m_strength(0),m_experience(0),m_level(0),m_healthMax(0),m_gold(0)
{
	// Like normal, initialize all the data in this class before doing anything
	memset(&m_position, 0, sizeof(COORD));
	memset(&m_lastPosition, 0, sizeof(COORD));
	memset(&m_image, 0, sizeof(CHAR_INFO));
	memset(m_szName, 0, sizeof(m_szName));
	m_pHead = m_pChest = m_pWeapon = m_pFeet = NULL;
}


///////////////////////////////// ~CPLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the deconstructor for our CPlayer class
/////
///////////////////////////////// ~CPLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CPlayer::~CPlayer()
{
	// This isn't being used now, but will be used for freeing memory for party members
}


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our player with an image and position
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::Init(CHAR_INFO image, int x, int y)
{
	// Set the players image and x,y position
	m_image = image;
	m_position.X = x;
	m_position.Y = y;

	// Set the variables to our player defaults
	m_healthMax = m_health = 50;
	m_strength = 3;
	m_protection = 3;
	m_level = 1;
	m_gold = 25;
}



//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// GET ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns a pointer to an item with the same name
/////
///////////////////////////////// GET ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CItem *CPlayer::GetItem(char *szName)
{
	// Go through all of the items
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		CItem *pCurrentItem = &m_vItems[i];

		// If the current item has the name of the desired item
		if(!strcmp(szName,pCurrentItem->GetItemName()))
			return pCurrentItem;
	}

	// Return an invalid item
	return NULL;
}


///////////////////////////////// GET ITEM INDEX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns the index of an item passed in
/////
///////////////////////////////// GET ITEM INDEX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CPlayer::GetItemIndex(CItem *pItem)
{
	// Go through all of the items
	for(int i = 0; i < (int)m_vItems.size(); i++)
	{
		CItem *pCurrentItem = &m_vItems[i];

		// If the current item is the desired item, let's return the index
		if(pItem == pCurrentItem)
			return i;
	}

	// Return an invalid index
	return -1;
}


///////////////////////////////// GET EQUIPMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function gets a pointer to the current equipped item
/////
///////////////////////////////// GET EQUIPMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CItem *CPlayer::GetEquipment(int type)
{
	// Check which item is desired, then return a pointer to it
	switch (type)
	{
		case kHead:		return m_pHead;
		case kChest:	return m_pChest;
		case kWeapon:	return m_pWeapon;
		case kFeet:		return m_pFeet;
		default:		return m_pHead;
	}

	// Return a default head pointer
	return m_pHead;
}


///////////////////////////////// SET EQUIPMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function equips an item to a desired place
/////
///////////////////////////////// SET EQUIPMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::SetEquipment(CItem *pItem, int type)
{
	// Return if there is no valid item
	if(!pItem) return;

	// We need to store the previous item to minus it's stats from the player
	CItem *pLastItem = NULL;

	// Check the type desired to change and store a pointer to it for stats reasons
	switch (type)
	{
		case kHead:		pLastItem = m_pHead;	break;
		case kChest:	pLastItem = m_pChest;	break;
		case kWeapon:	pLastItem = m_pWeapon;	break;
		case kFeet:		pLastItem = m_pFeet;	break;
		default:		pLastItem = m_pHead;	break;
	}

	// If there was an item equipped, let's take back it's stats before we add new ones
	if(pLastItem)
	{
		m_healthMax -= pItem->GetLifeInc();

		// When we take away the max health stat we need to make sure our health is below it
		if(m_health > m_healthMax)
			m_health = m_healthMax;

		m_strength -= pLastItem->GetStrengthInc();				
		m_protection -= pLastItem->GetProtectionInc();
	}

	// Depending on the type, let's store a pointer to the new item
	switch (type)
	{
		case kHead:		m_pHead   = pItem;	break;
		case kChest:	m_pChest  = pItem;	break;
		case kWeapon:	m_pWeapon = pItem;	break;
		case kFeet:		m_pFeet   = pItem;	break;
		default:		m_pHead   = pItem;	break;
	}
}


////////////////////////// ERASE ITEM FROM INVENTORY \\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This safely erases an item from the inventory and fixes equipped item pointers
/////
////////////////////////// ERASE ITEM FROM INVENTORY \\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::EraseItemFromInventory(CItem *pItem)
{
	char szHead[255] = {0};
	char szChest[255] = {0};
	char szWeapon[255] = {0};
	char szFeet[255] = {0};

	// We need this function because of the STL list that we use for our inventory.
	// Since we store pointers to items in the inventory, these can change when we
	// erase an item from the list.  That means we need to go throught he inventory
	// list and restore new pointers for our equipped items.  If we don't do this,
	// the pointers will get mixed up and funny things will happen.  We just store
	// the name of the equipped items, since the pointers will be changed once an
	// item is erased.  After we erase the item, then we get new pointers from the
	// names we saved.

	// If we have an equipped item, store it's name
	if(m_pHead)		strcpy(szHead, m_pHead->GetItemName());
	if(m_pChest)	strcpy(szChest, m_pChest->GetItemName());
	if(m_pWeapon)	strcpy(szWeapon, m_pWeapon->GetItemName());
	if(m_pFeet)		strcpy(szFeet, m_pFeet->GetItemName());

	// Erase the item, going through the list and then finding the item that has the same
	// memory address at the item we are trying to delete (notice the & in front of the item index).
	// Then, to erase, we add the beginning position plus the current index to tell it which one to delete.
	for(int i = 0; i < m_vItems.size(); i++)
	{
		if(pItem == &m_vItems[i])
			m_vItems.erase(m_vItems.begin() + i);
	}

	// Restore the correct pointers from our inventory list
	m_pHead = GetItem(szHead);
	m_pChest = GetItem(szChest);
	m_pWeapon = GetItem(szWeapon);
	m_pFeet = GetItem(szFeet);
}


///////////////////////////////// ADD ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function adds an item to our inventory list and plays a sound
/////
///////////////////////////////// ADD ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::AddItem(const CItem &newItem)
{
	// Every time we pick up an item let's play an asynchronous sound
	// and add it to our item STL vector list.
	PlaySound("ItemPickup.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_vItems.push_back(newItem);
}


///////////////////////////////// DROP ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function drops an item from the player's inventory
/////
///////////////////////////////// DROP ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::DropItem(CItem *pItem)
{
	// We first create a variable that says that the item we are dropping
	// is not equipped to the player.  If it is, we need to set pointer to NULL.
	bool bEquipped = false;

	// Below we check the item against all of the equipped items.  If the
	// item is equipped then we need to set that pointer to NULL and subtract
	// the stats from the player's stats.
	if(pItem == m_pHead)	
	{
		m_pHead   = NULL;	bEquipped = true;
	}
	else if(pItem == m_pChest)
	{
		m_pChest  = NULL;	bEquipped = true;
	}
	else if(pItem == m_pWeapon)		
	{
		m_pWeapon = NULL;	bEquipped = true;
	}
	else if(pItem == m_pFeet)		
	{
		m_pFeet   = NULL;	bEquipped = true;
	}

	// If we are dropping an equipped item we must subtract the item's stats
	if(bEquipped)
	{
		m_healthMax -= pItem->GetLifeInc();

		// When we take away the max health stat we need to make sure our health is below it
		if(m_health > m_healthMax)
			m_health = m_healthMax;

		m_strength -= pItem->GetStrengthInc();				
		m_protection -= pItem->GetProtectionInc();
	}

	// Since we dropped the item we need to add it to the map in case we
	// want to pick it back up again.  After that, we erase it from our inventory.
	g_Map.InsertTile(pItem, kItemType, m_position.X, m_position.Y);
	EraseItemFromInventory(pItem);
}


///////////////////////////////// USE ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our player with an image and position
/////
///////////////////////////////// USE ITEM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::UseItem(CItem *pItem)
{
	// If a weapon or something has special stats that increase the player's
	// health, then we want to increase the max health of the player
	if(pItem->GetItemType() != kItem)
		m_healthMax += pItem->GetLifeInc();

	// When we use an item we need to add the item's stats to the player's stats.
	// If the item isn't like a weapon, then we need to check to make sure that
	// we don't go over our max health.  If not, then add health to the player.
	if(m_health + pItem->GetLifeInc() <= m_healthMax)
		m_health += pItem->GetLifeInc();	

	// Increase the other stats to the player
	m_strength += pItem->GetStrengthInc();				
	m_protection += pItem->GetProtectionInc();

	// If the item we just used isn't equipped to the player we need to erase it
	// from the player's inventory (like food or a potion, etc...).
	if(pItem->GetItemType() == kItem)
		EraseItemFromInventory(pItem);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////



///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws the player character over the map
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::Draw()
{
	char szPlayer[2] = {0};
	DWORD dwResult = 0;

	// Get an output handle for drawing
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	// Add the player character to a string (We need a string for WriteConsoleOutputCharacter())
	sprintf(szPlayer, "%c", m_image.Char);

	// Finally, we draw our player's character and then draw it's color
	WriteConsoleOutputCharacter(hOutput, szPlayer, 1, m_position, &dwResult);
	WriteConsoleOutputAttribute(hOutput, &m_image.Attributes, 1, m_position, &dwResult);
}


///////////////////////////////// MOVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This moves the player in a direction (kUp, kDown, kRight, kLeft)
/////
///////////////////////////////// MOVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CPlayer::Move(int direction)
{
	if(direction == kUp)				// If the player went up
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.X, m_position.Y - 1))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.Y--;
			return true;
		}
	}
	else if(direction == kDown)			// If the player went down
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.X, m_position.Y + 1))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.Y++;
			return true;
		}
	}
	else if(direction == kLeft)			// If the player went left
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.X - 1, m_position.Y))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.X--;
			return true;
		}
	}
	else if(direction == kRight)		// If the player went right
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.X + 1, m_position.Y))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.X++;
			return true;
		}
	}

	// Return false, we didn't move because there must have been collision
	return false;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Hopefully as you saw the new functions in this file you were surprised
// to see how simple it is to create a basic inventory system.  There is
// many little functions that need to be written, but each one is pretty
// small and intuitive.  The only tricky part comes in when we add or drop
// items because we need to take into account the stats that an equipped
// item gave to the player.  If they wear a weapon and it gives them
// extra strength, we need to take those stats away when we change weapons
// or drop/sell that weapon.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
