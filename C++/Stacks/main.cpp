// Done by TheTutor

/*
	This tutorial has a two fold purpose:

	1)  Introduce the high level concept of a stack.
	2)  Implement, with a decent level of error checking, a stack class

	So lets tackle #1 first.  What is the heck is a stack?  It's not a tripple patty burger at
	your local fastfood joint.  We are talking about something different here.  
	A stack is a LIFO structure.  So what is a LIFO stucture?

	LIFO stands for Last In First Out.  Here's an example of a stack:

	plate5		Here plate5 was added LAST to the stack of dishes.
	plate4		Yet, when it comes time to wash them, plate5 will be
	plate3		the FIRST to be washed (removed).  Once plate5 is removed, plate4 will
	plate2		have the status of "being the last plate added to the stack", so it will
	plate1		be the next dish to be washed (removed).  Etc, etc, etc...

	Every time you push something on a stack, it gets added to the the top of the stack.  Every time
	you remove from a stack, the top item gets removed and the item directly below the top becomes
	the new top of the stack.

	Now it's time to take this concept to practice, by implementing our very own stack class.
*/

#include "stack_class.h"

/* Remember we #include <iostream> in "stack_class.h" so we DON'T have to re-include it here */

int main()
{
	CStack stack; // Default constructor is called.  Our stack will be 
				 // kDefaultStackSize (10) elements large.

	// Let's push some integers (0, 1, 2, 3, 4) on the stack
	for(int i = 0; i < 5; i++)
		stack.push(i);

	// Now lets pop all the elements off of the stack printing each one 
	// as they come off
	while(stack.isEmpty() == false)
	{
		cout << "Top = " << stack.getTop() << endl;
		stack.pop();
	}
	
	return EXIT_SUCCESS;

} // end of main()

/* 
	Don't blow your stack!

	Stacks are one of the three (stacks, queues, and lists) data structures in computer programming
	you will see over and over and over again.  As you tackle different programming tasks, 
	inevitable you bump into something where a stack provides a slick solution to the problem at hand.

	If you have any further questions about stacks and the potential uses, be sure to post
	your quandaries at www.GameTutorials.com.
*/

/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/



