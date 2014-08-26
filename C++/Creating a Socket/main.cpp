/****************************************************
	Introduction

****************************************************/

// Welcome to redKlyde's Networking Tutorials.  These tutorials are designed to give you a basic
// understanding of network programming, while at the same time provide useful code
// examples that give a starting point for network games.  These tutorials are not designed to make
// expert network programmers within a few lessons.  They are not designed to give an in-depth
// education on networking principals and applications.  But they should provide you with enough to
// start experimenting on your own.

// Before we even look at code, we need to discuss what we are trying to accomplish.  Obviously,
// we are trying to get a bunch of computers to "talk" to one another, but what does that mean?  Well,
// you will often hear network programming referred to as "socket" programming.  This is no surprise
// because everything we are about to discuss revolves around the use of a "socket", but without describing
// what a "socket" is, it would be somewhat unclear what is going on when we starting making and using them.

// Simply put, a socket is an identifier for your computer to use when sending and receiving data.  For those
// of you who are familiar with Unix, sockets in Windows behave similarly to sockets in Unix.  These tutorials,
// however, are geared toward the Windows programmer.  So, if you are familiar with Unix socket, aka. BSD
// sockets, you may just use these tutorials as a reference for the Windows specific differences.

// What we do in network programming is create sockets using Windows' networking API called "Winsock",
// and then use some other handy function in the Winsock library to send and receive data on that socket.

// To cover all the concepts we need to, and keep the tutorials progressing slowly and steadily, they will
// be broken down into 5 different tutorials.  They are titled as follows:

// 1. Getting Started
// 2. Making the Connection
// 3. Sending and Receiving
// 4. Flexible Sending
// 5. Multiple Hosts

// Due to the nature of networking, starting with tutorial #2 "Making the Connection", we will break the
// tutorials down even further into 2 parts, a "ServerSide" and a "ClientSide", more elaboration will be done
// on these parts when the time comes.

// Well, that just about sums up the plan for things.  Lets jump into the first tutorial "Getting Started".
// Thanks for reading, and as always, comments and suggestions are welcomed at redklyde@gametutorials.com.

// Sincerely,
// redKlyde

/****************************************************
	Networking Tutorials #1
	Getting Started
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// - Adding winsock to a project
// - Starting up and shutting down winsock
// - Opening and closing sockets

// First things first - we have to setup a project with the appropriate libraries
// for networking.  These tutorials will use winsock, windows networking API.
// After creating a new project, go to Project/Settings/, click the "Link" tab
// and add WS2_32.LIB to the Object/Library Modules list.  Click "Ok" and
// you should be ready to go ...

// All the networking information is included in one library.  The header you need
// to include is winsock2.h  Let's do that now to get started.

#include <winsock2.h>

// Some other libraries I will use
#include <stdio.h> // for I/O

// Link with the needed library
#pragma comment(lib, "ws2_32.lib")

int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 1 : Getting Started\n\n");

	// Now that we have winsock included, winsock must be initialized before we can
	// use any networking functions.  You do this with WSAStartup()

	// We need a couple variables first.

	// All winsock functions return an error code, so we will use this to get more information
	// about our errors.
	int error;

	// And a winSock data structure that will hold information about our network
	// capabilities.  We will not, however, use any of the information in it.  It 
	// is included simply for completeness.
	WSAData wsaData;

	// And the call to start up winSock
	error = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Before we get going too fast, have no fear about the MAKEWORD macro.  It is simply used so winsock can
	// identify what version of winsock we are trying to use.  In this case, we are using v2.2.
	// You can research more if you would like to inquire about earlier versions of winsock and/or what their differences
	// with v2.2 are if you chose to implement them.

	// Now the nice thing about winsock is the way it handles return values and errors.
	// Virtually EVERY winsock function will return SOCKET_ERROR if an error occurred, and something else
	// if it was successful.  This makes error checking quite simple.

	// So lets check for an error.
	if (error == SOCKET_ERROR) 
	{
		printf("Could Not Start Up Winsock!\n");
		return 1;
	}

	printf("WinSocket started\n");

	// "But..." you may ask, "now do I know what the error was if all I get is SOCKET_ERROR?".  That is where
	// this next little function comes in handy.  WSAGetLastError().  WSAGetLastError() will return you the error code
	// of the last error winsock logged.  Then you can examine what the error was, and deal with it accordingly.  For a
	// list of winsock error codes, look on msdn.microsoft.com, and search for "windows sockets error codes", this should
	// get you a list of codes, and what they mean when you encounter them.

	// To demonstrate WSAGetLastError() let's try something that will cause an error.

	error = send(0, 0, 0, 0);

	if (error == SOCKET_ERROR)
	{
		printf("ERROR OCCURED!\n");

		// Get the last error that occurred
		error = WSAGetLastError();

		// I bet I know what it was
		if (error == WSAENOTSOCK) 
		{
			printf("aah WSAENOTSOCK\nThis error says that send() tried to operate on something that wasn't a socket!\n");
		}
	}

	// Do not worry right now about the arguments of send(), nor why it failed.  We will get to sending
	// and receiving in a later tutorial.  It was used here simply to generate an error.

	// In case the concept of a socket is still fuzzy at this point, let's elaborate a little.
	// A good way to think of a socket is something like a file.  Only, a file typically refers to some
	// place on our disk.  A socket, on the other hand, refers to something out on the network.
	// For those of you familiar with BSD sockets, the syntax between windows sockets and BSD sockets
	// are largely similar.  In FACT, windows sockets were structured on BSD sockets so they would be
	// as closely compatible as possible.

	// Now lets create a socket so we can communicate with another computer!

	// This is the socket I am going to use.
	int mySocket;

	// And create the socket
	mySocket = socket(AF_INET, SOCK_STREAM, 0);

	// Once again, we see some things that are unfamiliar.  What is AF_INET and SOCK_STREAM?
	// AF_INET is the "family".  It tells the computer to create a socket for the Internet.  Sockets
	// operate much like files do, and we COULD use a different family to define a different kind of
	// communication, but since we are doing networking, we want to use the AF_INET family.

	// SOCK_STREAM is the type of Internet socket this is.  SOCK_STREAM tells Winsock this is a TCP socket.
	// For now we will not get into the specifics of TCP, this will also be covered in a later tutorial.  
	// An alternate type would commonly be SOCK_DGRAM, which is a UDP socket.  UDP however, is not within 
	// the scope of these tutorials.  We will primarily use TCP.

	// The third argument 0 is an additional qualifier, which we will NOT use for Internet communication, so
	// we pass 0 for completeness.

	// Better check and make sure nothing bad happened
	if (mySocket == SOCKET_ERROR) 
	{
		printf("Error Opening Socket!\n");
	}

	// A good question at this point might be, "What errors can I get from creating a socket?" or "Why would
	// socket() ever fail?".  Exact answers to these questions depend on the platform they are being run on.
	// This question is something like asking, "Can I ever open TOO many windows on my computer?".  This
	// answer is literally yes, it IS possible for you to open too many windows.  But would you EVER in practice
	// actually open up too many windows for your computer?  That answer is no, so it is something you don't
	// ever worry about when you program windows.  The same applies to our sockets.  If for some reason
	// our operating system could NOT open a socket for us to communicate with, it is pretty safe to assume
	// that this is not anything you have done or can possibly solve with code.  So just abort your application
	// and let the user fix the problem himself.  You shouldn't run into this problem unless you are attempting
	// to do something strange, or somehow have socket() in a loop that is allocating all the sockets you computer
	// could create.  If you run into this problem ... YOU ARE DOING SOMETHING WRONG!  Look at what you are
	// doing and why.  There is likely a better solution.

	printf("Socket Opened!\n");

	// Well, we have created a socket!  Now would be time to do something with it.  These steps will be
	// built upon in the next tutorial.  For now, we have learned how to insert the winsock libraries into 
	// a project, start up winsock, and create a socket.  Now we must clean up our mess.

	// First this involves closing our socket.  Much like a file, we have opened our socket and now
	// we must close it.  There is one simple call to do this, closesocket().

	// Close our socket
	closesocket(mySocket);

	printf("Socket Closed\n");

	// Finally, we have to shut down winsock.  This MUST be done in order to clean up properly.
	// WSAStartup() loads a .dll ... so if we do not free that .dll, we have caused a memory leak,
	// which could eventually cause some problems.  We shutdown winsock with a call similar to
	// WSAStartup().  It is appropriately named WSACleanup().  This function requires
	// no arguments and only needs to be called once.

	// Shut down winsock
	WSACleanup();

	printf("WinSocket Shutdown\n");
	return 0;

	// Congratulations!  You are well on your way to becoming a network programmer.
	// The next few tutorials will cover connecting 2 computers, and sending and receiving data.
	// Good luck!
}

/*----------------------------*\
|  Done by:  redKylde		   |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/

