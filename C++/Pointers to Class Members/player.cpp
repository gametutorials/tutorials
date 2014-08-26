#include "player.h"

// Constructor -- Set everything to zero
CPlayer::CPlayer()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	mHealth = 0.0f;
}

// Sets the player's position to the (x,y,z) passed in
void CPlayer::setPos(float xPos, float yPos, float zPos)
{
	x = xPos;
	y = yPos;
	z = zPos;
}

// Sets the player's health to the value passed in
void CPlayer::setHealth(float hhh)
{
	// If "hhh" (health value) is NOT between 0 - 1, just ignore it
	if (hhh < 0.0f || hhh > 1.0f)
		return;

	mHealth = hhh; // Set the health value
}
