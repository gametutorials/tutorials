#include "stack.h"

// The fun part -- Implementing our wonderful creation the SStack!

bool InitStack(SStack *stack, int size)
{
	// Can't create a SStack of negative or zero size
	if(size <= 0)
		return false;
	
	// Create an array of integers as big as the "size" passed in
	stack->elements = (int*)malloc(size * sizeof(int));

		// Error Check -- malloc returns NULL if it can not allocate
		//				  the memory we requested
		if(stack->elements == NULL)
			return false;

	// Otherwise everything was created successfully so set our two other variables
	// and return true.
	stack->maxElements = size;
	stack->openIndex = 0; // Nothing is on the stack

		return true;

} // end of InitStack(SStack *stack, int size)

bool Push(SStack *stack, int num)
{
	// If the stack is full we CAN NOT push anything on to it
	if(stack->openIndex == stack->maxElements)
		return false;

	// Fill the stack with "num" that was passed in
	stack->elements[stack->openIndex] = num;

	// Move to next OPEN slot in the stack
	stack->openIndex++;

	return true;
}

// Return the top element of the stack and remove it from the stack
int Pop(SStack *stack)
{
	// If the stack is empty we can't pop anything of it so by default we're going
	// to return zero.
	if(!stack->openIndex)
		return 0;

	// Otherwise we want to return the top element of the stack and remove it from
	// the stack.

	stack->openIndex--; // Move to the top element which also corresponds to the next OPEN
						// slot in the stack.

	return stack->elements[stack->openIndex];  // The TOP of the stack
}

// Last but not least, since we're good programmers, we'll free all the memory
// (Assuming of course the client calls our function).
bool FreeStack(SStack *stack)
{
	// CAN NOT free a "stack" that doesn't point to anything (has no memory associated with it)
	if(!stack)
		return false;

	// Same idea -- If the "array" portion of the stack doesn't have any memory associated
	// with it, we can't free it.
	if(!stack->elements)
		return false;

	free(stack->elements);

	// Zero out the stack's variables
	stack->elements = NULL;
	stack->maxElements = 0;
	stack->openIndex = 0;
		
	return true; // We freed the memory!
}

	 



