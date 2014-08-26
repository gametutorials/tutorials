#ifndef LIST_H
#define LIST_H

#include <iostream>
using namespace std;

/* 
	First off, what is a linked list?  A linked list is a list of "nodes".  Nodes can 
	thought of "structures" that hold data and a link to another node.  These nodes are
	"chained together" to form a list.

	A link list IS NOT a LIFO or FIFO structure by default.  So it is very important to
	define a couple of key terms in a linked list.

	The "head" of the list points to the "beginning" of the list.  For our list
	implementation this is ALWAYS going to be the FIRST node placed in the list UNLESS
	we place a node before the head in which place this node becomes the new head node.
	   
		**NOTE** It is not uncommon to have the "head" node be the last node placed 
				into the list

	The "tail" of the list points to the "end" of the list.  For our implementation this
	will be the node who's next link is NULL.  Check out the diagram below.
	   
		**NOTE** You can define your list how you want, just be specific :)

	A graphical representation of our linked list may look something like this:

		[data]->[data]->[data]->[data]->[data]->[data]->NULL
		"head"									"tail"	

	NULL (or an empty node) is the most common way to signify the end of a linked list
	  
	The "head" and "tail" will be kept track of internally.  Our list is going to give the
	client (person using it) four ways to enter data:

	1) Before the head
	2) After the head
	3) Before the tail
	4) After the tail

	To retrieve data stored in the list the client can either:

	1) Retrieve the data stored at the head node
	2) Retrieve the data stored at the tail node

	I think that about does it.  So on with the list making...
*/

// This is a VERY COMMON data structure used by linked lists.
// Now for our example, this node is simple (it's only data element is a integer), but 
// we could have a "Player struct" in here for a list of players or whatever else we
// want to keep a list of.
struct SNode
{
	int data; // This is our data
	SNode *next; // This is a pointer to "our next node" ("next link") in our 
			 	// linked list.
};

class CList
{
	public:

		// Our constructor -- This will initialize our linked list
		CList();

		// This inserts the data the start of the list.  If this was the list:
		/*
			H == Head of list
			T == Tail of list
		
			[1]->[2]->[3]->NULL
			 H		   T
			
			And we passed in "99", after the functional call the list would
			look like this:
			
			[99]->[1]->[2]->[3]->NULL
			 H               T
		*/			
		void insertBegin(int data);
		
		// This inserts the data the end of the list.  If this was the list:
		/*
			H == Head of list
			T == Tail of list
		
			[1]->[2]->[3]->NULL
			 H		   T
			
			And we passed in "99", after the functional call the list would
			look like this:
			
			[1]->[2]->[3]->[99]->NULL
			 H              T
		*/			
		void insertEnd(int data);
		
		// Prints the CList by starting at the "head" and 
		// printing out the data for each element in the list followed
		// by the space.  The prints out the value of the "head" and "tail" element
		void printList();

		// This clears a list COMPLETELY, meaning all the memory associated with it is
		// cleared and it is if you were starting with a fresh list.
		void clear();

		// The deconstructor -- This frees all the memory associated with the CList
		~CList();

	private:
		
		SNode *mHead; // This will always point to the head of our list
		SNode *mTail; // This will always point to the tail of our list

};




#endif
