#ifndef DAMAGE_H
#define DAMAGE_H

// These will be the two types of "magic" available :)
enum EMagicDamage
{
	eFire,
	eIce,
};

// This class will determine how much damage a "player" will receive based on what 
// magic attack is cast against them.
class CMagicDamage
{
	public:

		// This takes the magic type and the magic level (how strong of magic) and
		// returns the amount of damage that occurs to a "player".
		int calcDamage(int magType, int magLevel);
};


#endif
