//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 1)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this file we define the basic functions for our CPlayer class.  Besides our
// standard constructor and deconstructor, we have Init(), Draw() and Move() functions
// for our player.  These are pretty simple and much of the code has been used in past 
// tutorials when dealing with character movement.
// 
// In the next tutorials we will add more functions that are specific to the game.
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
	// This function takes an image of the player and sets it's x,y position.
	// We also initialize the player with basic stats to start out with.

	// Set the players image and x,y position
	m_image = image;
	m_position.X = x;
	m_position.Y = y;

	// We set the players health and max health to the same.  We need 2 variables
	// for health because the player will be able to use items to replenish the
	// health, so we don't want to go past their max health stats.  As the player
	// levels up, this max health and all the other stats will increase as well.
	m_healthMax = m_health = 50;
	m_strength = 3;
	m_protection = 3;
	m_level = 1;
	m_gold = 25;
}


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
	// This is the function that actually changes the players position
	// on the map.  We can move the player up, down, left or right.
	// We check which direction is passed in and then move accordingly.
	// Before we move the player, we need to test collision detection.
	// We do this with IsACollision().  If the next position of the
	// player is on a solid tile, then we can't move there, so we don't.
	// That is our simple collision detection.  We return true if not a collision.
	// We save the last position of the character for party members who will follow us.

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
// This file defines the constructor, deconstructor, Init, Draw and Move
// functions for the start of our player class.  There is nothing new
// or interesting yet about this code when compared with previous tutorials.
// Take a look at Player.h for the class, then jump over to Map.cpp.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
