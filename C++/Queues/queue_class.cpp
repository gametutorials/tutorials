#include "queue_class.h"

// Constructor, sets the size of the queue to whatever is passed in
CQueue::CQueue(int size)
{
	mOpenSlot = 0; // No elements in queue
	mMaxSize = size; // The maximum size of our queue
	mError = (size <= 0); // Get a bit tricky :)  If size is zero or negative, well that is an 
						 // error!  Thus mError would be set to "true", otherwise it will 
					    // start out as "false"

	if(mError == false)
		mData = new int[mMaxSize]; // Allocate the queue
}

// Copy Constructor
// We have dynamic data so we must be sure it gets copied correctly
CQueue::CQueue(const CQueue &qqq)
{
	mOpenSlot = qqq.mOpenSlot; // Set the index to be the same
	mMaxSize = qqq.mMaxSize; // Set the mMaxSize to be the same

	mData = new int[mMaxSize]; // Create the memory needed to store the integers in the queue

	// Now copy over each entry in the queue
	for(int i = 0; i < mMaxSize; i++)
		mData[i] = qqq.mData[i];
}

// We have dynamic data so when we set two queue's equal to one another we want to make sure
// each CQueue has a copy of the dynamic data
CQueue& CQueue::operator =(const CQueue &qqq)
{
	delete[] mData; // First delete any data associated with this CQueue
	mData = new int[qqq.mMaxSize]; // Create the new memory

	// Set "mOpenSlot" and "mMaxSize" to qqq's mOpenSlot and mMaxSize
	mOpenSlot = qqq.mOpenSlot;
	mMaxSize = qqq.mMaxSize;

	// Copy over the data
	for(int i = 0; i < mMaxSize; i++)
		mData[i] = qqq.mData[i];
	
	return *this; // Return a reference to "this" instance of the class
}

// Returns the front of the queue which will always be mData[0]
int CQueue::front()
{
	// We know mData is always allocated due to constructor, but that doesn't
	// mean anyone has added anything to it before calling front().  If they
	// do that, set the error flag.
	if(isEmpty())
		mError = false;
	
	return mData[0]; // mData[0] is always the front of the queue
}


// Returns the back of the queue 
int CQueue::back()
{
	if(!isEmpty())
	{
		// mOpenSlot equals the index of the next available slot in the
		// queue, so the last element in the queue will always equal mOpenSlot - 1
		return mData[mOpenSlot - 1];
	}
	
	// If the queue is empty, there is no "back of the queue" so we'll
	// just return the front of the queue which we know to be valid AND
	// it will set the "mError" flag correctly
	return front(); 	
}

// Insert into the queue.  Enough said %)
void CQueue::insert(int element)
{
	// If it is not already full
	if(isFull() == false)
	{
		mData[mOpenSlot] = element; // Set the element
		mOpenSlot++; // Move to the next open slot
	}
	else
	{
		mError = true; // We tried to insert into a full and tired queue
	}	
}

// Remove the front of queue
void CQueue::remove()
{
	// If there is something inside the queue
	if(isEmpty() == false)
	{
		mOpenSlot--; // The next open spot is now one less than what it used to be

		// Move down all the remaining elements in the queue
		for(int i = 0; i < mOpenSlot; i++)
			mData[i] = mData[i + 1];

		/* 
			A quick ASCII illustration of the above for loop:

				mData[0]'s value will be gone
				mData[0] will now equal data[1]
				mData[1] will now equal data[2]
				mData[2] will now equal data[3]
				etc, etc... 

			Until all the elements that have been previously set in the queue
			have been shifted down.
		*/
	}
	else
	{
		// Somebody tried to remove an element from an empty queue
		mError = true;
	}
}

// Returns if the queue is empty or not
bool CQueue::isEmpty()
{ 
	// Does this make sense?  If mOpenSlot equals 0 the queue is empty
	// !0 equals true, otherwise mOpenSlot will be a positive number (say 4) and !4 equals false.
	return !mOpenSlot; 
}	

// Returns true if the queue is full, false otherwise
bool CQueue::isFull()
{ 
	// If mOpenSlot equals mMaxSize then we are full.  Otherwise 
	// the condition will evaluate to false.
	return (mOpenSlot == mMaxSize); 
} 
// Destructor
CQueue::~CQueue()
{ 
	// Free up the memory
	if(mData)
	{
		delete[] mData;
	}
}


