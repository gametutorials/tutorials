#ifndef QUEUE_CLASS_H
#define QUEUE_CLASS_H

class CQueue
{
	public:

		// Constructors
		// We're going to overload our constructor so that we can give the queue any size we want
		CQueue(int size = 10); // The queue created will be whatever size we pass in
							   // **NOTE** Since we put a default value for size, this
							   // also doubles as a default constructor

		// Copy Constructor
		// We have dynamic data so we want to make sure it gets handled correctly.
		CQueue(const CQueue&);

		// Assignment Operator
		// Again we have dynamic data so if we set two CQueues equal to one another we are responsible
		// for making sure everything gets allocated and set correctly
		CQueue& operator =(const CQueue&);

		int front(); // Returns the first element in the queue, but doesn't remove it from the queue
		int back();	// Returns the last element in the queue, but doesn't remove it from the queue 

		void insert(int element); // Puts "element" into the queue (in the back of the queue)
		void remove(); // Removes the first element of the queue

		bool isEmpty(); // This will return true when the CQueue is empty, false otherwise
		bool isFull(); // This will return true when the CQueue is full, false otherwise

		void clearError() { mError = false; } // Clear the error flag
		bool isError() { return mError; } // This will return true if an error occurred in 
										 // any of the methods, false otherwise

		~CQueue(); // Destructor frees memory associated with CQueue

	private:

		int *mData; // An array to store all the integers in the queue
		int mOpenSlot; // This index always correspond to the next available slot in the queue
		int mMaxSize; // This is the maximum amount of elements that the queue can hold

		bool mError; // True if there has been an error
};


#endif
