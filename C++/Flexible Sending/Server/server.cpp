/****************************************************
	Networking Tutorials #4
	Flexible Sending - ServerSide
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// sending and receiving messages of variable length from a client

// In the previous tutorial we finally got to transmit data between a client and a server, but
// we did this with some special knowledge.  We knew EXACTLY the size of the messages we
// were sending and receiving.  This was very convenient, but in practice we often (if not ALWAYS)
// send data of varying length.  In this tutorial we will explore one method of sending data of differing size.

// Let's startup our application like we normally would, and move on down to the part where we need to
// receive data.

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
// socket created by the startupServerForListening() function
void shutdownServer(int socket);


int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 4 : Flexible Sending - ServerSide\n\n");

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

	// Accept a client
	int clientSocket;
	clientSocket = accept(serverSocket, 0, 0);

	// Check for errors
	if (clientSocket == SOCKET_ERROR) 
	{
		printf("Accept Failed!\n");
	}

	// The number of bytes I send/read ... will also serve as my error code
	int nBytes;

	// Ok, we have got everything setup and are ready to send and receive some data, but how are we
	// going to do this?  Since we have covered the basics of sending and receiving, there is not much
	// need to revisit the specifics of both of them.  So for the server, we will focus directly with receiving
	// data, and with the client we will cover sending.  What we are more interested in is the details of
	// how we communicate the amount of data we are receiving, or in other words, the "size of the message".

	// Why must we go through all this trouble?  This is a good question, because it is not quite clear how we
	// could get our data mixed up if we don't.  Without getting too complicated: We must do this because of how TCP
	// works.  Remember the identifier we used to set up our TCP sockets, SOCK_STREAM.  Just as
	// the name implies, TCP works as a stream of data.  This means that there is no natural division between
	// one send() and another.  So if we sent a string "this is string one", and then send another string
	// "this is string two", they would be strung together in the network as "this is string onethis is string two".
	// There is no division between the two separate send() calls.  So if we wait a long time between
	// recv() calls, we may have a bunch of strings all bunched up together, and if we were to just read as much
	// as we could, we would not be able to tell where one ended and the next began.

	// There are multiple ways to deal with this problem.   One of them is to create an escape character in the
	// message that will identify and "end of transmission" point.  If we are just working with character data, a
	// newline ('\n') or a NULL ('\0') just might do the trick.  Another method is to notify the receiver of the amount
	// of data to expect.  We can do that by encoding the size with the actual message, or by what we are going to do.
	// When we send a message, we will first send the size of that message.  That way we can read the size of the
	// message and know how many more bytes to read out of the network.

	// We need a big enough buffer for our maximum message size.
	#define MAX_MESSAGE_SIZE 4096
	char buffer[MAX_MESSAGE_SIZE];

	// And we need a variable for our size.
	unsigned long messageSize;

	// Receive the message size from our client
	nBytes = recv(clientSocket, (char*)&messageSize, sizeof(messageSize), 0);

	// Check for errors
	if (nBytes == SOCKET_ERROR) 
	{
		printf("Recv Failed!\n");
	}

	// Remember to fix our byte ordering since this is a number
	messageSize = ntohl(messageSize);

	// This is a very important step.  Remember that every time we send a numerical value, we don't know
	// if the target system is a big-endian or little-endian system.  So if we just sent our data in our native endian
	// format, it could be received in the incorrect order and misinterpreted.  So be certain to fix your byte ordering
	// when both sending and receiving.

	// And now receive the rest of the message.
	nBytes = recv(clientSocket, buffer, messageSize, 0);

	// Check for errors
	if (nBytes == SOCKET_ERROR) 
	{
		printf("Recv Failed!\n");
	}

	// Now here is an interesting case.  We just read messageSize characters from our client, and stored
	// that as a string.  But what do we know about strings?  Strings are required to be NULL terminated
	// in order to work properly.  But what we have read does NOT include the terminator, so we must
	// terminate the buffer ourselves.

	buffer[messageSize] = '\0';

	// At this point we should have received data from clientSocket, and stored it in inMessage.
	// So lets print out our input buffer and see what the message was ...

	printf("Message Received : %s\n", buffer);

	// Congratulations, we have just received data of variable length!  We have just about everything we
	// need to write a real program to send useful data between 2 computers!  Wait a second .... 2 COMPUTERS!!
	// How useful is just connecting 2 computers?  Well, with this design, it is not very useful, but getting 2 
	// computers to communicate covers the basics we need for 1,000 computers to communicate.  In the next
	// tutorial we will cover methods to communicate with multiple computers at once.

	// But let us not forget the client side of this tutorial.  Let's clean up and then see how we send data to our server.

	// Close our client socket
	closesocket(clientSocket);

	// And shutdown our network
	shutdownServer(serverSocket);

	printf("Press any key to continue ...\n");
	getchar();
	
	return 0;
}


// -----------------------------------------------------------------------------------
// startupServerForListening() - a function to startup winsock, and open a socket for listening

int startupServerForListening(unsigned short port) 
{
	// An error code we will use to get more information about our errors
	int error;

	// The winsock data structure
	WSAData wsaData;

	// startup winsock
	if ((error = WSAStartup(MAKEWORD(2, 2), &wsaData)) == SOCKET_ERROR) {
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

	// And now bind my socket
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
