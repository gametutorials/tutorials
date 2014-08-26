/****************************************************
	Networking Tutorials #5
	Multiple Hosts - ServerSide
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// -handling connections from more than 1 host
// -using select()
// -using file descriptor sets

// A quick note on threading:
// In order to accomplish the goals of this tutorial, it will be necessary to do some threading.
// However, since it is not the aim of this tutorial to give an in depth education on threading,
// we will have to do a little magical hand-waving at some point in the threading discussion.
// Even though we aren't going to TEACH threading in this tutorial, we should still cover the 
// basics so we have a clear understanding of what is going on and why, so here we go ...
// For a more in-depth look at threading, check out our threading tutorial series in the C++ section.

// Why should we have threads in the first place?  Well there are 2 basic types of functions
// when we talk about networking.  They are either blocking or non-blocking.  A blocking function is
// one that will halt the program until the function completes, or "blocking" the program from
// continuing.  A non-blocking function is one that will NOT halt the program, but
// let it run while the function completes.  This information is important to some of our functions,
// in particular, the accept() function.  When we call accept(), the program will wait inside the
// accept() function until it receives a connection from a user.  Obviously, if we want to send and
// receive information from users, we can't do this if our program is waiting to accept a connection, so
// we have to make a thread to accept connections for us, while we run the program and communicate
// with the users.  We have a similar problem with the recv() function.  The recv() function will ALSO
// block until it receives data from the specified socket.  This is bad because what if we read from
// a socket that hasn't sent any data?  Well, we will have to wait until data is sent before we can
// continue with the rest of the program.  Of course, we don't want to do this, especially when
// we are making a game, we need to send and receive data very quickly so we can process our game
// objects.  This was not a problem in our earlier examples because the system of sending and receiving
// was very well defined.  From the client we ALWAYS sent our message and then received a confirmation
// from the server.  On the server, we ALWAYS received a message from the client and then sent
// our confirmation.  Not only that, but we only had one client, so it didn't matter when the client sent
// the data, there wasn't another client waiting for service.

// How on earth are we going to fix these problems?  Well, with a couple different solutions.  Keep in
// mind that again, this is not the ONLY way to get around these issues, but it is a simple, and effective
// way to create a server application.  The first thing we are going to do is create a thread to
// accept connections.  The second thing we will do is utilize the select() function to find out which
// sockets already have data on them.  This way we will not block while we wait to receive data
// with recv().  Instead, the data will already be there.

// Let us talk about threading first.
// What is a thread anyway?  In simple terms, a thread is a function that we can run at the same time
// as another function.  What we will do is create a procedure (aka. a function) that will handle accepting
// new connections, run this function as a thread (so it happens at the same time, or in other terms, so it
// runs asynchronously), and our main program will handle sending and receiving to all the clients that
// our thread accepts.

// Now on to select().
// How does select() work to handle the blocking recv() function?  What select() will do is create for us 
// a list of sockets that already have data to read.  We will have to create a "socket set" that has
// all our client sockets, and "select" from that set all the sockets with data ready.  This way, when we
// call recv(), we are guaranteed that we will receive data immediately, and we will not have to block
// until data is sent.

// That's probably enough explanation for now, lets dig into the code.  We start as usual with all the
// proper setup.

// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>

// -----------------------------------------------------------------------------------
// startupServerForListening() - will return us a socket that is bound to the
// port we specify, and is listening for connections if all operations succeeded,
// and a -1 if startup failed.
int startupServerForListening(unsigned short port);

// -----------------------------------------------------------------------------------
// shutdownServer() - will take the socket we specify and shutdown the 
// network utilities we started with startupServerForListening()
// Note: In order to function properly, the socket passed in MUST be the
// socket created by the startupServerForListening() function.
void shutdownServer(int socket);

// Note: When programming multi-threaded applications, it is necessary to tell the compiler that we
// are using more than one thread.  This is so the compiler can chose certain functions that are said
// to be "thread-safe".  Some functions that we commonly use are not designed to work with more than
// one thread.  Calling these functions would cause some unexpected results, or even some serious
// problems.  So be certain to always set the libraries to be thread-safe.  Do this by clicking Project/Settings.
// Go to the C++ Tab.  Change the category to "Code Generation".  Change the "Use run-time library"
// drop down box to "Debug Multithreaded", and you are ready to go.

// Normally, after our definitions and includes we would jump right into the main() and start setting
// up the network, but this time we will explore the acceptance thread first, since it is the most new
// concept to our architecture.

// First some definitions.

// For threading, we will need a thread handle.  We will receive this handle when we create the thread
// and can use this handle any time we need to reference our thread.
HANDLE threadHandle;

// The beauty of threading is that both of our threads will have access to the same memory locations.
// However, this is dangerous because we could both potentially attempt to read or write to that memory 
// at the same time!  So we need some way to protect that memory so that only one thread at a time
// can control it.  For this, we need a mutex. A mutex is a structure we can lock and unlock any time
// we write to a shared section of memory.  No other thread can lock our mutex while we have it locked,
// and our thread will block until we gain access to the mutex.

// A mutex for my shared data
HANDLE mutexHandle;

// Finally, the file descriptor set (FD_SET).  A FD_SET is simply our list of sockets.

// My master socket set that will be protected by my mutex
FD_SET masterSet;

// A FD_SET has data members:
// - fd_count - the number of file descriptors in the set
// - fd_array - the array of file descriptors

// There are a few convenient macros we can use to manipulate the set.
// - FD_SET(a socket, FD_SET to manipulate) - This macro will add a socket to a set
// - FD_ZERO(FD_SET to manipulate) - This macro will zero a set
// - FD_CLR(a socket, FD_SET to manipulate) - This macro will remove a socket from a set
// - FD_ISSET(a socket, FD_SET to manipulate) - This macro will check to see if a socket is in a set

// A quit flag I will use to exit my program with if something goes wrong
bool gQuitFlag = false;

// Now that we have everything defined that we will need, let's look at the acceptance thread procedure.
// The sole responsibility of this function is to accept connections from our clients, and add them to the
// masterSet.  This way our main thread can be free to send and receive data without blocking during
// the accept call.

// -----------------------------------------------------------------------------------
// acceptingThreadProcedure() - a procedure that will accept clients and add their
// sockets to the master file descriptor set
void acceptingThreadProcedure(int* serverSocket) 
{
	// Copy my socket over to a local variable
	int mySocket = *serverSocket;

	// Run forever
	for (;;)
	{
		// Accept a client like normal
		unsigned int clientSocket = accept(mySocket, 0, 0);

		// Make sure things are ok
		if (clientSocket == SOCKET_ERROR) 
		{
			// Stop of we received any errors
			printf("Accept Failed!\n");

			// Signal to quit the application
			gQuitFlag = true;

			// Quit this thread
			return;
		}
		else 
		{
			// Everything went well.  We need to take the client socket we received, and add
			// it to our master socket set so the other thread can use it.  Remember though,
			// since our masterSet can be accessed by both threads, we need to make sure
			// that we are not trying to write to it in this thread while at the same time we are trying
			// to read from it in the other.  We need to lock our mutex.  We do this with the function
			// WaitForSingleObject().  We need to give WaitForSingleObject() our mutex handle
			// and a time value to wait.  We will use INFINITE for the time value because there
			// really isn't anything else our thread needs to do.

			// Lock the mutex
			WaitForSingleObject(mutexHandle, INFINITE);

			// Add this socket to the master set using our FD_SET() macro
			FD_SET(clientSocket, &masterSet);

			// Now we have to unlock our mutex so that our main thread can also access the data.
			// To be absolutely clear on this, here is how it works.  When we call WaitForSingleObject()
			// we have locked the data.  We have control of the mutex until we call ReleaseMutex().
			// If the main thread were to call WaitForSingleObject() before we call ReleaseMutex(),
			// WaitForSingleObject() will block until we unlock the mutex.  At that point, WaitForSingleObject()
			// would immediately get a lock on the mutex, and the main thread would be able to continue.
			// So always lock the mutex, write/read the data, and then unlock the mutex.  This will make
			// everyone trying to access the data very happy.

			// Unlock the mutex
			ReleaseMutex(mutexHandle);

			// A quick message
			printf("client on %d connected\n", clientSocket);
		}
	}
}

// Well, that's all our acceptance thread needs to do.  Once we start the thread, we will begin accepting
// clients and adding them to the masterSet.

// Now to write our main(), we will need to startup the network like normal, start our acceptance thread
// and then send and receive data from our clients.  Since we are servicing multiple clients, we will have
// to create a loop and continuously check our network.

int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 5 : Multiple Hosts - ServerSide\n\n");

	// Startup our network as usual.

	// The socket my server will use for listening
	int serverSocket;

	// Startup my network utilities with my handy functions
	serverSocket = startupServerForListening(7654);

	// Check for errors
	if (serverSocket == -1) 
	{
		printf("Network Startup Failed!\nProgram Terminating\n");
		return 1;
	}

	// And now ... the magical hand waving act.  It would be great if this were a REAL threading tutorial,
	// but unfortunately it isn't.  So, at this point we will just have to say that this stuff works.  We will
	// create our mutex and our thread and receive handles for both.

	// Create the mutex
	mutexHandle = CreateMutex(NULL, false, NULL);

	if (mutexHandle == NULL) 
	{
		printf("Error creating mutex\n");
		shutdownServer(serverSocket);
		return 1;
	}

	// Create the thread
	int threadId;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)acceptingThreadProcedure, &serverSocket, 0, (LPDWORD)&threadId);
	
	if (threadHandle == NULL) 
	{
		printf("Could not start acceptance thread\n");
		shutdownServer(serverSocket);
		return 1;
	}

	// Sleep the main() so the acceptance thread has time to start ... this is cheesy
	Sleep(100);

	// Now that that is all over with, it's down to business.

	// It's always a good idea to initialize our structures before we access them, so let's zero our masterSet.
	FD_ZERO(&masterSet);

	// The main loop ... run forever
	for (;;) 
	{
		if (gQuitFlag) 
		{
			break;
		}

		// Now the fun part.  At this point we have our masterSet that contains a list of all our client sockets.
		// We need to look in that set for clients that have data, and then read their data.  This means we have
		// to access our masterSet that is, again, ALSO used by our acceptance thread.  This means we have
		// to use our mutex, so let's get a lock on our mutex so we can properly access masterSet.

		// Lock the mutex
		WaitForSingleObject(mutexHandle, INFINITE);

		// Instead of reading directly from masterSet we are going to make a copy of it instead.
		// This will keep us from having to lock the mutex for the entire duration of the loop.  Remember,
		// as long as we are accessing masterSet, we need to keep the data locked.  We will need a temporary
		// set to use with the select call.  This will be called the polling set.

		// Make the polling set and copy everything from masterSet
		FD_SET pollingSet = masterSet;

		// Unlock the mutex
		ReleaseMutex(mutexHandle);

		// Here is an interesting scenario.  If we were to select() from a set with count 0, we would actually
		// generate an error.  We could use WSAGetLastError() and handle the error properly, but it is easier
		// to avoid the situation entirely.  We can do this by NOT calling select() if the set was empty.  So check
		// the count, and just skip the select if our set is empty.

		// Check if our set is empty
		if (pollingSet.fd_count == 0) 
		{
			continue;
		}

		// Now it is time to use select().  We have a set that contains all of our sockets, and that set is not empty.
		// Select() will actually alter the set we give it, and remove any sockets that do NOT have data ready. For
		// that reason, we always want to use the copy of the master set when we use select.  If not, we will lose
		// all the sockets that did not have data read, which is never a good thing.

		// select() has 5 parameters.  They are:
		// - the number of file descriptors
		// - a FD_SET to check for readability
		// - a FD_SET to check for writeability
		// - a FD_SET to check for errors
		// - a wait time

		// The wait time is a timeval structure.  You can set the number of seconds and microseconds for select
		// to wait before it times out.  This can be useful in specialized circumstances, but we will just make it 0
		// so it times out immediately and returns.

		// The wait time
		timeval waitTime;
		waitTime.tv_sec = 0;
		waitTime.tv_usec = 0;

		// Let's select from the polling set using fd_count as the number of sockets.  We do not have a
		// write set nor an errors set, so just pass NULL for them.

		int result = select(pollingSet.fd_count, &pollingSet, NULL, NULL, &waitTime);

		// The return value for select() is the number of sockets that have data ready to read.  Like most our
		// networking function, if an error occurred, this value will be SOCKET_ERROR, otherwise it will indicate
		// success.

		// However, select() can return 0 if there was no data to be read.  In this case, just continue on with the
		// rest of the program.

		// Check for no sockets with data
		if (result == 0) 
		{
			// No sockets have data
			continue;
		}

		// Check for errors
		if (result == SOCKET_ERROR) 
		{
			printf("Error in select()\n");
			continue;
		}

		// Now that we have the polling set that contains just the sockets that have data, let's step through
		// the list of sockets and read from them all.

		// For every socket in my polling set
		for (unsigned int i = 0; i < pollingSet.fd_count; i++) 
		{
			// We can access the socket list directly using the fd_array member of the FD_SET
			unsigned int clientSocket = pollingSet.fd_array[i];

			// We will be using the same variable length data system that we implemented in tutorial #4.
			// So we need a few variables to facilitate the communication.

			// The number of bytes we received
			int nBytes;

			// A buffer to hold my data
			#define MAX_MESSAGE_SIZE 4096
			char buffer[MAX_MESSAGE_SIZE];

			// The size of the message that is being sent
			unsigned long messageSize;

			// Receive the message size first
			nBytes = recv(clientSocket, (char*)&messageSize, sizeof(messageSize), 0);

			// Check for errors
			if (nBytes == SOCKET_ERROR) 
			{
				// Uh Oh!  We have our first real use of error handling!  Something can happen here that is
				// pretty significant.  Let's grab the error number from winsock.
				int error = WSAGetLastError();

				// The error we got should be WSAECONNRESET.  This error says that the connection was either
				// closed or somehow reset from the other end.  That means that our client has shutdown his
				// application.  We have 2 real ways of dealing with closing connections.  One is to send a
				// disconnect message from the client to the server.  The other is to handle the dropped
				// connection error.  The disconnect message is something that we would have to write ourselves.
				// We would somehow make an identifier message that tells our server that we are disconnecting,
				// that way we can handle it properly.  That will work assuming that all our connections are closed
				// cleanly (like they are supposed to).  But what if someone just shuts down their application, or
				// looses power to their computer?  They can't send a message if their computer isn't on!
				// So we still need to check for the error condition.

				// Handle the dropped connection
				if (error == WSAECONNRESET) 
				{
					// When we receive this error.  Just get a lock on the master set, and remove this socket from
					// set using the FD_CLR() macro.

					// Lock our mutex
					WaitForSingleObject(mutexHandle, INFINITE);

					// Remove the socket from our master set
					FD_CLR(clientSocket, &masterSet);

					// Unlock our mutex
					ReleaseMutex(mutexHandle);

					// Close the socket on our side so our computer cleans up properly
					closesocket(clientSocket);

					// A quick message
					printf("client on %d disconnected\n", clientSocket);

					// Move on to the next client
					continue;
				} 
				else 
				{
					// We failed, but it wasn't an error we were expecting ... so kill the server
					printf("Recv Failed!\n");
					gQuitFlag = true;
					break;
				}
			}

			// We have already handled the unexpected disconnect from a client, but what about the first case we
			// talked about, the "clean disconnect"?  Well, in the case of the clean disconnect (the client 
			// called closesocket()), we will NOT generate an error.  Instead when we read from the socket, it will 
			// return us 0 bytes.  When this happens we should simply handle it just like we did the disconnect, 
			// remove the socket from our master set, then continue.

			if (nBytes == 0) 
			{
				// Lock our mutex
				WaitForSingleObject(mutexHandle, INFINITE);

				// Remove the socket from our master set
				FD_CLR(clientSocket, &masterSet);

				// Unlock our mutex
				ReleaseMutex(mutexHandle);

				// Close the socket on our side, so our computer cleans up properly
				closesocket(clientSocket);

				// A quick message
				printf("client on %d disconnected\n", clientSocket);

				// Move on to the next client
				continue;
			}

			// At this point, the rest is just like tutorial #4.  Just receive the rest of the message and print it out.

			// Convert the message size to host ordering
			messageSize = ntohl(messageSize);

			// Receive the rest of the message
			nBytes = recv(clientSocket, buffer, messageSize, 0);

			// Check for error
			if (nBytes == SOCKET_ERROR) 
			{
				printf("Recv Failed!\n");
				gQuitFlag = true;
				break;
			}

			// Remember this is a string, so terminate the buffer so we can print it
			buffer[messageSize] = '\0';

			// Print the message
			printf("Message Received from client on %d : %s\n", clientSocket, buffer);
		}
	}

	// Cleanup
	shutdownServer(serverSocket);

	printf("Press Enter to Exit ...\n");
	getchar();
	
	return 0;

	// That's just about it.  You now have all the tools you need to handle connections from multiple
	// hosts and a send and receive data between them and the server.  This also concludes the basic
	// networking tutorials.  Part 2 of this tutorial does not cover any new material.  It has some mild
	// changes to allow clients to send information they type in, but no new material is covered.  So, 
	// browse through it if you like; otherwise, I hope these tutorials have been informative enough about
	// networking and helpful enough with code examples that you may have many joys in experimenting,
	// and a little less pains than the rest of us.  Thanks for enduring my ramblings and best of luck!
	// - redklyde
}


// -----------------------------------------------------------------------------------
// startupServerForListening() - a function to startup winsock, and open a socket for listening

int startupServerForListening(unsigned short port) 
{
	// An error code we will use to get more information about our errors
	int error;

	// The winsock data structure
	WSAData wsaData;

	// Startup winsock
	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR) 
	{
		printf("Could Not Start Up Winsock!\n");
		return -1;
	}

	// Create my socket
	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	// Make sure nothing bad happened
	if (mySocket == SOCKET_ERROR) 
	{
		printf("Error Opening Socket!\n");
		return -1;
	}

	// The address structure
	struct sockaddr_in server;

	// Fill the address structure with appropriate data
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	// Now bind my socket
	if (bind(mySocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) 
	{
		printf("Bind Failed!\n");
		closesocket(mySocket);
		return -1;
	}

	// Mark my socket for listening
	if (listen(mySocket, 5) == SOCKET_ERROR) 
	{
		printf("Listen Failed!\n");
		closesocket(mySocket);
		return -1;
	}

	printf("Server Started\n");

	return mySocket;
}


// -----------------------------------------------------------------------------------
// shutdownServer() - a function to shutdown a socket and clean up winsock

void shutdownServer(int socket) 
{

	// Kill my thread and my handle
	WaitForSingleObject(threadHandle, INFINITE);
	CloseHandle(threadHandle);
	CloseHandle(mutexHandle);

	// Close our socket
	closesocket(socket);

	// Shut down winsock
	WSACleanup();

	printf("Server Shutdown\n");
}


/*----------------------------*\
|  Done by:  redKylde		   |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
