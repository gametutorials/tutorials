// Done by TheTutor
//
//
/*	This tutorial deals with the keyword "const".  Depending on where you use "const" it will
	mean different things in your program.  The five places you can use "const" in your 
	programs are:

	1)  When declaring a constant variable
	2)  When declaring pointer to constant data
	3)  When declaring a constant pointer
	4)  In an argument being passed to a function/method
	5)  When you declare a member function "const"

	If that seemed like gibberish in items 1 - 5, read on to see examples of each :)

*/


#include <iostream>
using namespace std;


const int num = 4; // This is example #1.  Here we are declaring num to be a CONSTANT int.
				   // That means, through the life of the program, num will ALWAYS equal 4.
				   // "num" CAN NEVER be modified anywhere in the program.  Attempting to do
				   // so will result in a compile time error.  This is a great alternative
				   // to using a #define.  The main benefit you get is that "num" will show
				   // up in the debugger, a #define will not.  Since we are in C++ land, if we
				   // wanted to we could define a const variable in a header file.  You can't 
				   // do that in C Land.

// Here's a string we're going to use for the next two examples
char a_string[64] = "Hello";

// The following is example #2
// Here we are declaring a POINTER TO CONSTANT DATA.  This works very similarly to example #1.
// For the life of the program the DATA that "buff1" points to CAN NOT BE MODIFIED.
// So this is ILLEGAL code:
//
//  buff1[2] = 'w'; // Illegal
// 
// So the contents of buff1 can not be changed, but we can set buff1 to point to another
// location in memory if we want.  I do this in main().  But if we were to reassign buff1, we 
// could ONLY reassign it to another const char*.  So if you have a pointer to constant data
// you can only reassign it to another pointer to constant data of the same type.  Man, 
// what a tongue twister :)
// 
// Why would you ever do this?  Well, if you had a string you didn't want somebody to 
// accidentally change, this is a great way to guarantee it won't get modified.
const char* buff1 = a_string;


// The following covers example #3
// First I decided to use a highly creative name "buff2".  Hopefully you can see the pure
// genius behind the name :) -- Anyway, what we are defining here is a CONSTANT POINTER.
// That means for the life of the program the POINTER will NEVER CHANGE.  So this would
// be ILLEGAL CODE:
//
// buff2 = "Hi there"; // Illegal
// 
// However, if we want to change the value(s) that are pointed to, we can do that.  I do
// this in main().
//
// Why would you want to do this?  Well if you want to always write to a SPECIFIC spot
// in memory, this gives you a way to do just that.  There are certain programming situations
// where you'll be writing data directly to memory.  This is a great way to keep track of
// the beginning/ending of where you want to write to.  Of all the ways to use "const", this
// is the one you'll most likely use the least.
char* const buff2 = a_string;

// To demonstrate the next place where you can use "const" we have to make 
// a class.  Now this class doesn't really do anything, we're just using it as an example.
class Point3
{
	public:

		// Default constructor
		Point3():x(0),y(0),z(0) { /* do nothing */ }

		// Set function
		void setPoint(int x_val, int y_val, int z_val);

		// This is example #5 (yes I know I went out of order :) -- This is what is referred
		// to as a "constant member function".  A constant member function has the following
		// properties:  It CAN NOT modify ANY variables inside of the class.  So if I tried to 
		// create this method:
		// 
		// void incX() const { x++; }
		//
		// I'd get a big old fat error message from the compiler.  Also, constant member functions
		// can ONLY call other constant member functions inside of their implementation.  So
		// why in the heck would you do this?  Simple, because the compiler KNOWS no data is
		// going to be changed inside of the method.  It can do some optimizing at compile time.
		// Plus, it makes it really clear to another programmer that this method doesn't modify
		// any internal class data. 
		int getX() const { return x; }
		
		// Now this is the EXACT SAME as the "getX()" method, I'm just putting the 
		// implementation below so you can see an example of that :)
		int getY() const;
		
		int getZ() const { return z; } // Yeah, same as getX() and getY()

	private:

		int x;
		int y;
		int z;

};

// Last but not least, you can pass a "const" pointer/reference to a function (example #4).
// By doing this you guarantee that whatever is passed in WILL NOT be modified
// by the function.  The main reason you pass by const reference (or pointer) is 
// so you can gain the speed by passing by reference (it's fast because no matter
// what the size of the struct/class you are passing in, it only has to pass the pointer which 
// is always 4 bytes of data).  You then know that your data won't get modified.  Whenever 
// you have a large struct or class you should always pass by reference/pointer (again for
// speed's sake).  The "const" really just prevents accidents :) -- Plus it tells fellow
// programmers, "Hey, I know for a fact this won't get modified in this function."
void printValues(const Point3 &ppp);

int main()
{
	buff1 = "Hi there"; // It's totally legal to set buff1 to another pointer to constant data
	
	*buff2 = 'w'; // It's totally legal to change any value pointed to by buff2
				  // In case you're a little fuzzy on dereferencing the following
				  // two statements are equivalent:
			      // *buff2 = 'w';		SAME AS		buff2[0] = 'w';


	// Create a point
	Point3 pt;

	pt.setPoint(5,6,7); // Set the point

	// Yep, the constant member functions work :)
	cout << "pt.x - pt.y = " << pt.getX() - pt.getY() << endl << endl;

	// Print the value of pt
	printValues(pt);
		
	return 0; // And we're done
}

////////////////////////////////////////////////////// Point3 Implementation
////////////////////////////////////////////////////

// Set function
void Point3::setPoint(int x_val, int y_val, int z_val)
{
	x = x_val;
	y = y_val;
	z = z_val;
}

// Here we are implementing example #5 (constant member function) outside of the
// class declaration.  Notice on the implementation part, you MUST have the keyword
// "const" as well.
int Point3::getY() const
{
	return y;
}

////////////////////////////////////////////////////// printValues() Implementation
////////////////////////////////////////////////////

void printValues(const Point3 &ppp)
{
	// Print out the values of the Point3

	cout << "X = " << ppp.getX() << endl;
	cout << "Y = " << ppp.getY() << endl;
	cout << "Z = " << ppp.getZ() << endl;

	// If we tried to call this inside of this function:
	//
	// ppp.setPoint(8,9,10);
	//
	// We'd get a compile time error.  When you pass an argument as a "const" to 
	// a function, you CAN NOT modify it inside of that function
}

// Constant Info
//
/*
	In general, use "const" if you want to make your data not modifiable.  Not
	only will this tell other programmers your intent, it will allow the compiler
	to do some optimization.

	If you have any questions be sure to post 'em at www.gametutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/


