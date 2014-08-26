/****************************************************
	Introduction

****************************************************/

// It seems there is always a wealth of questions about where to get started with threading.
// It also seems that there are somewhat limited resources to get one educated and up to speed
// with threading.  So, without further ado, I present my own attempt at a threading tutorial.
// Like usual there will be a few parts to the complete set and you can study them thoroughly
// or just skim through them for the concepts.  In the final tutorial, I will present a simple 
// thread class that should function for most your needs.  If you so chose, you can skip right
// to the last tutorial and begin using the thread class immediately.  In any case, I hope
// these tutorials will be of good use.

// As always, thanks for reading, questions and comments are welcomed at redklyde@gametutorials.com

// Sincerely,
// redKlyde

/*****************************************************
	Intro to Windows Threading

	Author: redKlyde
	File: main.cpp
	Description: a simple introduction to windows threading

*****************************************************/

// Things that will be covered in this tutorial:
// - What is a thread?
// - CreateThread()
// - WaitForSingleObject()

// So what is a thread really?  Well, if you know much about how your computer works, you will 
// know that your computer runs "processes".  A process is nothing more than a related set of 
// instructions.  Generally speaking, there is a process behind everything.  There is a process
// that monitors the keys you type; there is a process that watches your mouse; there is a
// process that displays what you see on your screen; everything your computer does is handled
// by some process.  Even the executables we program and run are considered processes.  When
// you double click on that .exe your computer creates a new process and runs the exe's instructions in
// that process.  This way your computer can keep track of what application is doing what on the machine,
// and it can coordinate the process with the hardware.  After all, your computer can only execute one
// instruction at a time right?  Well how then can you run more than one program at a time?  That
// is what the processes are for.  Your computer manages the processes allowing them each time for
// execution.  So technically, you can have 2 programs (2 processes) running at the same time, each
// doing some independent work.  This "parallel processing" is what we attempt to do with threads.
// We have one process that will create another process so that both can execute at the same time.
// We call this separate process a thread.  (Note: there is a fundamental difference between what your
// OS refers to as a "process" and what it refers to as a "thread", but for our purposes there need
// not be a distinction.)

// Now that we know what a thread is, what on earth are threads good for?  Well threads can be used
// for a variety or applications.  In general computing they can be used for many things.  I saw a post
// on the gametutorials.com forum where someone had an application that was doing some intense
// image computations.  Any time his app would go to perform image operations, his app would halt
// until the image processing was complete.  Obviously, this is not the desired effect because we might
// like to be able to do other things like resize or move the window while the image is being processed.
// But these things can be severely hindered if our process is tied up in image handling.  Why not start
// the image handling in a thread, this way your application can continue doing what it needs to do while
// the image is being processed?  Sounds like a good idea to me.  In game development we can use
// threads in a few different ways.  One important use is networking.  If you thumb your way through the
// networking tutorials, you will see a use for threads where our app must stop and wait for incoming
// messages, but this may be a millisecond, it may be a year!  We can't have our game stop process while
// it waits for messages!  So we use threads to get around this.  Some interesting, yet challenging 
// enhancements can be made with threads IF done well.  I have seen games that use rendering threads
// which, depending on the architecture, can greatly improve frame rate.  I have seen games spawn ("spawning
// a thread " is a typical term for creating a thread) threads to handle certain AI aspects.  You can use threading
// for just about anything.  You must be careful though, an overuse or misuse of threads can actually hinder
// your game rather than help it.  As a rule of thumb, use threads when you have a process with a high
// probability of stalling more important process.  Don't use threads just because you can.  Use them with 
// purpose.  You will stay out of a lot of trouble that way.  If anything, write it without threads first, then 
// keep that version and write it with threads.  Then compare the two and see which performs better.  I 
// think you will save yourself a lot of heartache if you don't try and use threads where you don't have to.

// Well enough of my sauntering, let's get to the code.  In this example we are just going to create a couple
// of threads; they won't do anything but print out a message when the thread executes.  Oh yes ... before
// I continue, let's make one thing clear.  We will consider the main() function a thread.  We will often refer
// to it as the "main thread".  So if you hear that term thrown about ... know that we didn't do any special
// work to create the main thread, this is the thread that was started when we ran our program.  So technically
// our program always creates one thread by default.

// First things first ... include the libraries we will use.  Let's grab iostream for cout, and we will need windows.h
// for all our threading routines.
#include <iostream>
#include <windows.h>

using namespace std;

// Now how are we going to actually create the thread?  We will have to use some windows OS functions
// to do this.  Namely, we will use a handy function CreateThread().  For a full definition please refer to
// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/createthread.asp.
// This will tell you everything you would ever care to know, however all the function arguments are not
// that important to us right now.  The main one we care about is the third parameter.  In fact ... when we
// call this, all the other parameters will be 0, or NULL.  The third parameter is the, as referred to in the windows
// definition, the ThreadProc or the "thread procedure".  As is suggested by the name, this is the procedure
// (or function) that will be executed when the thread is created.  This procedure is simply a function that we
// write in our program.  So wait ... does that mean that when we create a thread, we are just telling our
// program to execute a function?  BINGO!  Essentially, creating a thread is the same thing as calling a function,
// only we are telling this function to run at the same time as our other functions.

// Well, in that case, before we can begin a thread, we have to have a function to execute.  So let's make our
// threadProc.  All we need is a function ... let's just print something to get started.

// the procedure our thread will run
void threadProc() 
{
	cout << "HAHAH!!!! This is me ... the thread ... I am running!!\n";
}

// Ok, now we have a thread procedure and we know we need to use CreateThread to create the thread.
// Let's put the pieces together.


// the main thread
int main() 
{
	// a friendly welcome message
	cout << "Intro to Windows Threading by redKlyde\n";

	// Now create my thread and execute my threadProc function.
	HANDLE threadHandle;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadProc, NULL, 0, NULL);

	// CreateThread will return you a HANDLE.  This thread handle is nothing more than an identifier for your
	// computer to reference your thread by.  We will use this handle shortly.

	// Tada!!  We have just created our first thread.

	// There is one more function I want to cover now, and that is WaitForSingleObject().  WaitForSingleObject()
	// is a handy function that will wait for an object to finish what it was doing before continuing to execute.   
	// Its definition is as follows:

	// WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
	//   hHandle -  the handle of the object we want to wait for (in this case we will use our threadHandle)
	//   dwMilliseconds - the maximum number of milliseconds we will wait for the object to finish before
	//                    we continue execution.  (In this case we will wait forever)

	// In other words ... WaitForSingleObject will stall our main thread until threadProc returns.

	// Let's wait until my thread is done executing then continue ... more explanation at the end of the tutorial
	WaitForSingleObject(threadHandle, INFINITE);

	// Once our thread is done ... you tell me when to exit
	system("pause");
	
	// Return zero for "zero problems in this program"
	return 0;					
}

// And we are finished.  We created a thread from our main thread, waited for the thread to finish executing
// and then exited our program.  Why did we go through the trouble of calling WaitForSingleObject though?
// It would seem that if we have started our thread, it will just execute regardless of whether or not we wait
// for it to finish in our main thread.  This is true.  However, when you run this program, do you notice a slight
// pause before you see the threadProc print its message?  This is because it takes a little time for our computer
// to create the thread and begin executing it.  There is no way to tell how long it could take to create the thread.
// It could be almost immediate if we aren't allocating a lot of data and our computer isn't very busy.  But then
// again it could take a few seconds if we are doing something that is really intense.  If we were to just let our
// main program start the new thread and then immediately exit ... we would never get to see the threads message.
// This is because our main thread is exiting before the theadProc even got started!  As an experiment, go comment
// out the WaitForSingleObject() function.  What happens?  Did the thread's message appear AFTER the "press any key
// to continue" message?  Chances are it did.  This is because your computer can execute the main thread's code
// much faster than the new thread can be created.  Here we use WaitForSingleObject mainly as an introduction
// to how this all works.  We will see much more practical uses for WaitForSingleObject in the future.  You will
// see that in some cases, we couldn't get by without it.

// Well, that brings "Intro to Windows Threading" to a close.  Let's quickly recap before moving on.

// We have learned what a thread is, and what a thread is useful for.  We have learned how to create a
// thread using a procedure we created ourselves.  And we learned how to reference a thread using the
// handle we received when the thread was created.  In the next tutorial, we will learn about Mutex's and 
// why we must use them to protect certain data from corruption.

// © 2000-2005 GameTutorials
