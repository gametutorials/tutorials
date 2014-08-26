// Done by TheTutor

/*
	This tutorial has a two fold purpose:

	1)  Introduce the high level concept of a queue.
	2)  Implement, with a decent level of error checking, a queue class

	So lets tacke #1 first.  What is the heck is a queue?  Our geekspeak answer would
	respond, "A queue is a FIFO data structure.".  So what does that mean?
	
	Well FIFO stands for First In First Out.  Keeping that in mind you can
	think of a queue as a line at the grocery store.  Example:

	Check Out Register <$>	
					Customer1	Customer2	Customer3	Customer4	Customer5

	In the above queue, Customer1 was first in line (they are a fast shopper what can I say?), 
	so Customer1 will be the first customer to get checked out.  Customer1 would be referred to 
	as the front of the queue, while Customer5 would be referred to as the back of the queue.

	Every time you insert into a queue, it gets added to the back of the queue.  Every time
	you remove from a queue, the current front gets removed and the second thing in line (in the
	above example Customer2) becomes the new front of the queue.

	Make sense?  Well hold on to your hats 'cause we're about to implement a simple queue class.
*/

#include "queue_class.h"
#include <iostream>
using namespace std;

int main()
{
	int i = 0; // Our loop variable
	CQueue queue(50); // We'll make our queue 50 elements large

	// Let's put the numbers 1 through 10 onto the queue
	for(i = 1; i <= 10; i++)
		queue.insert(i);

	// Now let's print out the value at the front of the queue and the back of the queue
	cout << "Front of queue = " << queue.front() << endl;
	cout << "Back of queue = " << queue.back() << endl << endl;

	// Remove 2, that's T-W-O elements
	queue.remove();
	queue.remove();

	// Now let's print out the value at the front of the queue and the back of the queue
	cout << "Front of queue = " << queue.front() << endl;
	cout << "Back of queue = " << queue.back() << endl << endl;

	// Let's add the numbers 20 - 30 to the queue
	for(i = 20; i <= 30; i++)
		queue.insert(i);

	// Again, let's print out the value at the front of the queue and the back of the queue
	cout << "Front of queue = " << queue.front() << endl;
	cout << "Back of queue = " << queue.back() << endl << endl;

	// Lastly, for a sanity check these two statements should print to the screen
	if(queue.isFull() == false)
		cout << "The queue is not full" << endl;

	if(queue.isEmpty() == false)
		cout << "The queue is not empty" << endl;

	// Skip a line
	cout << endl << endl;

	// Okay, now if we implemented our CQueue correctly we should be able to set 
	// two CQueue equal to one another.  Lets try it.

	CQueue secondQueue; // Notice the default constructor will make "secondQueue" be of max size 10

	secondQueue = queue;	// They're equal now

	// Let's print them out to be sure
	while(queue.isEmpty() == false)
	{
		cout << queue.front() << " ";
		queue.remove();
	}

	// Move to next line
	cout << endl;

	// Now print out the second queue
	while(secondQueue.isEmpty() == false)
	{
		cout << secondQueue.front() << " ";
		secondQueue.remove();
	}

	// Skip some more lines for fun
	cout << endl << endl;
		return EXIT_SUCCESS; // And we're done...
}

/* Do you Q?

	Why use a queue you ask?  Well lets check out a game example where a queue may come in handy.

	Lets assume you are playing a RPG like Zelda or something.  Your character gets so far in a cave and
	then BAM a big door comes down behind you and seals you in the current room.  There is a door 
	in front of you but it is sealed by an unseen magical force.  While all this chaos closes in around you
	slimes, one by one, ooze in from the floor and begin attacking you.

	This is where the technical part ties in, the slimes could be stored in a queue.  Every time you
	kill one, another one gets popped off the queue until it is empty which would then trigger the
	event to open that magically sealed door.

	If you still have Q's about queues be sure to post 'em at www.GameTutorials.com 
*/
	
/*-------------------------*\
|  Programmed by:  TheTutor	|
|  ©2006 GameTutorials, LLC	|
\*-------------------------*/