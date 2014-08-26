/*****************************************************
	Working with Threads and Mutex's

	Author: redKlyde
	File: main.cpp
	Description: an introduction to mutex's and critical data

*****************************************************/

// Things that will be covered in this tutorial:
// - What is critical data?
// - What is a Mutex?
// - CreateMutex()
// - ReleaseMutex()
// - CloseHandle()

// Now that we can create a thread, we don't need much else to be able to use them.  We
// simply do just as we did in the first tutorial ... make a thread proc ... call CreateThread
// to start execution ... and that's basically it.  We don't need anything else to get 
// a thread to run.  What we DO have to cover though, is critical data (or critical sections).

// When we create threads, we would like to be able to send data between them.  After all,
// generally we want to improve performance by handling certain processes in their own
// threads.  This typically means that one thread will be providing data for another thread.
// To do this, we need to have both threads share the same memory, so one thread can
// write the data, and the other thread can read the data.  This in itself is not a difficult thing
// to do because any function is going to have access to data in the global scope.  Since a
// thread is just a function, any data that a function has access to, our thread also has
// access to.

// So if all a thread needs to share data is to share scope, what's the big deal?  Well ...
// what happens if one thread tries to write data, at the same time the other thread tries
// to read the data?  The answer is simple, we would get corrupted data.  How do we avoid
// this then if both threads need to access the same data?  Well we would have to have
// some kind of signal that would tell one thread that the other thread is using the data.
// If the data is in use, we would wait until it is free, and if it is not in use, we want to tell
// the other thread that WE are now using the data.  This data we will call the critical section
// and the tool we will use to signal that we are using the critical section is called a mutex.

// A mutex is exactly as we described, it is a tool that we will "lock" when we are using the
// critical section, and "unlock" when we are done with the critical section.  When we want to
// get a lock on the mutex, we simply wait until the mutex is not locked (because if the mutex
// is locked that means our other thread is currently using the critical section), then lock it
// ourselves to tell everyone else that we are now using the data.  Surprisingly enough, you
// have already seen the function we will use to lock a mutex ... you used this in the first
// tutorial to wait for the thread to finish.  WaitForSingleObject() will do just exactly that.  We
// will use three functions CreateMutex() to create a mutex and get a handle to the mutex.
// WaitForSingleObject() will wait for that mutex to be released or unlocked and then lock it.
// And ReleaseMutex() will release or unlock a mutex that we have locked.

// Enough chatter, let's get to the code ...

// First, all our standard things.

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

// libraries we will use
#include <iostream>
#include <windows.h>

using namespace std;

// Let's make some data that both our threads will share.  Just for fun, lets make a thread
// that will take fill a string with some text ... and our main thread will wait until that string is
// full and then continue.

// This buffer is our critical data ... both our main thread and our threadProc will read/write
// to this buffer.  This is what we will protect with our mutex.

char buffer[128] = "\0";

// Well we can't protect our buffer very well without a mutex.  So let's define one now.
// A mutex is a HANDLE just like our threads.  This is the identifier we will use to reference
// our mutex with.

HANDLE mutexHandle = NULL;

// Now for some more knowledge.  When you create a thread, your computer has to spend
// some time setting up the process with the OS.  Depending on how you have structured
// your thread, this could take a substantial amount of time.  Because of this, we usually
// want to know exactly when our threads have started.  Well, here is a handy little trick.
// For our thread we are going to keep a bool that we will set to true in the beginning of
// our threadProc.  The only way that bool can be true is if our threadProc has started 
// executing.  Then we will simply wait in our main thread until the bool is true before we
// continue.

// So we need a bool to flag when our thread begins.

bool threadStarted = false;

// Now our threadProc
void threadProc() 
{
	// Remember to set our flag to true so our main thread will know we have started.
	threadStarted = true;

	// Now, how about we take our buffer ... and add on to it a number.
	// Let's do this about 10 times ... 

	for (int i = 0; i < 10; i++) {

		// Now the fun part.  Since we are about to write data to our buffer, we need to get a lock on the mutex.
		// Once we have the mutex locked, our main thread will not be able to write to or read from our buffer.
		// We can be sure our data is safe.  We will lock the mutex with WaitForSingleObject(), the same function
		// we used to wait on our thread to finish in tutorial 1.

		// lock our mutex
		WaitForSingleObject(mutexHandle, INFINITE);

		// Now we have a lock on the mutex, let's write data to our buffer.

		// If we use the counter i, we should end up with a string like "0123456789".
		// If you don't know what sprintf does, you really should look it up, it is a great tool.  All we are doing
		// is adding the value of i on to the end of our string.  So when i=0 our string is still empty.  If we add
		// i to the end, our string will now be "0".  Then when i=1, our string will now be "01".  At i=2, buffer will
		// equal "012", and so on and so on.

		// add i to our buffer
		sprintf(buffer, "%s%d", buffer, i);

		// In order to see things more slowly ... let's put a sleep here so we can observe what happens better
		// in the output window.
		Sleep(50);

		// We are done writing to our buffer.  So we have to release our mutex so other threads can access
		// the buffer.  We do this with the ReleaseMutex().  The argument for ReleaseMutex() is the mutex
		// handle we want to unlock.  So pass in our mutex handle, and our main thread is now free to access
		// buffer

		// release the mutex
		ReleaseMutex(mutexHandle);
	}
}

// the main thread
int main() 
{
	// a friendly welcome message
	cout << "Working with Threads and Mutex's by redKlyde\n";

	// Before we start up our thread, we have to create the mutex.  Otherwise all the calls we have 
	// made to our mutex will do nothing, because the mutexHandle doesn't contain a valid mutex.
	// It is important that we do this in the right order as well.  It is easy to overlook where the mutex
	// is created, but it should be done before the thread begins.  This will ensure that our mutex will
	// be created before anyone tries to use it.  Once we create our thread, there better be a valid
	// mutex for it to use or there could be some problems.

	// Let's create our mutex with CreateMutex().  For our purposes, the parameters are irrelevant, but
	// if you want to check them out, visit:
	// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createmutex.asp
	mutexHandle = CreateMutex(NULL, false, NULL);

	// We have our mutex ready for work, and we have defined our threadProc, so let's start up the
	// thread just as we did in the first tutorial.
	HANDLE threadHandle;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadProc, NULL, 0, NULL);

	// Remember our handy threadStarted bool?  Now's the time to use it.  Let's let this thread wait until
	// our threadProc sets this value to true, then continue on as normal.  This will make this thread
	// wait until our thread has begun executing.  (at the end of the tutorial, come back and comment out
	// this while loop and see what happens ... you will surely then understand its purpose)
	while (!threadStarted) {}

	// Now our thread is executing, so if we don't do something with our buffer, our thread is going to fill
	// it up with data.  To keep things simple, let's just watch what our threadProc is doing.  Let's print out
	// our buffer as threadProc is writing to it.  But remember, we can't (or shouldn't) access the data at 
	// the same time as the threadProc, so we have to protect our buffer with the mutex.

	// let's print 10 times just like our threadProc

	for (int i = 0; i < 10; i++) 
	{
		// Get a lock on the mutex so threadProc will not write to buffer while we are printing.
		WaitForSingleObject(mutexHandle, INFINITE);

		// Print the buffer
		cout << buffer << endl;

		// Just like our threadProc, let's sleep this thread so we can better observe what is going on.
		Sleep(50);

		// We are done printing, so let's release the mutex and allow threadProc to add more data to it.
		ReleaseMutex(mutexHandle);
	}

	// Wait for our thread to complete before we exit the app
	WaitForSingleObject(threadHandle, INFINITE);

	// Much like any memory we manually create, our system has allocated resources for our mutex, and
	// we must also release them in order to clean up properly.  We do this with CloseHandle().  The argument
	// is just the mutexHandle itself.

	// clean up our mutex
	CloseHandle(mutexHandle);

	// All done... you tell me when to exit
	system("pause");
	
	// Return zero for "zero problems in this program"
	return 0;
}

// Now it's run-time.  Compile and run the app and see how things look.  Do you get a nice looking output
// like this:

// 0
// 01
// 012
// 0123
// 01234
// 012345
// 0123456
// 01234567
// 012345678
// 0123456789

// Chances are you did, and that is a good thing.  But now it is time to experiment.  First, let's try changing
// the sleep values in the threads.  Change the "Sleep(50)" in the threadProc to "Sleep(40)."  Compile and rerun.
// Did you get a pattern where more than one number was inserted before the buffer printed?  Try it the other
// way around.  Sleep for 50 ms in threadProc, and sleep for 40 ms in the main thread.  What happens now?
// The main thread finishes printing before threadProc even gets through with the buffer.

// "So what's the point of all this red?"  What I am trying to illustrate is how you can't be dependent on the rate
// at which your threads execute.  In this case we are really really lucky that our threads run in pretty much the
// same amount of time.  If both sleep values were 0, it may be hundreds or thousands of times before
// we would see one thread put more or less data into the buffer than was expected.  By changing our sleep values
// we can easily observe this.

// Now for more fun.  Let's comment out the CreateMutex() function.  This will not create the mutex and thus any
// call to WaitForSingleObject() or ReleaseMutex() will do absolutely nothing.  This will allow our threads to execute
// without waiting on each other.  Of course, we would like to see something go wrong or for something to be 
// somehow different than when we did it the correct way and protected our data with our mutex.  Well, chances
// are with what we are doing with the threads everything will be just fine.  This is because we aren't really
// manipulating the buffer very much.  Besides that, we are not altering the buffer, but in one place, the threadProc.
// Since all the main thread is doing is printing what it sees in the buffer, it would be virtually impossible to cause
// a problem here.  But what if we WERE trying to manipulate the data in the buffer from our main thread?  This,
// I leave up to you.  For an exercise, rewrite the main() function to remove a character at a time from the front
// of the buffer (this means of course that you are going to have to make the buffer a queue for characters).
// Print the contents of the buffer before and after you remove the character.  Once you have all that, play with
// turning the mutex on and off, and changing the sleep values around.  With the right combination you could find
// a way to break the flow of data.

// That's all for now.  Next stop, "race conditions" and a classic data-locking situation.

// © 2000-2005 GameTutorials
