#ifndef _SHOPKEEPER_H
#define _SHOPKEEPER_H


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

#include "Main.h"

// These are defines for our shop menu (confirm prompt dimensions and max shop keeper's items)
const int kShopPromptWidth = 20;		
const int kShopPromptHeight = 4;
const int kMaxShopKeeperInv = 10;


// This is our shop class.  Notice that we inherit from the menu class
// so that we can use the same functionality as the normal menu.
class CShop : public CMenu
{
public:

	// The constructor
	CShop();

	// This tells us if the user is buying or selling
	bool IsBuying()								{ return m_bBuying; }
	void SetBuyFlag(bool bBuying)				{ m_bBuying = bBuying; }

	// This keeps track of the shop keeper's npc pointer
	void SetShopKeeper(CNpc *pShopKeeper)		{ m_pShopKeeper = pShopKeeper; }
	CNpc *GetShopKeeper()						{ return m_pShopKeeper; }

	// This returns a pointer to the shop keeper's item list
	vector<CItem> *GetInventory()				{ return &m_vItems; }

	// This initializes the store and takes a pointer to the npc for the shop keeper
	void OpenStore(CNpc *pShopKeeper);

	// This loads some random items to sell to the player from the item's text file
	void Load();

	// This is the main draw function for the shop menu
	void Draw();
	
	// This resets all the menu data
	void Reset();

private:

	bool m_bBuying;				// This tells us if the player chose to buy or sell
	CNpc *m_pShopKeeper;		// This stores the shop keepers npc pointer
	vector<CItem> m_vItems;		// This is the item list for the shop keeper
};


#endif

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// This file stores our CShop class and a few definitions.  The shop
// keeper, like the player, has their own inventory list.  We will
// load some random items into this list upon opening the store.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials   
