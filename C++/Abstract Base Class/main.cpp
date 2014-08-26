// Done by TheTutor

/*	Okay, let's talk about Abstract Base Classes (commonly referred to as ABC's).

	Abstract Base Classes basically just serve as a "template" for creating polymorphic classes.
	This is very similar to an interface in Java or C#.  So you would use an ABC to mandate/design
	an interface for an object.  Now lets focus on the "base class" part of the name.  An ABC should
	hold the "base functionality" for whatever object you're trying to create.  Then you derive 
	objects (classes) from the ABC.  These derived classes would be forced to follow the layout of 
	your ABC.  In this example we're going to make a CShape abstract base class, then derive two 
	classes from it CTriangle and CSquare.  The CTriangle and CSquare class will have to follow the 
	conventions laid out by the CShape class or we wouldn't be able to create an instance of the
	CTriangle or CSquare class.

	Here's some basic rules of ABC's

	1)  You CAN NOT declare an instance of an ABC.  So for our example, CShape is an ABC  
	    (Abstract Base Class). I COULD NOT do the following:

		int main()
		{
			CShape shape; // I could not do this, won't compile

			return 0;
		}

	2)  You can have pointers to ABC's.  So for our example this would be legal code:

		int main()
		{
			CShape *s = new CTriangle;

			// rest of code goes here

		}

	3)  You can also have references to ABC's.  Again, for our example this is legal:

		int main()
		{
			CTriangle tri;

			CShape &s = tri;

			// rest of code goes here

		}

	4) Classes CAN derive from ABC's (hence this is where they are useful), but if a derived
	   class DOES NOT implement ALL of the ABC's pure virtual functions (more on these in
	   the header file) the derived class itself becomes an ABC.
*/

#include "Shape.h"

// Okay, we have our one ABC and two derived classes.  What we want to do is create a function
// that will draw ANY shape passed in.
void Draw(CShape &shape);

int main()
{
	// Here we make an instances of our "specific shape classes"
	CTriangle triangle;
	CSquare square;

	// **Note -- Every time we make an instance of any of CTriangle or CSquare,
	// that instance will have a hidden member variable called a vtable pointer (vptr).
	// This member variable will point to the class's vtable the object was created from.

	// Notice this function takes a CShape&, but remember CTriangle was derived from
	// CShape, so it is a "CShape".  Also notice WE DO NOT have to typecast.
	Draw(triangle); // This should draw a triangle to the screen

	Draw(square);   // This should draw a square to the screen

	// Again, this should draw a triangle to the screen
	triangle.draw();

	return EXIT_SUCCESS; // I feel good about this program ending :)

}


// Because we are using polymorphism, we derived all our "specific shape classes" (such as 
// CTriangle) from the CShape class.  The correct "draw" method will get called at run time
void Draw(CShape &shape)
{
	shape.draw();
}

/* You will notice that we take a reference to a CShape in the draw function.  Why not just a
   CShape?  Well, remember CShape is an ABC.  YOU CAN NOT create an instance of CShape (which would
   be done by the copy constructor if you had a function which took a CShape).  Thus
   we pass a reference to a CShape.  We could also pass a pointer to CShape.
*/

// If you have any more questions about ABC's, feel free to post 'em
// at www.GameTutorials.com.

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/
