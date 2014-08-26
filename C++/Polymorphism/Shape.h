#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
using namespace std;

/*	Lets keep these classes as SIMPLE as possible.  All we want to do is see
    how virtual functions work.
*/


/* Now as you'll recall if a class contains virtual function(s) it contains a vtable.
   This is a "graphical" representation of what the vtable looks like for the CShape class:

	vtable
		0 --> points to the draw method's implementation

	As you can see there is only one entry in the table, this is because there is only
	one virtual function in CShape.  If there were multiple virtual functions in the class, the
	first entry in the vtable, would be the first virtual function (defined from top to bottom) 
	in the class.
*/

class CShape
{
	public:

	// Here you see the key word "virtual".  This is what says "This is a virtual function".
	// Notice it goes BEFORE the return type.
	virtual void draw();

	// Now this draw function isn't going to do ANYTHING.  That's right, it's just going to return.
};

// About as simple as you can get for a CShape class.  Now where going to make two classes that 
// inherit from the CShape class.

// This class will "draw" an ASCII representation of a triangle
class CTriangle : public CShape
{
	public:

	// Again, we're just going to make a "draw" function
	void draw();
};

// Again, this class's ONLY method is "draw" which will draw an ASCII representation of a square.
class CSquare : public CShape	
{
	public:

	// We're just going to make a "draw" function
	void draw();

	/* To paraphrase what's happening slightly differently, here's what CSquare's vtable looks like:

		**********
		* 0	** | *
		*******|**
			   |--> points to CSquare's draw method implementation
	*/
};

// The "What If?" Section...

/*	1)  Question:  What if the derived class doesn't implement the virtual function?
	
			Example:

				class CBase
				{
					public:
						virtual void doSomething() { cout << "something" << endl };
				};

				class CDerived : public CBase
				{
				};

		Answer:  CBase's implementation will be called

	2)  Question:  What if the base class doesn't put the virtual keyword?

			Example:

				class CBase
				{
					public:
					void doSomething() { cout << "CBase is something" << endl };
				};

				class CDerived : public CBase
				{
					public:
					void doSomething() { cout << "CDerived is something" << endl };
				};

		Answer:  This is known as "functon hiding" and what will happend is CDerived
				 doSomething() will hide CBase's doSomething so in the following code:

				 CDerived d;
				 d.doSomething();

				 CDerived's doSomething would get called

	 3)  Question:  Do I have to write an implementation for every virtual function in
					the derived class?

		 Answer:  No.  Anything you don't implement will call the base classes' implementation.  This
				  will also work if you inheirtance chain is several classes long.
*/

#endif
