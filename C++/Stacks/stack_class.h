#ifndef STACK_CLASS_H
#define STACK_CLASS_H

#include <iostream>
using namespace std;

/*
	Below is the stack class in all it's glory.  Our aim is to make a simple, fairly robust stack
	class that will hold a stack of integers.
*/

// The number of elements that the stack can contain is dynamically allocated on creation
// of the stack.  If the user of the stack decides not to pass the number of elements they want,
// we'll use 10 as the default
const int kDefaultStackSize = 10;

class CStack
{
	public:

		// Constructor
		// If a size is not passed, we'll set the stack to be 10 elements large 
		CStack(int size = kDefaultStackSize);

		// Copy Constructor
		// Since we're using dynamic data, we have to make sure 
		// we copy the stack correctly when passing it into a function
		CStack(const CStack &stack);

		// Assignment Operator
		// Since we're using dynamic data, we need to implement the 
		// assignment operator to make sure that one stack properly gets set
		// equal to another stack
		CStack& operator =(const CStack &stack);

		// Push an element (an integer) onto the top of the stack.  If the
		// stack is full, the error flag will get set to true
		void push(int element);

		// Pop (remove) the top element of the stack.  If someone calls "pop" on a stack
		// that is empty, mErrorFlag will be set to true.
		void pop(); 

		// Test if the stack is empty or if it is full
		bool isEmpty();
		bool isFull();

		// Returns the value at the top of the stack.  If the stack is 
		// empty, zero will get returned and the error flag set to true.
		int getTop();

		// Clears the error flag
		void clearErrorFlag() { mErrorFlag = false; }  

		// Data Access ***
			
			bool isError() { return mErrorFlag; } // Return the error flag

		// *** End Data Access

		~CStack();	// Destructor.  Free up all the memory used by the stack

	private:

		int *mData; // This is the pointer that will hold our stack of integers

		int mMaxSize; // This is the maximum number of elements that can be in our stack

		int mOpenSlot; // This points to the next OPEN slot in the stack.  This means that
					  // the top of the stack will always correspond to (mOpenSlot - 1)

		bool mErrorFlag; // If anybody is naughty and uses the stack in foul ways, we'll set
						// the error flag to true.  Otherwise it will equal false.
};

#endif



			   
