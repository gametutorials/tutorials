// Done by TheTutor

/*	This tutorial is going to cover the "ternary operator".  For starters, it's called the
	"ternary operator" because it has three parts, but really what it is, is a conditional 
	operator that works JUST LIKE an "if statement".

	So the first thing we'll do is look at an actual piece of code that uses the ternary
	operator:

	(x == 200)?(x = 0):(x++);

	Whoa, that looks CRAZY right!  Well lets break it down into parts:

	(x == 200)?(x = 0):(x++);
	   |
	   |__ The (x == 200) part is just an expression for evaluating (it is also the first
		   part of the ternary operator).  It has the same meaning as the code was
		   writen like this:
		   
				if(x == 200)
					// Do stuff...

    (x == 200)?(x = 0):(x++);
	          |
			  |__ The '?' is literally like a question mark here.  It's posing the question:
				  "Does the expression (x == 200) evaluate to TRUE or FALSE?"

	(x == 200)?(x = 0):(x++);
				  |
				  |__ The (x = 0) part is what will be executed if x == 200 (if x is equal 
					  to 200).  This is the second part of the ternary operator and it 
					  ALWAYS gets executed if the first part (the expression part (x == 200)) 
					  is TRUE.  It NEVER gets executed if the expression part is FALSE.

	(x == 200)?(x = 0):(x++);
					  |
					  |__ The ':' (colon) is the basically saying "otherwise".
						  So what we are saying so far is this:
						  If (x == 200) is true then set (x = 0) OTHERWISE...

	(x == 200)?(x = 0):(x++);
					     |
						 |__ This is the 3rd part of the ternary operator.  This will 
							 ALWAYS is executed if the expression part (x == 200) is FALSE.
							 It NEVER gets executed if the expression part is true.

	 So, here is the format looking at it abstractly:  
		(conditional statement)?(execute this statement if TRUE):(otherwise, execute this statement)

	Okay that's it.  Not to bad once you look past the terse syntax

	So why would you use this instead of an if statement?  Well the answer is most times you
	probably won't.  Typically "if/else statements" are a lot clearer to read.  Plus,
	if you want to do exectue multiple staements based on a condition, for example:
		
		  if(x == 200)
		  {
			   x++;
			   cout << x << endl;
		  }

	You should use the "if/else statements"

	So back to when you should use the ternary operator.  There are some typcial occasions in
	code where ternary operators make life a little easier.  The first is if you are declaring a 
	variable and depending on a certain state in the program you want to set the variable's 
	initial value to one thing or another.  Another is when passing a variable to a function
	and depending on the state of the program, you wnat to pass one thing or another (very
	similar to the first occasion).  And lastly when defining macros, ternary operators 
	can be helpful. 
*/

#include <iostream>
using namespace std;

// Here are two macros that use ternary operators
#define MAX(x,y) ((x) > (y)) ? (x) : (y)
#define MIN(x,y) ((x) < (y)) ? (x) : (y)

int main()
{
	int level; // This will hold the level of difficulty for our pretend game

	cout << "Enter difficulty level (0 - 9): ";
	cin >> level; // Read in the "level of difficulty"

	// Error check to make sure they typed in a number between (and including) 0-9
	if(level < 0 || level > 9)
	{
		cout << "You didn't follow directions did you?!?" << endl;
		return 0; // Quit the program
	}

	cout << endl << endl; // Print a couple of blank lines

	// Okay here where we going to use the ternary operator
	int aiStr = (level < 5)?(level * 5):(10); // So what we saying is this:
											  // Create the variable "aiStr".  If "level"
											 // is less than 5, set aiStr to equal (level * 5),
										    // otherwise, set aiStr to equal 10;

	// Here we use the MAX macro
	int aiHitPts = MIN(level * 100, 500);

	// We'll print the variables to the screen
	cout << "AI Strengh = " << aiStr << endl;
	cout << "AI Hits Point = " << aiHitPts << endl;
	
	return 0; // And we're done
}

// To summarize tersely...
// (Questions >= 1) ? (goto www.GameTutorials.com) : (goto kNextTutorial)

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2006 GameTutorials |
\*-------------------------*/
