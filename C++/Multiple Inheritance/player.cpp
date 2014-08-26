#include "player.h"

// CPlayer's default constructor --
// Remember with this we can declare a CPlayer like this:
/*
	CPlayer player;

	Or like this:

	CPlayer player(1000);
*/
CPlayer::CPlayer(int max)
{
	mLife = max;
	mMaxLife = max;
}

// This method will "alter" (i.e. add or subtract to) the CPlayer's current life
void CPlayer::modifyLife(int amount)
{
	mLife += amount;
}

// This method will "alter" (i.e. add or subtract to) the CPlayer's max life
void CPlayer::modifyMaxLife(int amount)
{
	mMaxLife += amount;
}

// This method sets the CPlayer's name
void CPlayer::setName(const string &name)
{
	mName = name;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Implementation of the CSage /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// The default constructor -- Notice how we EXPLICITLY call the CPlayer constructor
CSage::CSage(int max, int mp, int str):CPlayer(max)
{
	mMagicSel = eFire; // By default we're going to say the CSage has "fire magic selected"

	// Set magic power and strength
	mMagicPower = mp;
	mStrength = str;

	mName.assign("Merlin"); // Now notice this -- Here we're assigning a default name for a CSage.
						   // We can DIRECTLY mess with "name" because it is a "protected"
						   // variable in CPlayer.
}

// This sets the "magic type" (i.e. the spell the CSage could cast)
void CSage::setMagicType(int type)
{
	mMagicSel = type;
}

// This is where we "figure out" the "magic level" (the strength of an attack)
int CSage::getMagicLevel()
{
	// Everyone knows your magic level is a number between 0 - mMagicPower + your 
	// strength
	return rand()%mMagicPower + mStrength;
}


//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Implementation of the CSwordsman /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// The default constructor -- Notice how we EXPLICITLY call the CPlayer constructor
CSwordsman::CSwordsman(int max, int str):CPlayer(max)
{
	mStrength = str;
	mName.assign("Conan"); // Now notice this -- Here we're assigning a default name for a CSwordsman.
						  // We can DIRECTLY mess with "name" because it is a "protected"
						  // variable in CPlayer.
}

// This returns the damage from a CSwordsman's attack
int CSwordsman::getSwordDamage()
{
	// This is the ultimate equation for SERIOUS sword damage :)
	return ((rand()%4 + 1) * -mStrength);

}
