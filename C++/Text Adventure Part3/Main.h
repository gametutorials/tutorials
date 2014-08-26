#ifndef _MAIN_H
#define _MAIN_H

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
using namespace std;

// As you will see, we stuck everything that was at the top of main.cpp 
// into a header file called main.h.  It is always good to keep our prototypes
// and #defines in header files.  That way other source files can access them
// if needed.   This also makes our code more clear and easier to maneuver around.
// Check below to see the new classes and #defines that were added in this tutorial.

// This is a #define for our game file.  The level data will be read from a text file called World.txt.
#define GAME_FILE "World.txt"

// These defines are to make our return values from GetInput() more clear.  
#define STILL_PLAYING	1
#define QUIT			0

// This define is used to restrict the maximum amount of things to look at in a room.
// I chose 3 as an arbitrary number.  You can increase this to what ever you want.
// This means that we will only be able to look at 3 different things in a room.
// If we have more than 3 keywords in the game file, it will read in only 3 and ignore the rest.
#define MAX_LOOKS		3


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// These defines are exactly like the STILL_PLAYING and QUIT defines, except
// we will be using them for the outcome of fighting the monsters.  New #defines
// were created to be more descriptive than "true" and "false".  After a battle
// is over, one of these constants will be returned from AttackPlayer().
#define PLAYER_STILL_ALIVE	1
#define PLAYER_IS_DEAD		0	


// This is our player class.  An instance of this class is created in Main.cpp,
// which holds the player's information.  We created a bunch of data access
// functions to set and query the players data.  The functions should be very straight
// forward to what they do.  Though there isn't must in the player class now, you will
// add more functions for the player's inventory, party members, etc...
class CPlayer {
public:
	void SetName(string strPlayerName)			{ m_strName = strPlayerName; }
	void SetHealth(int playerHealth)			{ m_health = playerHealth; }
	void SetWeapon(string strPlayerWeapon)		{ m_strWeapon = strPlayerWeapon; }
	void SetDamage(int playerDamage)			{ m_damage = playerDamage; }
	
	string GetName()	const					{ return m_strName; } 
	string GetWeapon()	const					{ return m_strWeapon; }
	
	int GetDamage()		const					{ return m_damage; }
	int GetHealth()		const					{ return m_health; }
	
private:	
	string m_strName;					// This stores the player's name
	int m_health;						// This stores the player's health
	string m_strWeapon;					// This stores the name of the player's weapon
	int m_damage;						// This stores the damage the player's weapon inflicts
};


// This is our monster class.  An instance of this class is created in our tRoom,
// structure.  We created a bunch of data access functions to set and query the 
// monster's data.  The functions should be very straightforward to what they do.  
// Though there isn't must in the player class now, you will add more functions 
// for the monster's AI, inventory, etc...
class CMonster {
public:
	void SetName(string strMonsterName)			{ m_strName = strMonsterName; }
	void SetAttackMessage(string strMessage)	{ m_strAttackMessage = strMessage; }
	void SetHealth(int monsterHealth)			{ m_health = monsterHealth; }
	void SetDamage(int monsterDamage)			{ m_damage = monsterDamage; }

	string GetName()	const					{ return m_strName;	}
	string GetAttackMessage()	const			{ return m_strAttackMessage; }
	
	int GetHealth()		const					{ return m_health;	}
	int GetDamage()		const					{ return m_damage;	}
	
private:								
	string m_strName;					// This stores the monster's name
	string m_strAttackMessage;			// This stores the monster's attatcking message
	int m_health;						// This stores the monster's health
	int m_damage;						// This stores the damage the monster's weapon inflicts
};

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



// This is the room structure.  This holds all the information about the current room.
// Notice that we added 2 new variables to our tRoom structure.  The first one is an
// instance of the CMonster class.  For this tutorial, we will only allow one monster
// per room.  You will want to make an array of CMonster's if you want more than one.
// The bMonsterInRoom boolean tells us if there is a monster in the room to attack.

struct tRoom
{										
	string strCurrentRoom;				// This stores the name of the current room we are in (I.E. "Hallway")
	string strRoomDescription;			// This holds the description of the current room			
	string strRoomNorth;				// This holds the name of the room that is to the north					
	string strRoomEast;					// This holds the name of the room that is to the north			
	string strRoomSouth;				// This holds the name of the room that is to the north				
	string strRoomWest;					// This holds the name of the room that is to the north			
	
	string strLookArray[MAX_LOOKS];		// This holds the names of the things to look at in the room				
	string strLookDescription;			// This holds the description of the thing we look at
	
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	CMonster monster;					// Our monster data for this room
	bool bMonsterInRoom;				// This tells us if we have a monster in the room

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

};


#endif
