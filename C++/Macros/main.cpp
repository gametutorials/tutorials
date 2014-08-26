// Done by TheTutor

// This tutorial is all about "macros" -- As you may or may not recall, #define is 
// a preprocessor directive that does a textual substitution BEFORE your .cpp file(s)
// are compiled.  So for example:

/*
	#define NUM 36

	#include <iostream>
	using namespace std;
 
	int main()
	{
		cout << NUM;		// Here NUM will LITERALLY be replaced with 36
		return 0;
	}

*/

// Macros are very similar, expect they are a "function-like" replacement.  So lets do
// a quick example:

/*
	#define DOUBLE_SPACE() cout << endl << endl;
	  
	#include <iostream>
	using namespace std;

	int main()
	{
		cout << "HI";

		DOUBLE_SPACE()

		cout << "BYE";
		return 0;
	}

*/

// Okay now we will break down the example:

// #define DOUBLE_SPACE() cout << endl << endl; 
//	  |
//	  |__  The "keyword" to declare a macro
//
// #define DOUBLE_SPACE() cout << endl << endl;
//				|
//				|__ The name of our macro, just like you give a function a name, you also
//					give a macro a name.
//
// #define DOUBLE_SPACE() cout << endl << endl; 
//									|
//									|__ All of this: "cout << endl << endl;" is the 
//										macro implementation.  This textual string gets 
//										copied everywhere DOUBLE_SPACE() is found in the
//										.cpp file(s) BEFORE they get compiled.
//
// You can think of the example like this:
/*
	This is what you type in:

	#define DOUBLE_SPACE() cout << endl << endl;
	  
	#include <iostream>
	using namespace std;

	int main()
	{
		cout << "HI";

		DOUBLE_SPACE()

		cout << "BYE";
		return 0;
	}

	Then you hit compile -- What the compiler does is FIRST runs through your code and 
	changes everywhere it finds	"DOUBLE_SPACE()" to "cout << endl << endl;"

	Meaning your .cpp that ACTUALLY GETS COMPILED looks like this:
 
	#include <iostream>
	using namespace std;

	int main()
	{
		cout << "HI";

		cout << endl << endl;

		cout << "BYE";
		return 0;
	}
	
*/

// All right!  You are well on your way to becoming a "macro guru" :) -- In the code that 
// follows, we are going to implement three different types of macros -- The first macro will 
// just be the one used in the example, DOUBLE_SPACE() (just so you can see that it works :)
// The second will be a macro that takes an argument (so the macro is even more like a typical
// function) -- The last will be a macro that's definition is on multiple lines.  There's a
// special type of syntax required for macros that span more than one line of code.
//
// And now it's macro time

#define DOUBLE_SPACE() cout << endl << endl; // Hopefully this was pretty clear from the example
											// above -- Again everywhere in our code where
										   // "DOUBLE_SPACE()" is found, it will be replaced
										  // with "cout << endl << endl;".

// For the next macro we need to define PI
#define PI 3.141592653589

// Below is our macro that takes an argument -- It will convert degrees to radians.
// Notice how 'x' is NOT A VARIABLE (it's not an int, float, char, double, etc).
// This means you can pass in whatever you want, which is very handy.  The compiler should 
// puke at you if you pass in a variable where the '*' operation is NOT DEFINED.  However, with
// this flexibility you have to be careful.  One thing to definitely notice is that how 'x' is
// wrapped in ()'s in the implementation part of the macro.  It is IMPERATIVE that you wrap all
// arguments to a macro in ()'s in the implementation part.  Why?  Well it's pretty simple.  
// In main() we could do something like this:
//
// int main()
// {
//		float rad = DEG2RAD(45 + 90);
//		
//		cout << rad;
//		return 0;
// }
//
// This line:  float rad = DEG2RAD(45 + 90); would expand out to this:
//
// float rad = 45 + 90 * PI / 180.0f;  
//
// Now from recalling your math order of precedence rules, you can see very quickly that 
// this WILL NOT produce the correct answer.  The 90 * PI will execute FIRST, we want the
// 45 + 90 to execute first.  This is why it is a good idea to place ()'s around your arguments
// in the implementation part of a macro.
#define DEG2RAD(x) ((x) * PI / 180.0f)

// The last macro we will create is going to be called TRIPLE_SPACE() -- It will print
// out three blank lines -- Now we could do this macro on one line of code, but we're going
// to create it on 3 separate lines so you can see the special syntax needed to write 
// a macro on more than one line.
#define TRIPLE_SPACE() cout << endl \
					        << endl \
							<< endl;

// In the macro above the '\' means "this macro continues on the next line of code" -- Now
// if you're browsing through somebody else's header file(s) and you see some nasty syntax
// like this, you'll know what it means.  Again all the '\' does is continues the macro onto 
// the next line of source code.

// Okay now here's our main to actually use these three macros

#include <iostream>	// So we can use cout
using namespace std; // We want to use the STanDard namespace

int main()
{
//	***** MACRO 1 *****

	cout << "Hello";

	DOUBLE_SPACE()

	cout << "World";

	DOUBLE_SPACE()

//	***** MACRO 2 *****

	int deg = 90;

	cout << deg << " degrees = " << DEG2RAD(deg) << " radians";

//	***** MACRO 3 *****

	TRIPLE_SPACE()
		
	cout << "And we're out..." << endl;
		return 0;
}

// Macro Miscellaneous:

// For the most part, you should only create "one line" macros -- Macros ARE NOT functions,
// they are "textual substitutions".  So if you create very long elaborate macros that are called
// all over in the .cpp file(s) your executable is going to become HUGE!!!!!  Plus, macros
// are very difficult to debug.  This is another reason a macro should be very straightforward
// as to what it is doing so there is NO reason to debug it.

// If you have further questions be sure to head to the www.gametutorials.com forums

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/






