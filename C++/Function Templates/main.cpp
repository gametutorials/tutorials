// Done by TheTutor

/*	Like function overloading, templates are one of the wonderful advantages C++ has over C .
	In C land lets say we wanted to write a function that took the average
	of two integers.  The function prototype would probably look something like this:
	
	double Average(int num1, int num2);	// Returns the average of num1 and num2

	Now lets say we wanted to do the average of two doubles.  We'd have to come up with a
	completely new name:

	double AverageDbl(double num1, double nun2); // Returns the average of num1 and num2

	//////////////////// Well in C++ Land, we can "templatize" functions (in our example,
						 the average function) so it will take "any data type" we pass in.

	template<typename type>
	float average(type num1, type num2);  // With this templatized function we can take the
										  // average of ANY data type (even classes and structs,
										  // as long as their copy constructors are correct and 
									      // their '+' and '/' operators are overloaded)

	So lets break down the syntax:

	template<typename type>
	   |
	   |__ This is the keyword "template", you use this to signify the start of a 
		   templatized function.

    template<typename type>
			      |
				  |__ The "typename" keyword simply says "the next term (in this case "type") is going
					  to be a generic data type that will represent any defined data type that we pass 
					  to the function.  For the hard core C users out there, this is very similar in
					  concept to a void pointer (void*).

	Let look at a slightly different example:

	template<typename var> float Average<var num1, var num2);

		Things to note from the code the line above:

		1)	The "template<>" syntax can be on the same line as the function declaration
		2)	The name after "typename" can be whatever you want.  The standard name used
			by C++ is "T".  So when you look at STL or other built in classes/functions 
			of the C++ language you'll see the name of the typename to be upper case "T".

	Whoa!  Hope that wasn't too confusing.  It's real easy.  After taking a look at this
	simple program, hopefully templates will make perfect sense :)

*/

#include <iostream>
using namespace std;

// Here we're making a template average function -- It will return a "double" and take
// two variables of type "type" (where a variable of type "type" means ANY VARIABLE TYPE)

template<typename type>
double Average(type num1, type num2);

int main()
{

	double answer = 0.0; // Variable for holding the answers from the average functions

	// Two "float" variable types to pass into our templatized average function
	// When declaring a type "float" you put the 'f' after it, otherwise the compiler (VC++)
	// assumes it is of type "double" (you'll probably get a warning).
	float x = 3.5f; 
	float y = 7.9f; 

	// Print out the average of two "ints"
	answer = Average(43, 24);
	cout << "The average of the integers 43 and 24 = " << answer << endl << endl;

	// Print out the average of two "doubles"
	answer = Average(23.2, 62.33);
	cout << "The average of the doubles 23.2 and 62.33 = " << answer << endl << endl;

	// Print out the average of two "floats"
	answer = Average(x, y);
	cout << "The average of the floats " << x << " and " << y << " = " 
		 << answer << endl << endl;

	return EXIT_SUCCESS; // EXIT_SUCCESS is just #defined to be 0 so you could
						 // replace EXIT_SUCCESS with zero if you wanted.
}


// Returns the average of two "items" (Again "type" can be any valid data type)
template<typename type>
double Average(type num1, type num2)
{
	return (num1 + num2) / 2.0; // We divide by 2.0 because we want to insure that
							    // "double division" takes place as opposed to "int division".
}

// Some more food severed up on your temp plate...

/*	
	When looking at other peoples code you may see templates defined like this:

	template<class T>
	double Average(T num1, T num2);

	class is also a way to define a template.  It is equivalent to typename.  The short history 
	lesson is that originally only the "class" keyword was defined.  But that became confusing so
	they introduced the keyword "typename".  You should probably use "typename" for all your code
	as the "class" convention over time will get phased out.

	Well that's templates.  Any questions, be sure to post 'em at www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
