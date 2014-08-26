#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
using namespace std;

/* Facts about pure virtual functions:

	1)  They have the keyword "virtual" before the function
	2)  They are followed by "=0"
	3)  They have NO implementation
	4)  Any class that contains a pure virtual function, makes that class a
		Abstract Base Class

*/

class CShape
{
	public:

	// Here you see the key word "virtual".  This is what says "This is a virtual function".
	// Notice it goes BEFORE the return type.
	// Also notice the "=0".  This says the method is a pure virtual function.
	// You CAN ONLY use the "=0" (pure specifier) in conjunction with virtual functions.
	virtual void draw()=0;

};

// Pretty simple CShape class.  Now we're going to make two classes that INHERIT from the 
// CShape class publicly.

// This class will "draw" a ASCII representation of a triangle
class CTriangle : public CShape
{
	public:

	// Again, were just going to make a "draw" function
	void draw();

	/* Here's what CTriangle's vtable looks like:

		vtable: 1 entry

		[0] -> points to CTriangle's draw method implementation

	*/

};

// Again this class's ONLY method is "draw", which will 
// draw an ASCII representation of a square.
class CSquare : public CShape	
{
	public:

	// Again, we're just going to make a "draw" function
	void draw();

	/* Here's what CSquare's vtable looks like:

		**********
		* 0	** | *
		*******|**
			   |--> points to CSquare's draw method implementation

	*/
};

#endif
