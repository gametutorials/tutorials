/****************************************************
	Networking Tutorials #3
	Sending and Receiving - Client Side
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// sending and receiving messages of fixed length between a client and server

// There really isn't anything going on in this part that we have not covered.  All the functions to
// send and receive are the same on the client as they are on the server.  So we will introduce
// no new functions.  The only difference is the sending order.  We designed our server to receive
// a message from our client the moment it connects.  So what we will do is send our message
// immediately after we connect.  We will then wait for the reply message by the server.

// The only noteworthy change to this side of the tutorial is just like we did for our server, we are
// going to encapsulate all our startup and shutdown into a couple of functions and clean up some
// of the clutter of our code.

// -----------------------------------------------------------------------------------
// startupClient() - will return us a socket that is connected to the server
// specified by serverName, on the port specified by port.  It will return -1
// if an error occurred during the process.
int startupClient(unsigned short port, const char* serverName);

// -----------------------------------------------------------------------------------
// shutdownClient() - will close the socket specified and clean up winsock.
void shutdownClient(int clientSocket);

// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>

int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 3 : Sending and Receiving - ClientSide\n\n");

	int mySocket;

	// Create our client
	mySocket = startupClient(7654, "localhost");

	if (mySocket == -1) 
	{
		// Hmm... an error occurred.  Shutdown our client and bail
		shutdownClient(mySocket);
		return 1;
	}

	// The number of bytes I send/read ... will also serve as my error code
	int nBytes;
	
	// I will define a message size so we will know how much data we will send and receive
	#define MESSAGE_SIZE 24

	// And the actual buffers
	char inMessage[MESSAGE_SIZE];
	char outMessage[MESSAGE_SIZE] = "I sent you this message";

	// Send the message to the server
	nBytes = send(mySocket, outMessage, sizeof(outMessage), 0);

	// check for errors
	if (nBytes == SOCKET_ERROR) 
	{
		printf("Send Failed!\n");
	} 
	else
	{
		// Print the message we sent
		printf("Message Sent : %s\n", outMessage);
	}

	// Receive the reply from the server
	nBytes = recv(mySocket, inMessage, sizeof(inMessage), 0);

	// Check for errors
	if (nBytes == SOCKET_ERROR) 
	{
		printf("Recv Failed!\n");
	} 
	else 
	{
		// Print the received message
		printf("Message Received : %s\n", inMessage);
	}

	// Shutdown our client
	shutdownClient(mySocket);	

	printf("Press any key to continue ...\n");
	getchar();
	
	return 0;
}


// -----------------------------------------------------------------------------------
// startupClient() - a function to startup winsock and connect to a server

int startupClient(unsigned short port, const char* serverName) 
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

	struct hostent *host_entry;

	// Setup the host entry
	if ((host_entry = gethostbyname(serverName)) == NULL) {
		printf("Could not find host!\n");
	}

	struct sockaddr_in server;

	// Fill in the server socket info
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;

	// Connect to the server
	if (connect(mySocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) 
	{
		printf("Error connecting to server!\n");
	}

	printf("Client Started\n");

	return mySocket;
}


// -----------------------------------------------------------------------------------
// shutdownClient() - a function to shutdown a socket and clean up winsock

void shutdownClient(int clientSocket) 
{
	// Close our socket
	closesocket(clientSocket);

	// Shut down winsock
	WSACleanup();

	printf("Client Shutdown\n");
}


/*----------------------------*\
|  Done by:  redKylde		   |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
