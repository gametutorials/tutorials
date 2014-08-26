// Done by TheTutor

/*	Polymorphism is the ability for determining which function/method to use at run-time.

	Normally when you compile a program the functions/methods have a STATIC memory location
	they will be called from.  For example:

	void PrintName(char*);

	So basically what is happening when you compile your program is that the compiler replaces
	"PrintName" with some address in memory that points to the location in memory where the 
	functions code is.  Make sense?   This address DOES NOT change for the entire
	life of the program.

	This is where polymorphism differs.  First off, when your thinking about polymorphism, you
	should think virtual functions.  C++ uses virtual functions we can achieve polymorphism.

	A virtual function lets us decide while the program is executing which "version" of particular
	function/method we are going to use.  It's still probably not making sense yet and that is okay.
	To start clearing things up, lets take a look at some rules for virtual functions:

	1)	They must be class methods
	2)	They CAN NOT be stand alone functions
	3)	They CAN NOT be class data
	4)	They CAN NOT be static methods
	5)	When a class has virtual function(s), that class will get a vtable.  This vtable
		exists for the class.  The vtable is in charge of calling the correct version of the
		function depending on what inheirted class is calling the function.  This concept should
		get clearer as you read through the tutorial.

	This program is going to make three classes:  CShape, CTriangle and CSquare.
	Using virtual functions we'll be able to write a function that takes a CShape and draws
	out the appropriate shape (i.e. a triangle or square).

	So let's get moving, there is plenty more green stuff to read through in Shape.h :)
*/

#include "Shape.h"

// Okay, we have our three SIMPLE classes and what we want to do is create a function
// that will draw ANY shape passed in.
void Draw(CShape *shape);

int main()
{
	// Here we make an instance of our "specific shape classes"
	CTriangle triangle;
	CSquare square;

	// Every time we make an instance of any of the classes we created,
	// the object we create (i.e. "triangle") will have a hidden member variable
	// called a vtable (vptr).  This member variable keeps track of what virtual 
	// functions to invoke for the instance of the object.

	// Notice this function takes a CShape*, but remember CTriangle was derived from
	// CShape so it is also of type "CShape".  Also notice WE DO NOT have to typecast.
	Draw(&triangle); // This should draw a triangle to the screen

	// Same thing for CSquare.  It was derived from CShpae so it also a type of CShape.
	Draw(&square); // This should draw a square to the screen

	// Again, this should draw a triangle to the screen
	triangle.draw();

	return EXIT_SUCCESS; // I feel good about this program ending :)
}

// Because we are using polymorphism we derive all our "specific shape classes" (such as 
// CTriangle) from the CShape class.  The correct "draw" method will get called at run time.
void Draw(CShape *shape)
{
	shape->draw(); // Because of polymorphism the correct "draw" method will get called 
}

// It's not polytheism...

// Polymorphism is one of the three paradigms of object oriented programming (OOP).  It is 
// a used extensively by languages such as Java or C# for their interfaces.  Polymorphism is a
// great tool to encompass a general grouping of functionality, then have it overloaded for
// specific instances inside of that group.  Hopefully this tutorial showcased this concept.

// If you have any more questions about polymorphism, please post 'em at www.GameTutorials.com
// and the GameTutorials community will do it's best to answer 'em :)

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
