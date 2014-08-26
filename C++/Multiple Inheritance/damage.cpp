#include "damage.h"

// This method calculates the amount of damage based on the
// type of magic and the magic level
int CMagicDamage::calcDamage(int magType, int magLevel)
{
	switch(magType)
	{		
		// If it fires magic, return negative 2 times the magic level
		case eFire:

			return -2 * magLevel;

		// If it's ice magic, return negative 3 times the magic level
		case eIce:

			return -3 * magLevel;

		// Otherwise the magic has no effect, so return 0
		default:
			return 0;
	}
}
