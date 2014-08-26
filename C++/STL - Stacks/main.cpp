// Done by TheTutor

/*  
	This tutorial is going to use the STL (Standard Template Library) "stack" to do some simple
	things.  STL provides quite a few useful well-tested templates to use in your programs.
	Typically there is no need to reinvent the wheel, so STL is a great choice for using a stack
	in your program.

	To help you better understand the STL stack class, this tutorial is going to make some 
	sample stacks and call commonly used methods of the stack.  This tutorial doesn't use all
	the methods in the STL stack class, but it is a good first step to stacking up some knowledge... 
*/

#include <stack> // The header file for STL stack
#include <iostream>
using namespace std; // The namespace STL stack is contained within 

// Functions are are friends!
// A quick little function to print a number of successive blank lines
inline void BlankLines(unsigned int num)
{
	while(num--)
		cout << endl;
}

/*
	**NOTE** Quick template refresher.  When declaring an object a templated class, the 
	type of the object is declared between the <>'s.  Example:

	stack<int> s; // Creates a stack of type "int"
	stack<float> s; // Creates a stack of type "float"
*/

int main()
{
	stack<int> theFatStack; // Create the stack of integers

	// Method 1:  empty()
	// Our fat stack starts out not so fat.  empty() returns true if the stack 
	// is empty, false otherwise
	if(theFatStack.empty())
		cout << "Our stack is empty, how will we eat?" << endl; // We should see this message

	// Method 2:  push()
	// This method adds an element to the top of the stack.  Therefore, whatever 
	// is pushed onto the stack last, becomes the new top of the stack.
	// After the code executes below, theFatStack will look like this:
	// 4
	// 3
	// 2
	// 1
	// 0
	for(int i = 0; i < 5; i++)
		theFatStack.push(i);

	// Method 3:  size()
	// If you want people to notice ya, you need to carry a fat stack.  But how do 
	// we know how fat our stack is?  Well size() tells us how many elements are contained
	// in our stack
	cout << "Stack size = " << theFatStack.size();

	BlankLines(2);

	// Method 3:  top()
	// This method returns a reference to the top of our stack.  We know the top
	// element should equal 5, so lets verify that.
	cout << "Top of stack = " << theFatStack.top();

	BlankLines(1);

	// Lets change the top of the stack to 4000 instead of 4.  The stack should now look like:
	// 4000
	// 3
	// 2
	// 1
	// 0
	theFatStack.top() = 4000;
	cout << "Top of stack = " << theFatStack.top(); // Much fatter %)

	BlankLines(2);

	// Method 4:  pop()
	// The pop() removes the top element of the stack.  So for our data
	// set, if we called pop() once the stack would then look like this:
	// 3
	// 2
	// 1
	// 0

	// Lets loop through our stack, popping off each element in it one-by-one
	// until the stack is empty
	while(theFatStack.empty() == false) // While we are not empty
	{
		cout << "Top of stack = " << theFatStack.top(); // Print out the current top
		theFatStack.pop(); // Pop off the top element

		BlankLines(1); 
	}

	return 0; // And we're out :)
}

/*	Is she stacked or what?

	If you need a stack, STL is a solid way to go.  The STL stack class doesn't contain a ton of 
	methods making it fairly simple to use.  If you have any questions or comments
	be sure to post 'em on the forums at www.GameTutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/