// Done by TheTutor

/*  
	In this tutorial we are going to cover the topic of linked lists.  A linked 
	list is one of the classic ways to keep around a set of data.  The advantage
	of using a list is that it has no predetermined size you can add as many
	or as few of items to the list as you want.
	
	The bulk of the work happens in list.cpp, so without listing
	any other reasons why, lets begin!
*/

#include "list.h"

// Our main creates a CList and fills it with data using the 4 insert
// methods contained in our CList class.
int main()
{
	CList list; // Here's our list 

	// First we're going to insert 1,2,3,4,5 after the head
	for(int i = 1; i <= 5; i++)
		list.insertBegin(i);
	
	list.printList(); // Print the list
	cout << endl << endl; // Skip two lines	
	
	list.clear(); // Clear out the list

	// Now we're going to insert 1,2,3,4,5 after the tail
	for(int i = 1; i <= 5; i++)
		list.insertEnd(i);

	list.printList(); // Print the list
		
	return 0; // Tutorial is finished :)
}	

/* Your local listings...

	This is just one of many representations of a list.  The most important thing
	when creating your own list class is to be specific as to what the "head" of the
	list is and the "tail" of the list is (assuming you have a "tail" of the list).
	
	Lists can be handy when you don't know how many items you are going to want
	grouped in some sort of a ... well list %)  Just remember because you are 
	allocating memory every time you add an element to the list, you do have
	a speed hit when using a list.  If you know for sure the number of elements
	you want in a list, you're probably better off using a stack, queue, or 
	just good old array to store the data.

	Any questions or comments please post 'em on the message board at:
	www.GameTutorials.com
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
