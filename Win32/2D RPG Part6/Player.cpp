//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 6)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// We added 3 functions to our CPlayer class: ResetLists(), LoadPlayer() and SavePlayer().
// We save and load in the same type of method we use for loading and saving maps.
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// RESET LISTS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This clears the items and party lists, used when loading a new game file
/////
///////////////////////////////// RESET LISTS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::ResetLists()			
{ 
	// This makes sure our list data is totally clear when we load
	// in a game file.  a call to vector::clear() just isn't good enough.
	memset(&m_vItems, 0, sizeof(m_vItems));  
	memset(&m_vParty, 0, sizeof(m_vParty)); 
}


///////////////////////////////// SAVE PLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This saves all the player's data, inventory and party member's data
/////
///////////////////////////////// SAVE PLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::SavePlayer(FILE *fp)
{
	// Since the player has some lists associated with it, like
	// inventory and party members, we can't just do a simple
	// fwrite() for a the player, we need to break it down into
	// a bunch of fwrite()'s to our game .sav file.  This is done below.

	// Write the basic data of the player to the file
	fwrite(&m_position, sizeof(POINT), 1, fp);
	fwrite(&m_lastPosition, sizeof(POINT), 1, fp);
	fwrite(&m_health, sizeof(int), 1, fp);
	fwrite(&m_healthMax, sizeof(int), 1, fp);
	fwrite(&m_strength, sizeof(int), 1, fp);
	fwrite(&m_protection, sizeof(int), 1, fp);
	fwrite(&m_experience, sizeof(int), 1, fp);
	fwrite(&m_level, sizeof(int), 1, fp);
	fwrite(&m_gold, sizeof(int), 1, fp);
	fwrite(m_szName, sizeof(char), kMaxNameLen, fp);

	// Get the size for each of our lists
	int partySize = GetPartySize();
	int inventorySize = GetInventorySize();

	// Write the sizes to the file so we know how many to read in later
	fwrite(&partySize, sizeof(int), 1, fp);
	fwrite(&inventorySize, sizeof(int), 1, fp);

	// To save the inventory, this is simple.  We already wrote
	// a function for this in the CItem class.  We just go through
	// all the items and save them to the file.
	for(int i = 0; i < inventorySize; i++)
	{
		// Get the current item in the list and save it
		CItem *pItem = &m_vItems[i];
		pItem->SaveItem(fp);
	}

	// Now we need to save data about the equipped items for the player.
	// We do this by just saving the index of the equipped item in our list
	// to the file.  If there isn't an equipped item for a certain part of
	// the body, then it should return a -1.  We check this when loading in
	// the game file.  We use a function we made, GetItemIndex() to give us
	// the index of the currently equipped item.
	int headIndex   = GetItemIndex(g_Player.GetEquipment(kHead));
	int chestIndex  = GetItemIndex(g_Player.GetEquipment(kChest));
	int weaponIndex = GetItemIndex(g_Player.GetEquipment(kWeapon));
	int feetIndex   = GetItemIndex(g_Player.GetEquipment(kFeet));

	// Write the equipped item's indices to the file
	fwrite(&headIndex,   sizeof(int), 1, fp);
	fwrite(&chestIndex,  sizeof(int), 1, fp);
	fwrite(&weaponIndex, sizeof(int), 1, fp);
	fwrite(&feetIndex,   sizeof(int), 1, fp);

	// Now we need to go through and save all the party members data too.
	// Party members don't have any items for other party members, so we
	// don't need to worry about that stuff, just their main data.
	for(int i = 1; i < partySize; i++)
	{
		// Get the current party member
		CPlayer *pPlayer = GetPartyMember(i);

		// Write their data to the save file
		fwrite(&pPlayer->m_position, sizeof(POINT), 1, fp);
		fwrite(&pPlayer->m_lastPosition, sizeof(POINT), 1, fp);
		fwrite(&pPlayer->m_health, sizeof(int), 1, fp);
		fwrite(&pPlayer->m_healthMax, sizeof(int), 1, fp);
		fwrite(&pPlayer->m_strength, sizeof(int), 1, fp);
		fwrite(&pPlayer->m_protection, sizeof(int), 1, fp);
		fwrite(&pPlayer->m_experience, sizeof(int), 1, fp);
		fwrite(&pPlayer->m_level, sizeof(int), 1, fp);
		fwrite(&pPlayer->m_gold, sizeof(int), 1, fp);
		fwrite(pPlayer->m_szName, sizeof(char), kMaxNameLen, fp);
	}
}


///////////////////////////////// LOAD PLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads all the player's data, inventory and party member's data
/////
///////////////////////////////// LOAD PLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CPlayer::LoadPlayer(FILE *fp)
{
	// The code for this function is exactly the same format as the
	// SavePlayer() function, but we use fread() to read the data instead
	// of writing it.  Remember, if we save it in one order, we need to
	// read it in the exact order, otherwise it won't work and most likely
	// will corrupt data and memory.

	// Read all of the player's saved data
	fread(&m_position, sizeof(POINT), 1, fp);
	fread(&m_lastPosition, sizeof(POINT), 1, fp);
	fread(&m_health, sizeof(int), 1, fp);
	fread(&m_healthMax, sizeof(int), 1, fp);
	fread(&m_strength, sizeof(int), 1, fp);
	fread(&m_protection, sizeof(int), 1, fp);
	fread(&m_experience, sizeof(int), 1, fp);
	fread(&m_level, sizeof(int), 1, fp);
	fread(&m_gold, sizeof(int), 1, fp);
	fread(m_szName, sizeof(char), kMaxNameLen, fp);

	int partySize = 0;
	int inventorySize = 0;

	// Read in the sizes of the party and inventory lists
	fread(&partySize, sizeof(int), 1, fp);
	fread(&inventorySize, sizeof(int), 1, fp);
	
	// Let's clear the lists before we load in the data using fread()
	ResetLists();

	// Go through and load each item, then add that item to the player's inventory
	for(int i = 0; i < inventorySize; i++)
	{
		CItem item;
		item.LoadItem(fp);
		AddItem(item);
	}

	int headIndex = 0, chestIndex = 0, weaponIndex = 0, feetIndex = 0;

	// Read in the equipped item's indices
	fread(&headIndex,   sizeof(int), 1, fp);
	fread(&chestIndex,  sizeof(int), 1, fp);
	fread(&weaponIndex, sizeof(int), 1, fp);
	fread(&feetIndex,   sizeof(int), 1, fp);

	// Now we need to check to see if the index is a valid index into
	// the inventory list.  We should have saved a -1 for places where
	// the user didn't have any equipped items.  We check this below and
	// set the item to the player if it's a valid index.
	if(headIndex >= 0)
		SetEquipment(g_Player.GetItem(headIndex),   kHead);
	if(chestIndex >= 0)
		SetEquipment(g_Player.GetItem(chestIndex),  kChest);
	if(weaponIndex >= 0)
		SetEquipment(g_Player.GetItem(weaponIndex), kWeapon);
	if(feetIndex >= 0)
		SetEquipment(g_Player.GetItem(feetIndex),   kFeet);

	// Now that we have a loaded player, we can add it to the part (index 0)
	AddPlayerToParty(&g_Player);

	// Just like how we wrote the party members to the save game file, 
	// we read them in, then add them to our party.
	for(int i = 1; i < partySize; i++)
	{
		// Here we allocate memory for a new player.  We release this
		// memory in the deconstructor of our CPlayer class.
		CPlayer *pPlayer = new CPlayer;

		// Read in the current party member's data
		fread(&pPlayer->m_position, sizeof(POINT), 1, fp);
		fread(&pPlayer->m_lastPosition, sizeof(POINT), 1, fp);
		fread(&pPlayer->m_health, sizeof(int), 1, fp);
		fread(&pPlayer->m_healthMax, sizeof(int), 1, fp);
		fread(&pPlayer->m_strength, sizeof(int), 1, fp);
		fread(&pPlayer->m_protection, sizeof(int), 1, fp);
		fread(&pPlayer->m_experience, sizeof(int), 1, fp);
		fread(&pPlayer->m_level, sizeof(int), 1, fp);
		fread(&pPlayer->m_gold, sizeof(int), 1, fp);
		fread(pPlayer->m_szName, sizeof(char), kMaxNameLen, fp);

		// Let's get the bitmap file is associated with this player (add the .bmp to the name)
		char szPlayer[kMaxNameLen] = {0};
		strcpy(szPlayer, kNpcsDir);
		strcat(szPlayer, pPlayer->m_szName);
		strcat(szPlayer, ".bmp");

		// Here we load the bmp image of the player
		pPlayer->m_image = g_Buffer.LoadABitmap(szPlayer);

		// Add the member to our party ("*Music*...like it's 1999...")
		AddPlayerToParty(pPlayer);
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added 3 new functions to the player class in order to load and
// save our player data to and from a save game file.  Since there is
// lists assigned to the player, we needed to save the player data
// first, then go through the data in the lists and save/load each item
// in the list.
//
// Let's finally go to SaveLoad.cpp and see the functions which control
// the saving and loading of game files.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
