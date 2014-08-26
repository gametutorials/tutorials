// Done by TheTutor

/*
	All righty!  This tutorial is about the very, very interesting C++ syntax known as
	"Pointers to Class Members" (in shorthand form "pointers to members").  So what exactly
	is a pointer to a class member?  Well, first we better define what a "class member" is.
	A class member is any variable or function that belongs to the scope of a class.
	Check out this example:

	class CPlayer
	{								// **This starts the scope for class CPlayer**
		public:

			void setPos(float xPos, float yPos, float zPos); // This is a member function 

			float x; // This is a member variable
			float y; // This is a member variable 
			float z; // This is a member variable

	};								// **This ends the scope for class CPlayer**

	So... A pointer to a class member is a pointer to either a class's member variable(s)
	OR a pointer to a class's member function(s).  Sounds confusing?!?  Well, wait till 
	you see the syntax %)

	What we are going to do is create a VERY basic player class, then using it we'll show
	examples of creating a pointer to a member variable and a pointer to a member function.

	So without further ado...	
*/

#include "player.h"
#include <iostream> 
using namespace std;

// This function will print a player's stats (the player's position and health) to the screen
void PrintStats(CPlayer &player);

int main()
{
	CPlayer player; // Here's our player class

	// We'll set some "defaults" for our player.  First we'll place 'em at (50,50,50) in the 
	// imaginary world and set his health to 100% (aka 1.0).
	player.setPos(50.0f,50.0f,50.0f);
	player.setHealth(1.0f);

	// Let's print the player's stats
	PrintStats(player);
	cout << endl; // Skip a line

	// Okay, let the CRAZY syntax begin.  First we'll JUST declare a 
	// pointer to a member variable.
	float CPlayer::*healthPtr; // Declaring a pointer to a class's member variable

	// Syntax explanation
	/*
		float CPlayer::*healthPtr;
			    |
				|
				--  This part "CPlayer::" says which class we're going to use
					when we declare a "pointer to a member".

		float CPlayer::*healthPtr;
						 |
						 |
						 -- This is the variable name we are arbitrarily choosing.
							So "healthPtr" is the variable name we're going to use
							to hold our pointer to a member variable.
	*/

	// Now we're going to initialize it.  As you may have guessed we're going
	// to initialize it to the player's health.

	healthPtr = &CPlayer::mHealth; // Initialize "healthPtr" to point to the class CPlayer's
							     // health member variable.
								    
	// Syntax explanation
	/*
		healthPtr = &CPlayer::mHealth;
					|
					|
					-- We're making a "pointer to a member variable" so we need 
					   to use the '&' to say "give me the address of" (typical pointer stuff %).

		healthPtr = &CPlayer::mHealth;
						   |
						   |
						   -- We then specify the member variable we want to declare a
							  pointer too by using the class name (in our case CPlayer), then the
							  :: (scope resolution operator), then the name of the variable
							  we want a pointer to (in our case "mHealth").
	*/

	// Last but not least we're going to use the pointer we created to set the 
	// player's health to 50% (aka .5).

	player.*healthPtr = .5; // Set the player's health to 50%

	// Syntax explanation
	/*
		player.*healthPtr = .5;
			  |
			  |
			  -- The ".*" is called a "pointer to member operator".  We need to
				 use this to access/modify our pointer we created.
	*/

	// Now lets prove all of this worked by reprinting out our player's stats

	PrintStats(player); // Print out player's stats
	cout << endl;       // Skip line

	// Well the fun is half over.  Now that you've seen crazy enough syntax to last ya a
	// year, we'll add some more equally strange syntax to the mix %)

	CPlayer johnny; // Here's our new player johnny

	johnny.*healthPtr = .25f; // Well set his health using our "pointer to a member variable"

	// Now we'll declare a "pointer to a member function"

	void (CPlayer::*setPosition)(float, float, float); // Declaring a pointer to a class's
													  // member function.

	// Syntax explanation
	/*
		void (CPlayer::*setPosition)(float, float, float);
		 |
		 |
		 -- The return type of the function.  This works the same as a normal function declaration

		void (CPlayer::*setPosition)(float, float, float);
					   |
					   |
					   -- This part (CPlayer::*setPosition) specifies two things.  First
						  the "CPlayer::" answers, "what class are we going to use to make
						  a pointer to a class member function with?".  The second part
						  "*setPosition" is the arbitrary function name we are assigning
						  to our pointer to a member function.  Also notice how the entire
						  thing is wrapped in ()'s.  The function pointer has to be 
						  declared like this.

		void (CPlayer::*setPosition)(float, float, float);
										    |
											|
											-- The final part of the declaration is
											   (float, float, float).  This specifies the
											   parameter list that will get passed to
											   our function we're declaring.  This works
											   just like a normal function declaration.

	*/

	// Next up, we'll initialize the pointer to a member function.  We're going to 
	// initialize it to the "setPos" member function.

	setPosition = &CPlayer::setPos; // Initialize our pointer to a member function

	// Syntax explanation
	/*
		setPosition = &CPlayer::setPos;
					  |
					  |
					  -- We're making a "pointer to a member function" so we need to use 
						 the '&' to say "give me the address of" (typical pointer stuff %).

		setPosition = &CPlayer::setPos;
						   |
						   |
						   -- We then specify the member function we want to declare a
							  pointer to by using the class name (in our case CPlayer), then the
							  :: (scope resolution operator), then the name of the function
							  we want a pointer to (in our case "setPos")
	*/

	// Last but not least, we'll set johnny's position by using our newly declared
	// "pointer to a member function".

	(johnny.*setPosition)(25.0f,25.0f,25.0f); // Set johnny's position

	// Syntax explanation
	/*
		(johnny.*setPosition)(25.0f,25.0f,25.0f);
			   |
			   |
			   -- Again we'll use the .* (pointer to member operator) to access the 
				  pointer to a member function we have created.  Notice how we 
				  must wrap the function name in ()'s

		(johnny.*setPosition)(25.0f,25.0f,25.0f);
									 |
									 |
									 -- The (25.0f,25.0f,25.0f) is the list of arguments
										we are passing to the function.  This works identically
										to normal function argument lists.
	*/

	// To show that this all worked we'll print out johnny's stats:

	PrintStats(johnny); // Print out johnny's stats
	cout << endl;       // Skip line
	
	return 0; // And we're out
}

// This function prints a player's position and health to the screen all nicely formatted
void PrintStats(CPlayer &player)
{
	cout << "Player's position = (" 
		 << player.getX() << ", "
		 << player.getY() << ", "
		 << player.getZ() << ")" << endl;
	cout << "Player's health = " << player.mHealth << endl;
}

/*
	Member Appointment...

	Now that you've added some wicked looking syntax to your bag of tricks, you are
	probably wondering, "When in the heck am I going to use this?".  Well the answer is,
	"Could be never."  You certainly do NOT have to use "pointers to members" to make
	a ballz-2-the-wallz game.  This is an ADVANCED C++ feature that should only be
	used in cases where prudent.

	So for a prudent example... Lets suppose we have a base class CObject that
	comprises all "3D objects" in our 3D world.  It has a render() function
	which draws a wire-frame rendition of the object.  Now we also have a CPlayer 
	class which we've derived from our CObject class.  The CPlayer class also has
	a render function which draws the player fully textured map and all that good stuff.
	Since both classes have a render function, CPlayer's render() will "function
	hide" (make it so we can't directly call) CObject's render(), but we still want
	to call CObject's render()!!!!  Pointers to member functions to the	rescue!  If we
	make a pointer to CObject's render() for the CPlayer, we can call both anywhere as
	we see fit.

	If you have any further quandaries about the wackiness know as "pointers to members", post
	a message at www.gametutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
