#ifndef PLAYER_H
#define PLAYER_H

/*	
	Okay, here is a VERY basic player class.  This really doesn't do anything, but we're going
	to use it to demonstrate using "pointers to members".
*/

// Our basic player class.
class CPlayer
{
	public:

		CPlayer(); // Constructor

		void setPos(float xPos, float yPos, float zPos);
		void setHealth(float h);

		// Data Access Functions ***

		float getX() { return x; }
		float getY() { return y; }
		float getZ() { return z; }

		// *** End of Data Access Functions

		// **NOTE** See here how we have "health" as a public member variable
		//			This is because you can ONLY have "pointers to members" if the members
		//			are public to the class.

		float mHealth; // Player's health stored as a percentage from 0.0 - 1.0

	private:

		float x; // X-pos in world
		float y; // Y-pos in world
		float z; // Z-pos in world
};


#endif
