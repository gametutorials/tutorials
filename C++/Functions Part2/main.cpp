// Done by TheTutor

/*  This tutorial is all about functions.  Functions are essential in
	programming.  You will create them or use them time and time again.  So for starters
	I'm going to go over the basic parts of a function:

	Example Function:

	int Add(int num1, int num2); // This entire line of code is referred to as a 
								// functions signature (function prototype) -- It defines
							   // what the function is going to return (return type), 
							  // the name of the function (in this case "add") and 
							 // the parameters to the function (what gets passed into
							// the function)

    Now just for emphasis I'll break this function into parts (starting at the left going
	to the right).

	"int" -- This is the return type (answers the question what variable is this function
			 going to return) -- Any "legal variable" can be a return type -- If you don't 
			 want your function to return anything, you can have your return type be "void".

    "Add" -- The name (label) of the function -- So in main() we can call this function
			 like this:  add(5,4); -- main() knows where to go to for the code for "add"
			 to execute it.

    int num1, int num2 -- These are the variables that the function takes -- A function can
						  take as many or as few variables as you want (even zero) -- Again
						  these variables can be any "legal variable" -- When listing the 
						  input parameters, separate them by a comma -- When writing a 
						  function signature (defining a function) you can name the variables
						  but you don't have to.  Also when implementing the function (writing
						  the code that does what the function is supposed to do)
						  you DO NOT have to use the same variable names as given in
						  the function's signature.

    Okay I think we're ready to do a simple function.

	How about a the "raise to a power of" function?  Sounds good to me :)
	Just to be sure we're on the same page here's an example in math:
	(NOTE: '^' means raise to)

	2^3 = 2 * 2 * 2 = 8

	3^3 = 27

	Okay now you KNOW what I'm talking about so here we go...
*/

#include <iostream>
using namespace std; // We can use "cout"

// Here we are DEFINING OUR FUNCTION PROTOTYPE -- It will return an integer (the resulting
// answer) -- It will take two integers, the first is the number being raised (base number),
// the second is the exponent (number we're raising the base number to).
int Power(int base, int exp);  // NOTE** negative exponents ARE NOT accounted for

// The "main" function (the core of your program)
int main()
{

	int result = 0; // A variable for holding the result of calling the power() function

	// "result" will equal 2^3 (remember '^' means "raise to the power of")
	result = Power(2,3);

	cout << "2 raised to the 3rd power = " << result << endl;

	// "result" will equal 5^3
	result = Power(5,3);

	cout << "5 raised to the 3rd power = " << result << endl;

	Power(4,4); // Now why the heck are we doing this???
			   // I did this just to demonstrate that you can call functions and 
			  // COMPLETELY IGNORE their return types (if you want) -- This call
			 // adds nothing beneficial to the overall program, but yet it is legal code.

	// Lastly, we know how to cout an int right (we're doing that a couple lines above),
	// so instead of storing the "answer" of power() in result, we can also do this:
	cout << "4 raised to the 6th power = " << Power(4,6) << endl;
		return 0;

} // end of main()

// Here we are IMPLEMENTING OUR FUNCTION -- Again the return type is the resulting answer
// of taking base and raising it to exp (exponent) -- It takes as parameters two integers
// (base and exp).
int Power(int base, int exp) // **Notice NO SEMI-COLON
{
	// The implementation of a function is held between the two curly brackets { and }

	int result = 1; // This is local variable -- IT IS ONLY KNOWN to the function
				   // power().  That means when we return from this function this variable
				  // will cease to exist.

	// While exp DOES NOT equal zero, subtract one from exp
	while(exp--)
		result *= base; // result = (result * base);

	return result; // Here we use the keyword "return" -- This is the keyword you use to
				   // return a "variable" from a function.

} // This curly bracket marks the END of the power() function

/* Rule of thumb:

	Use functions to break down you program into "steps".  For instance you could have a
	main() that looks like this:

	int main()
	{
		InitGame();

		while(GameNotOver() == true)
			RunGame();

		return 0;
	}

	Now these functions, for example InitGame(), could be broken down into even more functions.
	Such as:

	LoadLevel()
	LoadMonsters()
	LoadPlayers()
	etc, etc, etc.

	One last tidbit, you ALMOST ALWAYS want to create a function for ANY task in your program
	that is going to be repeated multiple times throughout the program.
	
	Well we at GameTutorials hope this clears up the concept of functions a little bit :)
	As usual, bring any questions you may have to www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
	

	
						


     
