// Done by TheTutor

// As you may of guessed from the title, this tutorial is about "stacks" :)
// There isn't much of a prelude here 'cause all the "hard-core" explaining goes on
// in "stack.h" and "stack.c".
// 
// So without further ado...

#include <stdio.h>
#include "stack.h"

int main()
{
	// Let's make a stack -- Always a good idea initialize variables to some known value
	SStack stack = {0};
	int counter = 0; // A counter for our "for loops"
	
	InitStack(&stack,10);


	// Let's fill the stack with numbers 0 - 9.
	// When this for loop is done the stack will be FULL.
	for(counter = 0; counter < 10; counter++)
		Push(&stack,counter);
	
	if(Push(&stack,22))
		printf("This is we bad! We just pushed an element onto a full stack.\n\n");

	else
		printf("This is good!  We were denied while trying to \"push\" onto a full stack.\n\n");


	// We'll print out the numbers.  You'll see there in LIFO order (The last one we
	// pushed on, is the first one we'll pop off, etc).
	for(counter = 0; counter < 10; counter++)
		printf("We just popped off #%d\n",Pop(&stack));
		
	
	if(FreeStack(&stack))
		printf("\nYeah the memory was freed successfully!\n\n");
	else
		printf("\nOh NO!  An error occurred while trying to free the memory");

	return 0;
}

/* Extra Chatter %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  What happens if we didn't Pop() all the elements off the stack and then tried to free it?
  Well I know what you are all thinking, your thinking "It would still FREE ALL THE MEMORY".
  And that's CORRECT!  It would still free all the memory. 

  If you have questions that's too bad, you're just going to have to figure 'em out on your own
  
  OR 
  
  you can post 'em at www.GameTutorials.com %)

*/


/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
