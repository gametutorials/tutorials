#include "stack_class.h"

// Construct the stack
CStack::CStack(int size)
{
	if(size > 0)
	{
		mMaxSize = size; // Max number of elements on our stack
		mOpenSlot = 0; // The array is wide open so the first open slot in the stack is zero

		mData = new int[mMaxSize];

		// Error Check
		if(mData == NULL) // We couldn't dynamically allocate memory
			mErrorFlag = true; // Ahhh... Houston we have a problem
		else
			mErrorFlag = false;	// Yeah buddy!  We we're successful
	}
	else
	{
		mErrorFlag = true; // Somebody was funny and passed in a negative or zero size
						   // for the stack...
	}
}

// Copy Constructor 
CStack::CStack(const CStack &stack)
{
	// Copy over max size and open slot
	mMaxSize = stack.mMaxSize;
	mOpenSlot = stack.mOpenSlot;
	
	// Allocate enough space for the stack
	mData = new int[mMaxSize];

	if(mData)
	{
		mErrorFlag = false; // We should be in the clear
		
		// Lastly, copy the array of integers from one stack to the other
		for(int i = 0; i < mMaxSize; i++)
			mData[i] = stack.mData[i];
	}
	else
	{
		mErrorFlag = true; // Couldn't allocate the memory!!!
	}
}

// Place on element on the top of the stack... Carefully %)
void CStack::push(int element)
{
	// If we are already, full then we can't push anything else
	// onto the stack no matter how hard we try
	if(isFull())
	{
		mErrorFlag = true; // Set the error flag
			return;
	}

	// Set the newly pushed element
	mData[mOpenSlot] = element;	// Fill the current open slot in the stack with "element"
	mOpenSlot++; // Move to the next open slot
}

// Remove the element at the top of the stack
void CStack::pop()
{
	if(isEmpty())
	{
		mErrorFlag = true;
			return;
	}

	// Well we want to remove the top element, in the stack.  To do this, all we have
	// to do is decrement "mOpenSlot".  This effectively, removes the top element of the 
	// stack. 
	mOpenSlot--;	
}

// True if the stack is empty, false otherwise
bool CStack::isEmpty()
{
	// Okay lets think about this.  mOpenSlot points to the next open slot in the 
	// stack.  So if mOpenSlot equals zero, then this must mean nothing is in
	// the stack.  Thus the below conditional should do the trick!
	return (mOpenSlot == 0); // The stack is empty if mOpenSlot equals zero, otherwise it is not empty
}

// True if the stack is full, false otherwise
bool CStack::isFull()
{
	// Take a second and think about the stack.  mOpenSlot points to the next open
	// slot in the stack.  mMaxSize is the maximum size of the stack.  Therefore, if 
	// mOpenSlot equals mMaxSize, the stack is full
	return (mOpenSlot == mMaxSize); // Stack is full if mOpenSlot equals mMaxSize, otherwise it is not full
}

// Returns the top element of the stack.
// **NOTE** This does not remove it from the stack, it only returns
//			the value at the top of the stack
int CStack::getTop()
{
	// If the stack is empty, we will return 0 by default
	if(isEmpty())
	{
		mErrorFlag = true; // Somebody made a boo-boo
			return 0;
	}

	// Otherwise the stack isn't empty, so we know that mOpenSlot points
	// to the next OPEN slot in the stack, thus (mOpenSlot - 1) must
	// be the current top element
	return mData[mOpenSlot - 1];
}

// Destructor.  Free the memory associated with CStack
CStack::~CStack()
{
	if(mData) // If there is data ...
	{
		// ... Free it up
		delete[] mData;
		mData = NULL;
	}
}




