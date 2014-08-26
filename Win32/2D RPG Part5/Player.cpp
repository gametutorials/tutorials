//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 5)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// Nothing new was added to this file.
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
	memset(&m_position, 0, sizeof(POINT));
	memset(&m_lastPosition, 0, sizeof(POINT));
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
	// Get the size of our party
	int partySize = (int)m_vParty.size();

	// Go through the whole party execept the main player and free them if allocated
	for(int i = 1; i < partySize; i++)
	{
		CPlayer *pPlayer = m_vParty[i];

		if(pPlayer)
			delete pPlayer;
	}
}


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our player with an image and position
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::Init(HBITMAP image, int x, int y)
{
	// Set the players image and x,y position
	m_image = image;
	m_position.x = x;
	m_position.y = y;

	// Initialize the remaining player data
	m_healthMax = m_health = 50;
	m_strength = 3;
	m_protection = 3;
	m_level = 1;
	m_gold = 25;
}


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

	// If we have an equipped item, store it's name
	if(m_pHead)		strcpy(szHead, m_pHead->GetItemName());
	if(m_pChest)	strcpy(szChest, m_pChest->GetItemName());
	if(m_pWeapon)	strcpy(szWeapon, m_pWeapon->GetItemName());
	if(m_pFeet)		strcpy(szFeet, m_pFeet->GetItemName());

	// Erase the item
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

	// Below we check the item against all of the equipped items
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
	g_Map.InsertTile(pItem, kItemType, m_position.x, m_position.y);
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

	// When we use an item we need to add the item's stats to the player's stats
	if(m_health + pItem->GetLifeInc() <= m_healthMax)
		m_health += pItem->GetLifeInc();	

	// Increase the other stats to the player
	m_strength += pItem->GetStrengthInc();				
	m_protection += pItem->GetProtectionInc();

	// If the item we just used isn't equipped to the player we need to erase it
	if(pItem->GetItemType() == kItem)
		EraseItemFromInventory(pItem);
}


///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws the player character over the map
/////
///////////////////////////////// DRAW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::Draw()
{
	// Here we set the portion to draw for our transparent tiles
	RECT portion = {0, 0, kTileWidth, kTileHeight};

	// Convert the position from tile coordinates to screen coordinates
	int x = m_position.x * kTileWidth;
	int y = m_position.y * kTileHeight;

	// Draw the player with a transparent blit
	g_Buffer.DisplayTransparentBitmap(m_image, x, y, portion);
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
		if(!g_Map.IsACollision(m_position.x, m_position.y - 1))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.y--;
			return true;
		}
	}
	else if(direction == kDown)			// If the player went down
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.x, m_position.y + 1))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.y++;
			return true;
		}
	}
	else if(direction == kLeft)			// If the player went left
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.x - 1, m_position.y))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.x--;
			return true;
		}
	}
	else if(direction == kRight)		// If the player went right
	{									// Check for collision in the new position
		if(!g_Map.IsACollision(m_position.x + 1, m_position.y))
		{								// No collision, so change position and return true
			m_lastPosition = m_position;
			m_position.x++;
			return true;
		}
	}

	// Return false, we didn't move because there must have been collision
	return false;
}


///////////////////////////////// ATTACK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This attacks a monster that the player ran into
/////
///////////////////////////////// ATTACK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::Attack(CMonster *pMonster)
{
	// Here we set the default damage ratio assuming we are stronger than the monster
	int damageRatio = (m_strength - pMonster->GetStrength()) * 2;
	bool bMiss = false;			// This holds if we miss or not
	int damage = 0;				// This is the actual damage taken
	string strName = "You";		// This is the default name for the attacker

	// Check if the main player is attacking or another party member, grab their name if so
	if(strcmp(g_Player.GetName(), m_szName) != 0)
		strName = m_szName;

	// If the monster is stronger than us, let's divide our damage ratio by half
	if(m_strength < pMonster->GetStrength())
		damageRatio /= 2;

	// If we get unlucky, let's make the player miss.  The higher the ratio the better
	if(!(rand() % damageRatio))
		bMiss = true;
	else	// otherwise get a random damage between 0 and the damage ratio
		damage = rand() % damageRatio + 1;

	char szMessage[255] = {0};

	// If we missed, say so, otherwise create a message that says our damage report
	if(bMiss)
		sprintf(szMessage, "%s missed the %s!", strName.c_str(), pMonster->GetName());
	else
		sprintf(szMessage, "%s attacked the %s with %d points of damage!", strName.c_str(), pMonster->GetName(), damage);

	// Display the result of our attack
	g_Menu.DrawMessageBox(szMessage);

	// Set the monster's new life
	pMonster->SetLife(pMonster->GetLife() - damage);

	// Pause for a second and clear the input buffer, then wait for the user to hit a key
	Sleep(800);
	
	// Check if the monster is dead
	if(pMonster->GetLife() <= 0)
	{
		// Here we give the player some random gold for their good work!
		g_Player.SetGold(g_Player.GetGold() + (rand() % 15));

		// Display the death message of the monster
		sprintf(szMessage, "%s have killed the %s!", strName.c_str(), pMonster->GetName());
		g_Menu.DrawMessageBox(szMessage);

		// Wait for a second and wait for the user to press a key
		Sleep(1000);
		
		// Increase the player's experience and delete the dead monster
		g_Player.SetExperience(g_Player.GetExperience() + pMonster->GetStrength());
		g_Map.DeleteTile(kMonsterType, pMonster->GetIndex().x, pMonster->GetIndex().y);
		
		int i = 1;

		// Go through and find out how many levels we gained
		while(1)
		{
			if(((g_Player.GetLevel() * 2) * i) <= (g_Player.GetExperience() / 4))
				i++;
			else
				break;
		}

		// Since we started out as 1, we need to minus it back to 0
		i--;

		// If a level was gained, then we need to display that message and set the level
		if(i > 0)
		{
			int levelsGained = i;
			sprintf(szMessage, "You gained %d level(s)!", levelsGained);
			g_Menu.DrawMessageBox(szMessage);
			
			g_Player.SetLevel(g_Player.GetLevel() + levelsGained);

			// Here we do a loop that adds to our stats for each level that we earned.
			// We just add some random numbers with padding to the new stats.
			while(levelsGained--)
			{
				g_Player.SetHealthMax(g_Player.GetHealthMax() + (rand() % 10) + 5);
				g_Player.SetStrength(g_Player.GetStrength() + (rand() % 4) + 2);
				g_Player.SetProtection(g_Player.GetProtection() + (rand() % 5) + 3);
			}

			// Do a pause for the player to see their levels that were gained
			Sleep(1000);
		}
	}
}


///////////////////////////////// UPDATE PARTY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws all the players in the party and checks their vital signs :)
/////
///////////////////////////////// UPDATE PARTY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::UpdateParty()
{
	// Here we get the party size and return if there is no party
	int partySize = (int)m_vParty.size();
	if(!partySize) return;

	// Go through each of the party members from back to front
	for(int i = partySize - 1; i >= 0; i--)
	{
		CPlayer *pPlayer = m_vParty[i];

		// If the current player's health is below or equal to zero they died!
		if(pPlayer->GetHealth() <= 0)
		{
			// Play the death sound and draw a death message, then quit the game
			PlaySound("Death.wav", NULL, SND_FILENAME | SND_ASYNC);
			g_Menu.DrawMessageBox("Your party has been killed!");
			Sleep(1500);
			PostQuitMessage(0);
		}
		else
		{
			// Draw the current player if they aren't dead
			pPlayer->Draw();
		}	
	}
}


///////////////////////////////// MOVE PARTY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Sets the party members positions, each one behind the next party member
/////
///////////////////////////////// MOVE PARTY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::MoveParty()
{
	// Get the party size and return if it's only 1 person
	int partySize = (int)m_vParty.size();
	if(partySize < 2) return;

	// Go through all of the party members and set their new position
	for(int i = 1; i < partySize; i++)
	{
		CPlayer *pPlayer = m_vParty[i];

		// We want to set the current position of the party member to the
		// last position of the party member in front of them.  This is
		// how you get a player to follow right behind you when moving.
		pPlayer->SetLastPosition(pPlayer->GetPosition());
		pPlayer->SetPosition(m_vParty[i-1]->GetLastPosition());
	}
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Nothing was added to this file.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
