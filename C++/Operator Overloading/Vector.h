#ifndef VECTOR_H
#define VECTOR_H

/* Operator Overloading 

	Well, operator overloading is pretty much just like it sounds.  It's a concept that 
	allows you to take established operators (+ for example) and have these operators work
	normally on user defined objects (classes/structs).

	So what we going to do is make a class called CVector -- This class will embody a vector
	just as it would be found in math.  If you don't know what a vector is ask 
	your math teacher :)  (It's basically a direction with length in 2D or 3D space).

	Then we're going to overload the assignment '=', the addition '+' and
	the subtraction '-' operators so that if we had:

	CVector aaa AND CVector bbb -- The following code would work:

	CVector ccc = aaa - bbb;

	We're also going to overload the "cout" operator, though it's a little different then you
	may expect.  If we have:
	
		CVector vec1;
	
	We want to be able to write

		cout << vec1 << endl;

	and have it print the value of "vec1".  So we're going to overload the '<<'.  It will be 
	a crazy experience you want to tell your friends about.

	Overloading operators is an essential part of an OOP diet.  So let us begin the first batch
	of comsumption.
*/

#include <fstream>
using namespace std;

// Our vector class
class CVector
{
	public:
		
		// Our constructor.  It just initializes our vector to zero
		CVector():x(0),y(0) { /* do nothing */ }

		// An overloaded constructor -- This sets our vector to the x_val and y_val passed in
		CVector(int X, int Y):x(X),y(Y) { /* do nothing */ }

		// This is our assignment operator -- For this example we could actually not
		// define one and we'd still be able to set two VECTORs equal to one another.
		// (Remember you ALWAYS get an overloaded assignment operator when defining a class).
		// But, we're going to define it ourselves for practice :)
		CVector& operator =(const CVector&);

		// This is our overloaded addition operator -- This will allow us to add two vectors
		// the same way we would add two integers in code.
		// example:
		// int a = 5; int b = 6;   ----- You'll now be able to also say CVector a(2,2);
		// int c = a + b;												CVector b(3,3);
		//																CVector c = a + b;
		CVector operator +(const CVector&);
		
		// This is our overloaded subtraction operator -- This will allow us to subtract two vectors
		// the same way we would subtract two integers in code.
		// example:
		// int a = 5; int b = 6;   ----- You'll now be able to also say CVector a(2,2);
		// int c = a - b;												CVector b(3,3);
		//																CVector c = a - b;
		CVector operator -(const CVector&);

		// Man, does that look ugly, but have faith, it's not that bad.
		// Here's a quick note on the keyword "friend".  If a method has
		// the "friend" keyword attached to it, it means the method is a "friend" of
		// the class and is not actually part of the class.  The benefit of being a friend
		// is that friends can touch the classes protected or private variables.  So just like
		// a friend with benefits, the friend key word allows the method to touch the classes'
		// privates :)
		
		// Now for the return value "ostream&".  Well, an ostream is defined to be an "output stream".
		// "cout" is an example of an output stream.  So, what basically is going to happen is this:
		// The output stream "cout" will get passed into the function when we write 
		// "cout << vec1 << endl;".  What will return is an output stream filled with the text of what
		// we want.  It may be a bit wordy but study the code and it will clear up like a bad case
		// of achene.
		friend ostream& operator <<(ostream&, const CVector&);

	private:

		int x; // X component of our vector
		int y; // Y component of our vector
};

#endif
