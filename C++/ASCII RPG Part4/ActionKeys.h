#ifndef _ACTIONKEYS_H
#define _ACTIONKEYS_H


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// These are the special action keys for our game
const int kMaxActionKeys  = 1000;		// This stores the max amount of keys for our array
const int kShopKeeperKey  = 555;		// This is the key assigned for shop keepers	
const int kTreasureKey    = 499;		// This key is for when the player picks up the treasure
const int kValkarSceneKey = 500;		// This is the Valkar cut scene key
const int kAddJaxKey	  = 25;			// This key is set when we add Jax to our party
const int kKingSceneKey   = 1;			// This is the cut scene key for the King's dialog

// This is the treasures position right behind Valkar in the Palace
const int kTreasureX = 39;
const int kTreasureY = 15;

// This is our action key class for handling special events
class CAction
{
public:
	
	// Our constructor
	CAction();
	
	// This takes a action key and a tile, default is NULL
	void HandleKey(int actionKey, CTile *pTile = NULL);

	// This is called every time a map loads to check for special events/cut scenes
	void HandleMaps();

	// This sets and gets the action key array
	int *GetActionKeys()					{ return m_actionKeys; }
	void SetActionKeys(int *pActionKeys)	{ memcpy(m_actionKeys, pActionKeys, sizeof(m_actionKeys)); }

	// This sets and gets action keys from the array
	int GetActionKey(int index)						{ return m_actionKeys[index]; }
	void SetActionKey(int index, int value = 1)		{ m_actionKeys[index] = value; }

	// This resets the action key array to 0
	void ResetActionKeys()	{ memset(m_actionKeys, 0, sizeof(m_actionKeys)); }

private:

	// This is our action key array which stores all the game event keys
	int m_actionKeys[kMaxActionKeys];

};

#endif

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// We added our defines for the action keys and also created the
// CAction class which handles the action keys and special events.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials   

