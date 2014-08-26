#include <stdlib.h>
#include "list.h"

// The constructor -- Initially the "head" and "tail" pointers point to NULL
CList::CList():mHead(NULL),mTail(NULL) { /* do nothing */ }

// Inserts the data passed in to the beginning of the list
void CList::insertBegin(int _data)
{
	// If the list is empty, then we need to create it
	if(mTail == NULL)
	{
		mTail = new SNode; // Create the tail node
		
		mTail->data = _data; // Set the data passed in
		mTail->next = NULL; // There is no next element in the list so
						  // be sure to set this to NULL
						  
		// Because this is the first element in the list, it is
		// also the head of the list, so set the head and the 
		// tail equal.
		mHead = mTail;
	}
	else // Otherwise at least one element in the list has been set
	{
		// Create a new node
		SNode *node = new SNode;
		
		node->data = _data; // Set the data that's been passed in
		node->next = mHead; // This is the new head node in the list
						  // so link it to the current head node.
						  
		mHead = node; // Update the head pointer so that it
					// points to the newly created beginning node.
	}
}

// Inserts the data at the end of the list
void CList::insertEnd(int _data)
{
	// If the list is empty, we have to create it
	if(mHead == NULL)
	{
		mHead = new SNode; // Create the head node
		
		mHead->data = _data; // Set the data
		mHead->next = NULL; // No other nodes currently in list, so set
						  // set the "next node" to NULL.
						  
		// Because this is the first and only node in the list, it not
		// only is the head node, but also the tail node.  So 
		// set the tail to the head.
		mTail = mHead;
	}
	else // Otherwise there's at least one node in the list so 
	{	// add this node at the END of the list.
		
		SNode *node = new SNode; // Create a new empty node
		
		node->data = _data; // Set the data
		node->next = NULL; // This node will be at the end of the 
						  // list so it's "next node" will be NULL.
		
		// Link the current tail to the new end of the list				  
		mTail->next = node;
		
		// Reset the tail to point at the end of the list
		mTail = node;
	}
}

// Prints every element in the list, starting at the "head" of list,
// then prints out the value of the "head" and "tail element.
void CList::printList()
{
	SNode *walker = mHead; // We'll use this node to walk through the
						 // list.  Since we want to begin at the "head" we
						// set our walker to equal the head node.
	
	// While we have NOT reached the end of our list					
	while(walker != NULL) 
	{
		// Print out the data with a space after it
		cout << walker->data << " ";
		
		walker = walker->next; // Move to the next element in the list
	}
	
	// After we've printed the entire list, move to the next line
	cout << endl;
	
	if(mHead)
		cout << "Head Node = " << mHead->data << endl;
		
	if(mTail)
		cout << "Tail Node = " << mTail->data << endl;
}

// This wipes the list, freeing all memory in the process
void CList::clear()
{
	SNode *walker = mHead; // Set a "walker node" to the front of the list

	while(walker != NULL)
	{
		SNode *temp = walker; // Create a temp node 

		walker = walker->next; // Move the walker to the next node on the list

		delete temp; // Free the memory
	}
	
	mHead = mTail = NULL; // Set everything to NULL
}

// The Deconstructor -- Free all memory associated with the list
CList::~CList() { clear(); }
