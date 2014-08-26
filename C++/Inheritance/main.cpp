// Done by TheTutor

/*	Inheritance.  It is something you receive when your grandpa kicks the bucket and is one of the
	three programming staples of an object oriented programming (OOP) language.  Inheritance occurs
	when you declare a class that "inherits" the data and functions from another class.  The process
	of inheriting is typically referred to "deriving from a class".  On top of all that, the way you
	inherit (public,  protected, or private) determines what stuff the receiver of the inheritance
	gets.

	A lot to absorb right!?!  Well to ease this process, this tutorial creates two classes:

	CPlayer:  This class will encompass all the basic functionality of ALL players.
	CWizard:  This class will be a specific type of player, the wizard.

	You must now head to player.cpp to receive the rest of your inheirtance...
*/

#include <iostream>
#include "player.h"

using namespace std;

/* 
	For the main we're going to create a CWizard and print out it's name, life, 
    and spell power.  That's it :)
*/

int main()
{

	// Instantiate our wizard
	CWizard wizard("Merlin",50,40,30,70);
		
	const char *wizardName = wizard.getName(); // Get the name
	int life = wizard.getLife(); // Get the life
	int spellPower = wizard.getSpellPower(); // Get the spell power

	// Print it
	cout << wizardName << "'s life = " << life << "." << endl;
	cout << wizardName << "'s spell power = " << spellPower << "." << endl << endl;
	
	return 0;
}

//	Inherit this...

/*	
	Obviously these two classes are very limited (and not too useful in their current state),
	but nevertheless, the CWizard class uses inheritance.

	Inheritance is VERY powerful.  Used correctly, it can limit the amount of code you have to
	write AND make it reusable.  I can think of TONS of instances when inheritance would be 
	extremely useful.  For example, if you were making a RTS game that was comprised of many 
	different vehicles you could make a "base vehicle" (this would correspond to CPlayer).
	From that you could derive tanks, planes, boats, skateboards, whatever.

	The thing you want to remember if your going to use inheritance is to keep the BASE class
	(this refers to the thing other classes derive from) as GENERAL as possible.  Don't put 
	things in your base class that can't be used by all classes that derive from it.

	As usual, post your questions at www.GameTutoirals.com message board.
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
