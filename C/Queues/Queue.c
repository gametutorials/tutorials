#include "Queue.h"

// The fun part -- Implementing our wonderful creation the SQueue!

// This will initialize the queue
bool InitQueue(SQueue *queue, int size)
{
	// Can't create a SQueue of negative or zero size
	if(size <= 0)
		return false;
	
	// Create an array of integers as big as the "size" passed in
	queue->elements = (int*)malloc(size * sizeof(int));

		// Error Check -- malloc returns NULL if it can not allocate
		//				  the memory we requested.
		if(queue->elements == NULL)
			return false;

	// Otherwise everything was created successfully so set our two other variables
	// and return true.
	queue->maxElements = size;
	queue->openIndex = 0; // Nothing is on the queue

	return true;

} // end of InitQueue(SQueue *queue, int size)

// This will insert an element on to the BACK of the queue
bool Insert(SQueue *queue, int num)
{
	// If the queue is full we CAN NOT put any more elements in the queue
	if(queue->openIndex == queue->maxElements)
		return false;

	// Fill the queue with "num" that was passed in
	queue->elements[queue->openIndex] = num;

	// Move to next OPEN slot in the queue
	queue->openIndex++;
		return true;
}

// Return the FRONT element of the queue (and actually remove it from the queue).
// Then slide the elements on the queue down (so that we have a new FRONT element).
int Remove(SQueue *queue)
{
	int front = 0; // This is going to hold the front of the queue
	int i = 0; // A counter variable

	// If the queue is empty we can't remove anything from it so return the 
	// "default front".
	if(!queue->openIndex)
		return front;

	// Otherwise we want to return the front element of the queue (this is always
	// queue->elements[0]).
	front = queue->elements[0]; // Save off front element

	queue->openIndex--; // Decrease the index, since we're removing an element
						// the next open slot on the queue will be one less than what it was.

	// Now slide everything down
	for(; i < queue->openIndex; i++)
		queue->elements[i] = queue->elements[i + 1];

	return front;  // Return the FRONT of the queue

}

// Last but not least, since we're good programmers, we'll free all the memory
// (Assuming of course the client calls our function).
void FreeQueue(SQueue *queue)
{
	// In case the client passes in a "bogus queue"
	if(!queue)
		return;

	// If the "array" portion of the queue doesn't have any memory associated
	// with it, we can't free it.
	if(!queue->elements)
		return;

	free(queue->elements);
	
	// Zero out the SQueue
	queue->elements = NULL;
	queue->maxElements = 0;
	queue->openIndex = 0;
}
