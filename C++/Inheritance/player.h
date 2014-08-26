#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

// This will encompass attributes (variables, characteristics) that ALL players have
// for this tutorial.
class CPlayer
{
	public:

		// Our constructors
		CPlayer();
		CPlayer(const char *name, int life, int intellect, int strength); 

		// Data Access ***

			const char* getName() { return mName.c_str(); }
			int getLife() { return mLife; }
		
		// *** End Data Access

	private:

		string mName; // Player's name
	
		int mLife; // The amount of "life" the player has -- Range is 0 - 1000
		int mIntellect; // How "smart the player is -- Range is 0 - 100
		int mStrength; // How "strong" the player is -- Range is 0 - 100

		bool mAlive; // Is the player alive or dead?
};

/* 
	Okay, here is where the power of inheritance (deriving from classes) is.
	We are going to inherit the CWizard class from the CPlayer class, meaning certain aspects
	of the CPlayer class will exist in the CWizard class as if the code was cut 'n paste
	directly into the CWizard class.  Notice I said "certain aspects", depending on how we
	inherit, different aspects (member variables, methods) will be accessible to us in
	the CWizard class.

*/

// The single colon ':' signifies we are inheriting.  The "public" means that everything that
// is public to the CPlayer class will be public to the CWizard class.
class CWizard : public CPlayer
{
	public:

		// Constructors
		CWizard();
		CWizard(const char *name, int L, int I, int S, int SP); // L == life
																// I == intellect
																// S == strength
																// SP == spell power

		// Data Access ***
		
			int getSpellPower() { return mSpellPower; }
		
		// *** End Data Access

	private:

		// Let's notice that we don't need to declare any other variables other than
		// the spellPower variable.  Since we are inheriting, this class will already
		// have the variables in the CPlayer class.
		int mSpellPower; // The power of the wizard's spell -- Range 0 - 100

};

#endif
