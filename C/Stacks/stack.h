#ifndef STACK_H
#define STACK_H

#include <memory.h> // Used for memset()
#include <stdlib.h> // It's the standard library, what can I say?


// Unfortunately in C Land we don't have the "bool".  The "bool" is handy
// so we'll typedef our own.  Now we can use "bool" just like we would in C++ Land.
typedef unsigned char bool;

#define true 1
#define false 0

/* First off, what is a stack?  Well a stack is a LIFO based "container".  Yeah,
okay but what is that?

  LIFO stands for Last In First Out.  So a stack is nothing more than "something" that has
  this property:

  A perfect example is a stack of dishes.

  plate5		-- Here plate5 was added LAST to the SStack of dishes.
  plate4		   Yet, when it comes time to wash them, plate5 will be
  plate3		   the FIRST to be washed (removed).  Once plate5 is removed, plate4 will
  plate2		   have the status of "being the last plate added to the stack", so it will
  plate1		   be the next dish to be washed (removed).  Etc, etc, etc...

  // Make sense?  Now when related back to programming, a stack is a LIFO based arrangement
  of data (variables).

  // There are multiple ways you can represent a stack but we're going to
     use an array to do it.

*/

// Here is what our stack is going to look like -- 
// It's going to be a stack of integers that has a dynamic size which is created at
// creation of the stack (declaring a variable of type SStack).
typedef struct _SStack
{
	int *elements; // Pointer for our dynamic stack to be held in

	int maxElements; // This will hold our maximum capacity of integers that can
					 // be held by the stack.
	
	int openIndex; // This will always correspond to the next OPEN slot in the stack
				  // that means that the TOP of the stack will always be (openIndex - 1).
} SStack;


// Here's the functions will use to access the stack

// This MUST be called before we can use our stack.  It will set the max size of the
// stack and allocate memory for the stack.  It returns true if everything was successful, false otherwise.
bool InitStack(SStack *stack, int size);

// This will take "num" and "push" it onto the stack (add it to the top of the stack).
// If the function is successful it will return true, otherwise it returns false.
bool Push(SStack *stack, int num);

// Pop() returns the TOP element of the stack and removes it from the stack (i.e. we can 
// fill it with something else).  If there are no elements in the stack.
// Pop() horribly crashes (Haha -- Just kidding).   Actually if there are no elements in
// stack, Pop() by default returns 0.
int Pop(SStack *stack);

// Frees all memory associated with the stack.  Returns true on success, false otherwise.
bool FreeStack(SStack *stack);

/* Really that's all we need.  Now for safety sake it would probably be a good idea
   to change the int Pop(SStack *stack) to bool Pop(SStack *stack, int *top);

   Here Pop() would return true if successful and fill "top" with the TOP of the stack.
   Otherwise it would return false and "top" would remain unchanged.

   But, it's certainly not necessary.  It's just that there is no way for a client (someone else
   calling our function) to know if Pop() failed or not since 0 could be a valid
   element on the stack.

   Just something to ponder...
*/

#endif
