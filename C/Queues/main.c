// Done by TheTutor

/* Okay, all the good green stuff (the explanations) are in Queue.h and Queue.c.
   What this program is going to do is make a queue, insert some ints to the queue,
   then print them to the screen as we remove them from the queue.
*/

#include <stdio.h> // So we can print to the screen
#include "Queue.h"

int main()
{
	SQueue queue = {0}; // Our queue
	int i, front; // A counter variable and a variable for holding the front of the queue

	// Try to init our queue to 10 elements large.  If it fails, quit the program.
	if(InitQueue(&queue,10) == false)
		return EXIT_FAILURE;

	// Add the numbers 1 - 10 to our queue
	for(i = 1; i <= 10; i++)
	{
		// If we can't insert, free the queue and quit the program.
		// Remember we insert at the BACK of the queue.
		if(Insert(&queue,i) == false)
		{
			FreeQueue(&queue);
				return EXIT_FAILURE;
		}
	}

	// Remove all the elements from the queue, printing the "current front element"
	// each time we remove one.
	for(i = 1; i <= 10; i++)
	{
		front = Remove(&queue); // Get the front of the queue

		printf("Current front of queue is: %d\n",front);
	}

	FreeQueue(&queue); // Free the queue
		return EXIT_SUCCESS; // We did a mighty fine job
}

/*	Extra chatter...

	First off I want to say that this queue implementation is NOT robust in the slightest.
	It's intent is just to get you familiar on the high level concept of a queue and show you
	a possible implementation (although very primitive).  Generally the functions/methods 
	"insert" and "remove" are associated with a queue.  Also there are generally functions/methods.
	"front" (retrieves the front of the queue doesn't remove it from the queue) and
	"back" (retrieves the back of the queue, doesn't remove it from the queue) in a 
	more robust implementation of a queue.

	As always, post your questions at www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
