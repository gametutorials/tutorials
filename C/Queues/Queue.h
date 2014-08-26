#ifndef QUEUE_H
#define QUEUE_H

#include <memory.h> // Needed so we can call memset()
#include <stdlib.h> // It's the standard library, what can I say?


// Unfortunately in C Land we don't have the "bool".  The "bool" can be very handy
// so we will typedef our own.  Now we can use "bool" just like we would in C++ Land.
typedef unsigned int bool;

#define true 1
#define false 0

/*	Okay for starters, what the heck is a queue?  Well, a queue is nothing more than
	a FIFO structure.  FIFO stands for First In First Out --

	So a queue can be thought of as the line at a grocery store. Example:

	Check Out <>	Customer1	Customer2	Customer3	Customer4	Customer5

	In the above queue, Customer1 was first in line (they got their first), so Customer1 
	will be the first customer to get checked out.  Customer1 would be referred to as the
	front of the queue, while Customer5 would be referred to as the back of the queue.

	See, simple.  So we're going to implement a queue (that holds integers) using a 
	"SQueue" structure and some functions.

*/

// Here is what our queue is going to look like -- 
// It's going to be a queue of integers that has a dynamic size which is created when
// a queue is declared.
typedef struct _SQueue
{
	int *elements; // Pointer for our dynamic queue to be held in

	int maxElements; // This will hold our maximum capacity of integers that can
					// be held by the queue.
	
	int openIndex; // This will always correspond to the next open slot in the queue
				  // that means that the BACK of the queue will always be (openIndex - 1).

} SQueue;

// Here's the functions we'll use to access the queue.

// This MUST be called before we can use our queue.  It will set the max size of the
// queue and allocates memory for the queue.  It returns true if everything was successful, false otherwise.
bool InitQueue(SQueue *queue, int size);

// This will take "num" and insert it onto the queue (add it to the back of the queue).
// If the function is successful it will return true, otherwise it returns false.
bool Insert(SQueue *queue, int num);

// Remove() returns the FRONT element of the queue and removes it from the queue (i.e. everything
// gets moved down, so the element after the FRONT element is now the FRONT).
// If there are no elements in the queue, Remove() destroys your hard drive.
// (Haha -- Just kidding).  Actually if there are no elements in queue, Remove() 
// by default returns 0.
int Remove(SQueue *queue);

// Frees all memory associated with the queue
void FreeQueue(SQueue *queue);

/* Really that's all we need.  Now for safety sake it would probably be a good idea
   to change the int Remove(SQueue *queue) to bool Remove(SQueue *queue, int *front);

   Here Remove() would return true if successful and fill "front" with the FRONT of the queue.
   Otherwise it would return false and "front" would remain unchanged.

   But, it's certainly not necessary.  It's just there is no way for a client (someone else
   calling our function) to know if Remove() failed or not since 0 could be a valid
   element on the queue.

   But I never said we we're going to make a GOOD "queue implementation".  Just want to
   get familiar with the concepts :)

*/

#endif
