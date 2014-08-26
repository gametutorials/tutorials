// Done by TheTutor

/*	Okay, so what is a namespace?  Well, a namespace is basically just a way to
	"section off code so it doesn't conflict with other code".

	So what does that mean?  Well, we know in C++ land we can overload functions like this:

	int Add(int x, int y);
	float Add(float x, float y);

	Here these two functions have the same signature, but the compiler KNOWS they are different
	because they have different return types and take different data types as parameters.

	But lets say (for some reason) we want to do this:

	int Add(int x, int y);
	float Add(int x, int y);

	The above IS NOT legal syntax.  Two functions can NOT differ by only return type alone.
	But	if we use namespaces we can have these two functions co-exist.
*/

#include <iostream>
using namespace std; // As you can see, if you've been doing C++ coding, chances are you've
					// already been using namespaces for quite a while :)
				   // Most (if not all) of the functions in <iostream> (cout for example)
				  // are wrapped in the namespace std.  So by this line of code, what we are
				 // saying is "we want to be able to call these functions WITHOUT having to
				// directly access them through the namespace".
			   // So instead of having to do this:
			  /*
					std::cout << "Hello World" << endl;		// Yes this will work :)

					We can just do this:

					cout << "Hello World" << endl;
			 */

// Okay, here is our first "add function" -- As you can see this is just like normal
int Add(int x, int y)
{
	return x + y;
}

// Now we're declaring our "second add function", but we'll declare it inside of the namespace
// called NFloatFunctions -- To put something inside of a namespace simply type:
/*
	namespace THE_NAME_YOU_WANT_FOR_THE_NAMESPACE
	{

		// All functions, variables go in here

	}
*/
namespace NFloatFunctions // The name of our namespace
{

	// Here we have our add function (this is just like normal)
	float Add(int x, int y)
	{
		return float(x + y); // We typecast the answer to a float
	}

	int anInt; // Here we're declaring an int -- This is just here to show how we
			   // access it in main().
}

// Our program
int main()
{
	cout.setf(ios::showpoint); // Whoa!  What the heck is this?
							  // This is just a method of the cout object that allows you to 
							 // "alter how cout outputs" by passing in "flags".
							// The flag we passed in (ios::showpoint) makes it so cout will
						   // show the decimal point and trailing zeros for 
						  // floating-point values (i.e. float, double)

	// Your typical cout with a call to the add function
	cout << "Integer add of 2 + 3 = " << Add(2,3);
	
	// Now we're going to call our "float version" of the add function.
	// Notice that to get to that particular version, we first type the namespace (NFloatFunctions),
	// then the "scope resolution operator" (the double colons) -- Now we can access anything
	// that is contained within the scope (within the namespace) of NFloatFunctions so we can
	// call the "float add function".
	cout << endl << "Float add of 2 + 3 = " << NFloatFunctions::Add(2, 3);
	cout << endl; // Print a return for fun :)

	// Now, what if we wanted to get to the variable "anInt"???

	// It's the same syntax:
	NFloatFunctions::anInt = 55; // We've set "anInt" to 55

	// Lets prove it by printing it out
	cout << "Integer = " << NFloatFunctions::anInt << endl;	
		
	return 0; // And we are outta here
}

/*
  Hopefully the entire "namespace" concept makes a little more sense.
  Namespaces are just another way C++ allows you to label regions of code, so 
  hopefully NO functions or variables will ever step on one another.  When working
  with thousands and even millions of lines of code, you will see the importance
  of namespaces.

  Post your comments/questions on www.GameTutorials.com
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
