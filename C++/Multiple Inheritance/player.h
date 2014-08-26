#ifndef PLAYER_H
#define PLAYER_H

#include "damage.h" // Include our "damage class"

#include <string>
using namespace std;

// Okay, here is our CPlayer class that will have all the "base functionality" of all "players".
class CPlayer
{
	public:

		// Our default constructor --  Because we are giving a default value for ALL the 
		// parameters passed into the constructor (i.e. the only one passed in) this will
		// work as a default constructor.
		CPlayer(int max = 500);

		void modifyLife(int amount); // Modify current life by "amount"
		void modifyMaxLife(int amount); // Modify max life by "amount"

		void setName(const string &name); // Set's the CPlayer's name to "player_name"

		// *** Data Access

		string getName() { return mName; }
		int getLife() { return mLife; }
		int getMaxLife() { return mMaxLife; }

		// End of Data Access ***


	/*  This is the "protected" key word.  It works much the same way as the
	    "private" key word.  Just like when you use "private", client .cpp files
		(such as main.cpp) CAN NOT access protected variables/methods.  Their difference
		is that derived classes CAN access protected variables/methods (they can't access private).
	*/
	protected:

		string mName; // The CPlayer's name

		int mLife; // The CPlayer's current life
		int mMaxLife; // The CPlayer's max life
};


// This is our "Sage" player
class CSage : public CPlayer		// The single colon ':' signifies we are inheriting -- 
{									// The "public" means that everything that is public to
									// the CPlayer class will be public to the CSage class.
	
	public:

		// Here we'll make our default constructor.  Again, we'll provide default arguments so
		// if the client chooses, they don't have to pass in anything when creating a CSage.
		// The CSage takes "max" (the max life), "mp" (magic power) and "str" strength.
		CSage(int max = 100, int mp = 50, int str = 10);

		void setMagicType(int type); // Set "mMagicSel" to whatever magic the CSage is
								     // currently using.

		int getMagicLevel(); // This returns the "level" of the current magic attack

		// *** Data Access

		int getMagicType() { return mMagicSel; }
		
		// End of Data Access ***

	private:

		int mMagicSel; // Which magic is the CSage currently "using" (have selected to use)

		int mMagicPower; // How much magic power the CSage has
		int mStrength;    // How much strength the CSage has

};


// Okay, last but not least, our "Swordsman" player -- It will be derived from the
// CPlayer and CMagicDamage classes, hence "multiple inheritance" will occur.

class CSwordsman : public CPlayer, public CMagicDamage	// So here we say we're publicly deriving
{											        // from CPlayer and CMagicDamage.  You use
											 	    // a comma to separate the classes you're 
										 		    // deriving from.

	public:

		// Our default constructor:
		// "max" is the max life for the CSwordsman, "str" is the CSwordsman's strength
		CSwordsman(int max = 800, int str = 40);

		// This returns the amount of damage a CSwordsman's attack causes
		int getSwordDamage(); 

	private:

		int mStrength; // How much strength the CSwordsman has

};


#endif
