/****************************************************
	Networking Tutorials #3
	Sending and Receiving - Server Side
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// sending and receiving messages of fixed length between a client and server
// - send()
// - recv()

// By this point we should be able to startup winsock, create a socket,
// bind to a port, begin listening, and accept a connection from a client.
// Now the only thing left to do (and the purpose behind networking in
// the first place) is to actually SEND data!

// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>

// Since we have covered a lot of material so far, I am going to wrap up
// just about everything we have done in a couple functions.
// Note: This is JUST for convenience in this tutorial.  In practice, there are
// MUCH better ways of encapsulating network setup.

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
	printf("Tutorial # 3 : Sending and Receiving - ServerSide\n\n");

	// The socket my server will use for listening
	int serverSocket;

	// Startup our server utilities with my handy function
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

	// FINALLY ... we are actually getting to something worthwhile.  We have created our 
	// server socket, setup that socket for listening and accepting connections, and we have 
	// accepted a client which we can communicate with via the clientSocket.  We are now ready
	// to send and receive data.

	// For sending and receiving we need a few variables to keep track of things.

	// First, we need to keep track of the number of bytes we have sent and received.  This will help
	// us keep determine when communication runs smoothly.
	int nBytes;

	// Next, we need some buffers.  We have to have a buffer from which we send data, and
	// a place to store the data we receive.
	
	// I will define a message size so we will know how much data we will send and receive
	#define MESSAGE_SIZE 24

	// And the actual buffers
	char inMessage[MESSAGE_SIZE];
	char outMessage[MESSAGE_SIZE] = "I received your message";

	// When my client connects, they will send me a message first thing.  So go ahead and receive
	// a message from my client using the recv() function.  recv() takes 4 parameters.  The first
	// parameter is the socket that we are receiving data from.  The second is the location to
	// store the data we received.  The third is the maximum number of bytes to receive in this recv().
	// And the fourth parameter is a flag which we can change how recv() is handled (for all our 
	// networking needs, this flag will always be 0).  recv() will return the number of bytes actually
	// received from the recv() function.  Just like our previous networking functions, recv() will return
	// SOCKET_ERROR if some error has occurred ... so we better check that after we recv().

	// Receive at MAX sizeof(inMessage) bytes into inMessage from the client using clientSocket
	nBytes = recv(clientSocket, inMessage, sizeof(inMessage), 0);

	// check for errors
	if (nBytes == SOCKET_ERROR) {
		printf("Recv Failed!\n");
	}

	// At this point we should have received data from clientSocket, and stored it in inMessage.
	// So lets print out our input buffer and see what the message was ...
	printf("Message Received : %s\n", inMessage);

	// Since we received data from our client, our client application will await a response from us.
	// So lets send him a message back.  We will do this with send(), which works pretty much
	// the same as recv().  With 4 parameters, the first is the socket, the second is the location
	// of the message we are SENDING, and the third is the size of the message we are sending.
	// Just like recv() our flags parameter will be 0.

	// Send sizeof(outMessage) bytes from the buffer "outMessage" to the client on clientSocket.
	nBytes = send(clientSocket, outMessage, sizeof(outMessage), 0);

	// Check for errors
	if (nBytes == SOCKET_ERROR)
	{
		printf("Send Failed!\n");
	}
	else 
	{
		// If there were no errors, print our message
		printf("Message Sent : %s\n", outMessage);
	}

	// And congratulations ... we have just sent and received data between 2 programs!

	// The exact purpose of the third parameter of send() and recv() may be a bit unclear.
	// We defined it as the "size" of the message we are sending, and the "size" of the
	// message to receive, but we also mentioned that it was just the MAXIMUM number of bytes we
    // could receive.  If this is a MAX, is it possible to receive LESS data than we expected?
	// In short this answer is yes, but we will get to the details in part 2 of this tutorial.

	// Now that we have sent data back and forth from our client ... we have to clean house.
	// This involves closing any sockets we opened, and shutting down our network.

	// Close our client socket
	closesocket(clientSocket);

	// Shutdown our network
	shutdownServer(serverSocket);

	printf("Press any key to continue ...\n");
	getchar();
	
	return 0;
}


// -----------------------------------------------------------------------------------
// startupServerForListening() - a function to startup winsock, and then open a socket for listening

int startupServerForListening(unsigned short port) 
{
	// The winsock data structure
	WSAData wsaData;

	// Startup winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) 
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

	// and now bind my socket
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
