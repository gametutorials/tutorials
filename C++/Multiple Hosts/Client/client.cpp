/****************************************************
	Networking Tutorials #5
	Multiple Hosts - ClientSide
	by redKlyde

****************************************************/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

// Things that will be covered in this tutorial:
// -absolutely nothing

// This part of the tutorial is exactly like tutorial #4, with one small exception.  The
// client will accept input from the command line, and send it to the server.  It will
// continue this until the client types 'quit'.  There is no new material, so browse 
// through the code, and play around with it as you like.
// - redklyde

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
	printf("Tutorial #5 : Multiple Hosts - ClientSide\n\n");

	// My socket
	int mySocket;

	// Create our client
	mySocket = startupClient(7654, "localhost");

	if (mySocket == -1) 
	{
		// Hmm... an error occurred.  Let's shutdown our client and bail.
		shutdownClient(mySocket);
		return 1;
	}

	// Number of bytes sent
	int nBytes;
	
	// I will define a message size so we will know how much data we will send and receive
	#define MAX_MESSAGE_SIZE 4096
	char buffer[MAX_MESSAGE_SIZE];

	printf("Input text to send and press Enter\nType 'quit' to exit\n");

	// The main loop
	for (;;) 
	{
		// Get some input from the command line
		gets(buffer);

		// Check if quit was typed, then exit
		if (strcmp(buffer, "quit") == 0) 
		{
			break;
		}

		// Get our message size
		unsigned long messageSize = strlen(buffer);

		// Fix our byte ordering
		messageSize = htonl(messageSize);

		// Send the message size
		if ((nBytes = send(mySocket, (char*)&messageSize, sizeof(messageSize), 0)) == SOCKET_ERROR) 
		{
			printf("Send Failed!\n");
		}

		// Re-fix our byte ordering
		messageSize = ntohl(messageSize);

		// Send the actual message
		if ((nBytes = send(mySocket, buffer, messageSize, 0)) == SOCKET_ERROR) 
		{
			printf("Send Failed!\n");
		}
	}

	// Shutdown our client
	shutdownClient(mySocket);
	return 0;
}


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
	if ((host_entry = gethostbyname(serverName)) == NULL) 
	{
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
