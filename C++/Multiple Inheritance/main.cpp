// Done by TheTutor

/*	Multiple inheritance... Well for starters, what is it?  Multiple inheritance is 
	just having a class that is derived from more than one class.  So if you understand
	"inheritance" you'll understand multiple inheritance just fine.

	If you don't understand "inheritance" and think that's something you get when your 
	great grandpa kicks the bucket, this tutorial isn't for you at the present time.  Read 
	up on inheritance and come back.

	So, to illustrate multiple inheritance we're going to the following 4 classes:

	CPlayer - The base class 
	CSage - A magic caster player, devrived from CPlayer
	CSwordsman - A barbarian player, dervied from CPlayer and CMagicDamage
	CMagicDamage - Defines how to doll out damage based on magic attacks

	Now it's time to bestow on you your multiple inheirtance...
*/

#include "player.h"

#include <iostream>
using namespace std;


/* Okay, this main doesn't do anything spectacular.  It just creates all three of the 
   classes (CPlayer, CSage, CSwordsman) and calls some of their methods printing out the results.
*/

int main()
{
	// Create a CPlayer, CSage, CSwordsman
	CPlayer player;
	CSage sage;
	CSwordsman swordsman;

	player.setName("Player"); // Set the player's name

	// Print out the player's, sage's and swordsman's name
	cout << player.getName() << endl;
	cout << sage.getName() << endl;
	cout << swordsman.getName() << endl;

	cout << endl; // A blank line for spacing

	// Have the sage execute a eFire attack on the swordsman.

	// First print out current life
	cout << "Swordsman current life = " << swordsman.getLife() << endl; 

	// Now here we're going to combine a few function calls :)
	// Does this monstrosity make sense???
	// We're passing into modifyLife() the result of calcDamage().  calcDamage() takes 
	// the attacker's (in this case the CSage) magic type (retrieved through getMagicType())
	// and the attacker's magic level (retrieved through getMagicLevel()).
	swordsman.modifyLife(swordsman.calcDamage(sage.getMagicType(), sage.getMagicLevel()));

	// Reprint out the current life
	cout << "Swordsman current life = " << swordsman.getLife() << endl;


	return 0; // Okay, we're out of here
}

/*

	Okay, it is important to note that these classes ARE IN NO WAY ROBUST.  The tutorial is 
	meant to accomplish two things:

	1)  To show how to "syntactically" achieve multiple inheritance

	2)  Explain the "protected" keyword

	When making your own programs you'll be responsible for coming up with a 
	programming schematic that solves your problems elegantly and efficiently :)

	As always, post your questions at www.GameTutorials.com.

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
