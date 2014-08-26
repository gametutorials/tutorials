#include <windows.h>								// Include our standard files
#include <iostream>									// This line of code is necessary to include all the C++ standard functions\classes definitions
using namespace std;								// Set our namespace to standard 

#define NORTH 0										// Here we define NORTH as being the number 0;
#define SOUTH 1										// We will use these defines to assign and check the players direction
#define EAST  2										// The numbers aren't important, we just need each of the directions to 
#define WEST  3										// Have different numbers.  If you are confused why we do this, look at how we use them in the ".cpp" file.
													
#define SCREEN_WIDTH  79							// Here is some defines for the screen width and height.  To make our code more readable.
#define SCREEN_HEIGHT 24							// There is allowed 80 characters across in a general DOS window, unless you personally change it.
													// Since there are 80, and the top left corner is (0,0) in coordinates... we need to subtract one
													// From the width and the height giving us (79, 24) as the bottom right corner of the screen. 
													// We wouldn't have to do that if the top left corner started at (1, 1).
													// All coordinate systems that you will use will usually always start with (0,0).
													// The normal DOS/CONSOLE window is 25 characters long.  So subtracting 1 we get 24.

#define ERASE 0										// These defines are used to pass into to DrawPlayer() as a flag to say if you want to ERASE..
#define DRAW  1										// Or DRAW the player to the screen.  This makes it so we don't need to make 2 functions for drawing and erasing.

													// Here is our OWN structure that we made.
													// We want to keep track of player data, like a position and a current direction.
													// So we create a structure to hold all of that information.
													// We could just create a COORD and a "int" separately, but when our
													// Player structure gets a lot bigger, say like, magic, health, armor, weakness, poison, gold, etc..
													// We don't want to have to keep track of all of that stuff, and more importantly
													// We can just pass in ONE variable into a function that holds ALL the player data, instead of 20 :)
													// If you don't feel comfortable with structures after this tutorial, you can stick
													// To just using variables without structures, but it's good to start early :)									
																										
struct PLAYER {										// Here we create the player structure.  Here we put the "{" after the "PLAYER" ... It doesn't matter, just a preference.  It could be on a new line.
	COORD	Position;								// We want a COORD and an "int" in our structure, so let's put them in.
	int		Direction;								// The direction the player is going.
};													// Then we close the structure with a "};"

void DrawPlayer(PLAYER Player, int Draw);			// Here we declare our function to draw the player.  You'll notice that we have a PLAYER structure to pass in, as well as a flag.
													// If we didn't have a player structure, our function would look like this:	
													// "void DrawPlayer(COORD Position, int Direction, int Draw);"
													// And you can imagine if your player has 20 variables it could and does get ugly.  Even though we don't need the direction in this function..
													// You get the point if we did...
													// One other point, you don't need to call the variable "Player" ... you could call it "PLAYER ChickenLegs" or what ever you want.
													// The name "ChickenLegs" is only used INSIDE of the DrawPlayer() Function.  It's local, main() never sees it.
													
													// Now comes the crazy part.  REFERENCES!!!!  NO!!!
													// See the "&" in front of the "Player".  This means the function gets a REFERENCE to the variable
													
													// I called it a permission thing... In a way it is, but what it really is, is giving the function
													// The memory address of the variable being passed inside. If you pass something into a function
													// It makes it's own copy of the variable.  By giving the "reference/address" of the variable with the "&" character...
													// The function just uses the memory address of the variable, instead of creating a new variable.
													// So essentially, If the variable is changed inside the function, it is also changed outside the function.

void MovePlayer(PLAYER &Player);					// This function will be for checking the input from the user.

// EXAMPLES WITH STRUCTURES:						// Does this make much sense?  So now in the code we would do things like this:




// PLAYER Player;									// Here we define the player structure.  We could call is PlayerData, or whatever...  I usually call it the lowercase type with first letter capitalized.

// Player.Direction = NORTH;						// Here we just set the player's direction to NORTH
// Player.Position.X = 5;							// Here we set the Player's position X to 5.  See?  just put a period after the structure and the variable inside the structure.

// You CANT! do this:  Player.Position = 5;			// This WONT work.
													// That is saying you want the structure to equal 5.  That doesn't make sense.
													// Here is some other ways to add and subtract with structures, same thing as normal variables...
// Player.Position.X++;								// Add one to the players X position.
// Player.Position.Y--;								// Minus one from the players Y position.

// Player.Position.X += 5;							// These 2 are the EXACT same, just a easier way to do it.
// Player.Position.X = Player.Position.X + 5;		// These both add 5 to the players X position.

													// The "+=" makes it so you don't have to type "Player.Position.X" again...
													// There is "-=" "/=" "*="  which is minus, divide, and times.
													// Use what ever you feel comfortable with.

													// Structures are the same as variables in every respect... Look Below:
//  if(Player.Position.X > 20)						// You can compare the same .. it's just a normal variable, but you have to put the parents on top :)
//	{												// Think of the other stuff as a leash :)  You don't want the dog to get away! :)  ok, so it's corny...
//		Do Stuff...									// Think of them as a family tree, or any hierarchy..   Starting to make sense now?
//	}


// EXAMPLES WITH POINTERS							// Here is an example of why we need pointers

// int num = 2;										// Lets create a test variable.

// ChangeNumber(num);								// We pass the variable into the function below.

// void ChangeNumber(int number)					// Since we didn't put a * in front of "number", a local copy of the variable passed in is created.
// {
//		number = number + 5							// So now the local copy now gets 5 added to it, but not the number passed in (which was num).
// }												
													// "num" still equals 2 because we didn't use a pointer.

////////////////////////////// *Now With a reference* ////////////////////////////

// int num = 2;										// Lets create a test variable.

// ChangeNumber(num);								// We pass the variable into the function													

// void ChangeNumber(int &number)					// Since we put a & in front of "number", a local copy isn't made, but we are using the variable "num" now.
// {
//		number = number + 5;						// So now the variable we passed in "num", equal what?  7!  That's right! :)
// }

// Does this make a little more sense?  If not, I have a good tutorial using references in very simple to understand way.
// Basically, we use references in functions if we want the variable we are passing in to change.  Other times
// We just want the information the variable holds, so we don't NEED a pointer to it.
// Like in DrawPlayer(), we just want it's location, we aren't changing the player's location.
// But in MovePlayer() , we are changing the player's location so we need to use references.
