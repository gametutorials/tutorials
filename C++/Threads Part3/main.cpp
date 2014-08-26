/*****************************************************
	Race Conditions

	Author: redKlyde
	File: main.cpp
	Description: working with data locking scenario's

*****************************************************/

// Things that will be covered in this tutorial:
// - What are race conditions?

// Now in our last tutorial, we created a thread that added data to a buffer, and our main thread
// read the data from that buffer.  When we went back and experimented with the code, we saw
// that locking and unlocking our mutex didn't really effect the program that much.  We still obtained
// the results we expected when reading and writing data into our buffer.  In this tutorial, we will
// create a scenario that does NOT behave the way you would expect without properly locking and 
// unlocking the mutex.  The scenario we will present is a classic example of a "race condition".

// A "race condition" is a situation where two threads "race" to read or set values in our critical
// data.  In all honesty our previous program presented a good race condition where, depending
// on the sleep values we set in our threads, one thread may read or write data faster than the
// other thread.  This creates the concept of a "race".  Which thread will get there first?  The thread
// filling the buffer, or the thread reading the buffer.  Obviously the thread that executed faster
// would finish the race sooner than the other thread.  But since we were just reading data from
// the buffer, we couldn't negatively effect the data in the buffer.

// Now how about something that will NOT behave the way we expect?  Here is a classic race
// condition that will give you a better feel for working with mutex's.  In this example, we will 
// create two threads that will perform the exact same task.  The threads purpose is to print
// characters to the screen.  It will print either a '.' or a '#'.  It will determine which character
// to print according to the last character printed.  If the last character printed was a '.', the thread
// will print a "#".  Likewise, if the last character printed was a "#", the thread will print a '.'.

// Let's get straight to the code ...

// libraries we will use
#include <iostream>
#include <windows.h>
using namespace std;

// This is the last character we printed.  This is what we SHOULD protect with a mutex.
static char lastChar = '#';

// The procedure our threads will run
// This procedure accepts a sleep value as an argument so we can sleep each thread by 
// the value we pass in.  We can then use the same function and not have to write the same 
// code twice just to sleep the threads at different intervals.

void threadProc(int *sleepVal)
{
	cout << "sleepVal:  " << *sleepVal << endl;

	// Let's print 100 characters
	for (int i = 0; i < 100; i++) {

		// This is character we will print THIS time
		char currentChar;

		// If the last character printed is a '#'
		if (lastChar == '#') {
			// print a '.' this time
			currentChar = '.';
		} else {
			// otherwise .. print a '#'
			currentChar = '#';
		}
		// Sleep this thread, so we can run at a different rate than our other thread
		Sleep(*sleepVal);

		// Set the last character to the character we will print
		lastChar = currentChar;

		// Now print the character
		cout << currentChar;
	}
}

// Can you already predict what might happen?  If not, don't worry about it, we'll reveal all when we run
// the program.

int main() 
{
	// A friendly welcome message
	cout << "Race conditions by redKlyde\n";

	// We have already talked about it, but we need a sleep value for each thread.  This value will change
	// the rate at which each thread runs.  Let's make the sleep values just slightly off so they run at 
	// close to the same rate, but not exactly.  This will create the desired effect.
	int sleepVal1 = 50;
	int sleepVal2 = 40;

	// Now here's a handy trick.  We are going to pass an argument to our threads in the CreateThread function.
	// Remember our threadProc has an argument (int *sleepVal) ... or a pointer to an integer.  The fourth argument
	// in CreateThread is the "parameter" pointer for our threadProc.  Any integer address we place in this argument
	// will be the value of sleepVal pointer in threadProc.  If pointers are still a little fuzzy for you ... don't worry about
	// it, just know this all works.  You definitely should get more familiar with them though; pointers are the key to all
	// that is good.  For those who are confident in their pointer/reference abilities, you can play around with this
	// "parameters" argument and create some interesting ways to pass data to your threadProc.

	// Anyhow, back to business.  We pass the address of both our sleepVal's in each of our CreateThread calls.

	// Create thread 1 with sleepVal1
	HANDLE threadHandle;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadProc, &sleepVal1, 0, NULL);

	// Create thread 2 with sleepVal2
	HANDLE threadHandle2;
	threadHandle2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadProc, &sleepVal2, 0, NULL);

	// Wait for our threads to complete before we exit the app
	WaitForSingleObject(threadHandle, INFINITE);
	WaitForSingleObject(threadHandle2, INFINITE);

	cout << endl << endl; // A couple of blank lines for good reading measure

	// All done... you tell me when to exit
	system("pause");
	return 0;
}

// Now what would you expect when you run this program.  Naturally you would hope that all would behave
// as expected and you would see 200 characters of alternating '#''s and '.''s.  At least ... this is what we
// want our program to do.  Go ahead and run the program and see the results.

// Not exactly what we expected was it?  You likely saw a pattern similar to this:
// ".#.##..##.#.##..##"
// We see a pattern of .'s and #'s with occasional, if not often, double characters.  These don't alternate at all!!
// But why exactly is that?  Well, this is your classic race condition.  Which one will get there first?  Will one
// thread manage to read and write the lastChar variable before the next thread has the opportunity to do the
// same?  Sometimes yes, sometimes no.  We can see varying degrees of this by changing the sleep values
// around.  Try making them both 50 and running the app.  Did it fix the problem?  More than likely it did not!
// With our previous example, when the thread that was inserting data into the buffer ran at the same
// rate as the thread reading the data from the buffer, the two managed to stay pretty much in sync.  With this
// example though, unless they start at exactly the right time, the two threads will never be in sync.  You will
// probably always get an alternating pattern of "..##..##".

// Let's examine what happens in this example more closely.  Let's walk through each thread and see if we can
// determine why we are getting the results we are.

// So let's just say thread one is starting up, and it gets to examine the lastChar variable first.  It sees that 
// lastChar is a '#' and sets the character to print to be a '.' as it should.  It prints the '.' and sets lastChar to '.'
// and continues as it should.  During this same time, thread 2 begins to execute.  It enters and now sees that
// lastChar is a '.' and decides to print a '#' appropriately.  It sets the character to be printed to the '#'.  Thread
// one then continues in its execution and examines the last character and sees that it is still a '.'.  Thread one
// decides to print a '#'.  Thread two now prints his '#' and sets lastChar to '#'.  But then thread one does the
// exact same, he prints a '#' and sets lastChar to '.'.  You see, the two threads were both accessing the lastChar
// at the same time.  If both of them can observe lastChar at the same time, they can both, incorrectly, determine
// to print the same character, and now you have double characters printing to the screen.

// Note: If you have been playing around with the program already, you will see that it doesn't matter much where
// in the process you print the character, nor where you set the lastChar value equal to the new character.  You can
// certainly create situations where the threads will appear to behave in the expected manner, but you can't be certain
// of this.  A simple change like running the app on a slower or faster computer will throw off the entire synchronization.
// Feel free to experiment with the placement of the Sleep() functions, as well as the assignment of the lastChar and the
// printf().

// So how do we solve this little problem?  (You should know this by now) The answer is ... with a mutex!!!!  We need
// a mutex so that one thread can maintain complete control of access to lastChar until he is done with it.  He will
// lock the mutex before he reads lastChar.  Then he will unlock the mutex after he is done writing to lastChar.  This
// way when the other thread goes to check lastChar, he must first get a lock on the mutex to ensure that the other
// threads are not accessing the shared data.  Since we have already covered creating and using mutex's, I leave
// this task up to you.  Write in a mutex to this tutorial so that access to lastChar is limited to one thread at a time.
// Once you have done this, you can be certain you have all the tools you need to construct any threaded application.
// Just for fun, after you have written the mutex into the tutorial, create a third thread to perform the same task.
// Do any special modifications need to be made to the structure of the program to accomplish the task?

// Well that's all for this tutorial.  There really isn't much else to learn about threads before you can use them.  At this
// point you contain all the general knowledge needed to jump right in and start threading your applications.  Certainly
// you will learn many tricks and more efficient ways to handle your threads, but all that comes from experimenting
// and reading.

// © 2000-2006 GameTutorials
