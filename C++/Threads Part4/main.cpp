/*****************************************************
	A Simple Thread Class

	Author: redKlyde
	File: main.cpp
	Description: creating a simple thread class for windows

*****************************************************/

// At this point we know all we need to know about threads.  We COULD use the functions we have
// and begin creating them and using them in our programs, but as C++ programmers, we like
// to make nice objects to do all the work for us so we can make our code cleaner and easier to 
// understand and program with.

// So is there a way we can do the same thing with a thread?  Well of course we can!  What we 
// are going to do in this tutorial is create a thread class that should simplify creating and using
// threads so we don't have to rewrite the same code over and over for every thread.

// For convenience we will make the class right here in the main.cpp file so you don't have to 
// switch back and forth between header and source files.  Let's make our class now.  What
// exactly do we need?  Well, we need a threadProc, a thread handle, and we would like a 
// convenient way to start and stop our thread.

// Let's define that quickly ...

// First, our include files
#include <conio.h>	// For _kbhit()
#include <iostream>
#include <windows.h>
using namespace std;

class CThread 
{
protected:
	// This flag will signal our threadProc to stop executing
	bool terminateThreadFlag;

	// The handle to our thread
	HANDLE threadHandle;

public:
	// A constructor to setup our class correctly
	CThread();

	// Begin the thread and execute the threadProc
	void begin();

	// Signal to stop our thread
	void end();

	// We will make this function virtual so that we can derive a new thread with a new procedure.
	// This will give us the ability to only have to define this one function for any new thread we
	// want to create, and we can use the convenience of our thread functions over and over again.
	virtual void threadProc() = 0;
};


// That should be the basics for our class.  We just need to fill in the parts.  Let's start with the 
// constructor.  All we need to do is initialize our data.
CThread::CThread() 
{
	// flag our thread NOT to terminate ... that would be bad
	terminateThreadFlag = false;

	// zero the handle to our thread
	threadHandle = NULL;
}

// Now on to begin and end.  In the preceding tutorials we just told our threads to loop a certain
// number of times.  In practice, we typically want our threads to do something until we tell it
// to stop.  This is what we will use our terminateThreadFlag for.  We will have a loop in our
// threadProc that will check this flag, and if it is true, will exit the loop.  It should look something
// like this:

// aSampleThreadProc () {
//
//	while(terminateThreadFlag == false) {
//		do some processing
//	}
// }

// So the end function is easy.  All we need to do is set our terminateThreadFlag and our procedure
// should automatically exit at the end of its process loop.

void CThread::end() 
{
	// Set our terminate flag and let our threadProc exit naturally
	terminateThreadFlag = true;

	// Why don't we just wait here until our function finishes before we continue processing.
	// Note: Our end() function will stall the calling thread until this thread finishes executing.

	// Remember our waitForSingleObject function?  Let's use it here to wait for our thread to finish.
	WaitForSingleObject(threadHandle, INFINITE);
}

// While ending a thread is simple ... beginning one isn't quite as easy.  We have a small problem
// to get around.  Windows likes a threadProc to be a NON-class member function.  That means
// that windows will not allow CThread::threadProc() to be the procedure we pass to CreateThread().
// Obviously this is a problem, since we want to create a class to have the procedure as a member.
// Well there is a little trick we can implore to get around the problem.  We are going to trick windows
// into executing our threadProc.  We are going to create a normal C++ function that will accept a
// CThread pointer as an argument, then we will run that threads threadProc.

void WindowsThreadFunction(CThread* theThread) 
{
	// execute the thead procedure
	theThread->threadProc();
}

// Well how does this get around our problem???  It's pretty simple once you think about it.  Since
// windows won't let us make a member function a ThreadProc in CreateThread(), we will just
// call this function instead.  Only this function will execute the threadProc function of the thread
// we just created.  It may make more sense after we see begin(), so let's get to that now.

void CThread::begin() 
{
	// You remember in a previous tutorial how we passed a variable to threadProc in the CreateThread
	// function?  Well, we are going to do the same thing here.  Only instead of just passing the
	// address of an integer, we are going to pass a pointer to the thread we are starting.

	// Create the thread ... notice the "this" parameter in the forth argument.
	threadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) WindowsThreadFunction, this, NULL, NULL);
}

// When we call CThread.begin() we will create a thread that will execute the WindowsThreadFunction
// that we wrote.  This will, in turn, call our threads threadProc.  Now does it all make sense?

// Well ... now that all the pieces are there ... how do we actually USE this darn thing?  First things first,
// we have to define a thread with a threadProc.  Let's do something simple.  Let's make a thread that
// prints out numbers until we tell it to stop.

class CPrintSomethingThread : public CThread 
{
public:

	// This is the number that we will print
	int numberToPrint;

	// Our threadProc
	void threadProc() {

		// Print a quick statement so we know our thread started
		cout << "Thread Started\n";

		// Start printing at 0
		numberToPrint = 0;

		// Until we are told to quit
		while (!terminateThreadFlag) {

			// Print out the numberToPrint and add one to it
			cout << numberToPrint++ << endl;

			// Sleep a little bit so we can observe our thread working
			Sleep(100);
		}

		// Print a quick note so we know our thread ended
		cout << "Thread Exiting\n";
	}
};

// Now that we have defined our thread, lets actually put it to work.  Lets make a thread
// object and tell it to begin.  This will make our thread print forever ... so we better have 
// a way to stop it when we are through.  Lets make our main thread wait for a character
// from the keyboard, and once a key is pressed, we will tell our thread to end.  If we did,
// everything right, our main thread should stall at that point until our thread finishes
// executing.  Then we will be allowed to continue with the rest of our program.

int main() 
{
	// A friendly welcome message
	cout << "A Simple Thread Class by redKlyde\n";

	// Let's make an instance of our thread object
	CPrintSomethingThread theThread;

	// Tell the thread to begin
	theThread.begin();

	// Wait until we press a key
	_kbhit();

	// Then tell our thread to stop
	theThread.end();

	// Wait for you to exit the program
	system("pause");
	return 0;
}

// Pretty simple to use huh?  Thank goodness we will never have to write another CreateThread
// again.  All we have to do is derive a thread with a new threadProc and call begin()
// and our class will do all the work for us.  It makes things nice and simple to use and makes
// your program so that no one can come along and mess things up if they don't know what is
// going on (That's nice when working on a project with lots of people).

// Well folks, that's all she wrote.  I hope you have enjoyed reading and using these tutorials
// as much as I have enjoyed writing them.  It has been fun and I can't wait to see in what
// interesting ways this thread class will be used.  There is plenty more you can do to this thread
// class to make it even easier to use or more versatile.  Remember there is no one right way to
// get something done, and threads are no exception.  It all depends on how you like to program, 
// and how you want things to work.  Just to give you a few options when experimenting with your
// own thread class and trying to make things work the way you like them, try making the threadProc
// not have to contain a loop.  Let the thread class maintain the loop because what if some programmer
// forgot to check the terminateThreadFlag and exit his loop?  Well, there would be no way to terminate
// the thread would there?  If you made the programmer write a function that just performed ONE
// frame of the process loop, then you could keep the loop hidden from him and check the terminate flag
// yourself couldn't you?  Another option is to not have the threadProc a virtual function which makes the
// programmer derive a new class for every thread he wants to create, but instead pass in a pointer
// to a function in the begin() function.  If you choose this route, you never have to derive a new thread,
// you simply assign the thredProc and let it run.  The options are endless, it is just up to you to decide what
// access you want other programmers to have over your thread, and how you want your thread class
// to be used.

// Thanks to all for reading, and happy programming!!
// - redKlyde

// © 2000-2005 GameTutorials
