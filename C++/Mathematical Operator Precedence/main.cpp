/*	Done by TheTutor

  The mathematical order of precedence (operation precedence) is something any good programmer
  needs to know and know well.  So what exactly do we mean by "order of precedence"?

  Well here's an example:

  X = 5 - 16 / 4 = -11/4	If you solve this problem by going left to right -11/4 would
							be the answer

  However in C++ Land the answer is this:

  X = 5 - 16 / 4 = 1		Division will get carried out before the subtraction

  So lets list some precedence rules.  In main() there are "10 chunks of code" where "the order
  of precedence" will determine the final "answer".  It is suggested that you look at them
  first and figure them out on paper what will be printed.  To see what is printed simply change
  the #define SHOW_ANSWERS to 1 and recompile.

  But first, the precedence rules:

	1)  Mathematical operations "default order of precedence" is left to right.
		So for instance, when the following executes this is what happens internally:

		X = 5 + 10 + 15 - 30;

		First, 5 + 10 executes, the problem then becomes

		X = 15 + 15 - 30

		Next, 15 + 15 executes, the problem then becomes

		X = 30 - 30

		Lastly, 30 - 30 executes, and the net result has X equaling zero

	2)  Stuff in ()'s happens FIRST -- The innermost ()'s will get executed first
	
		Example:

		X = (5 - (4 - 3)) + 10 / 2

		So for this example (4 - 3) would be executed first and the result of this operation
		would be used to finish solving the equation.

	3)  Multiplication and Division occur before Addition and Subtraction
		
		X = 10 / 2 - 4 + 17 * 2

		10 / 2 will occur FIRST (remember the default behavior is left to right)
		that result will then be used to solve the rest of the equation.

	4)  Prefix operators will execute BEFORE any multiplication, division, 
		addition or subtraction operator.  Postfix operators will execute AFTER any
		multiplication, division, addition or subtraction operator.  In case you are not
		sure what the prefix and postfix operators are here is an example of both:

		++X	
		
		This is the prefix increment operator, it increases X's value by one
		
		X++	
		
		This is the postfix increment operator, it also increases X's value by one

		Here's a couple of examples:

		int X = 50;
		int Y = 50 + 50 - ++X;

		X would get incremented to 51 before anything else happens

		int X = 50;
		int Y = 50 + 50 - X++;

		X's value (50) would be used in the equation (resulting in Y equaling 50) THEN
		X would be incremented to 51.

  Okay, those are the 4 basic rules you need to know (yes there are more :)  When you get 
  into pointers and arrays these also follow an order of precedence.  Logical operators
  also follow an order of precedence, but for the time being we're just going to focus on 
  the "mathematical order of precedence" in C++ Land.

  It's time to see what ya know!  Grab your scrap paper and follow along below.
*/

#include <iostream>
using namespace std;

#define SHOW_ANSWERS 0 // If you want to see the answers printed to the screen 
					   // simply set this to 1, recompile and presto!

int main()
{
	int answer = 0; // This will hold our answer
	int x, y;       // A couple variables we'll use

	// If we're not showing answers, spit out a message to that effect and quit the program
	if(SHOW_ANSWERS == 0)
	{
		cout << "Not showing answers now.\n";
			return 0;
	}

//************************************************************** Number 1

	answer = 8 + 8 - 16 * 2;

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 2

	answer = (2 + (2 + 2 + (4 / 2 + 2) * 2) + 2);

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 3

	answer = 60 / 15 / 2 * 2;

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 4

	answer = 10 * 10 + 10 / 10 - 10;

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 5

	answer = 10 + 10 / 10 * 10 - 10;

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 6
//******************* They are trickier from here on out :) ****

	x = 22;
	answer = x + 3 / (++x + 2);

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 7
	
	x = 5;
	y = 5;
	answer = (x + y) + (++y + ++x) / 2;

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 8

	x = 10;
	answer = x + (x + x--) + (x + x++);  // The -- (postfix decrement operator) will 
									    // decrease x's value by one (in case you didn't know)

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 9

	x = 5;
	answer = (--x + 2) + (x-- + 2) * (x + 2) - (x + 2);

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;

//************************************************************** Number 10

	y = 3;
	x = 5;
	answer = --y - (x + y) - x / 2 + (--x + 5) - y;

	if(SHOW_ANSWERS)
		cout << "The answer equals " << answer << endl << endl;


	return 0; // Program is over -- How did you do?
			 // Be sure to look at the rules.  Look at the answers and be sure you know
			// how EVERY answer was arrived at :)
}

// The final word ---

/*
	For more on order of precedence you can check out
	MSDN at http://msdn.microsoft.com/library/default.asp

	There is another "rule" I'm going to talk about down here that if you read this first, it
	will probably help.  When we're in C++ Land we have to remember "integer truncation".
	What is that?  Well here is an example:

	int num = 2 / 15;	Well we know that 2/15 = 0.13333333333333 (calculator told me :)
					    But we're storing this into an "int" (integer) So num WILL ONLY
						EQUAL the integer part.  Thus after this operation "num" equals 0

	As always if you want/need more help let us know at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
