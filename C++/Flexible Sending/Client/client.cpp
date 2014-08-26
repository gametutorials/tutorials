/****************************************************
	Networking Tutorials #4
	Flexible Sending - ClientSide
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// We will be sending and receiving messages of variable length from a client.

// Well, now that we know how our server expects to receive our data, it should be pretty
// straightforward how to send it.  All we need to do is determine the message size, and
// send it just before sending our actual message.

// Once again, lets setup our client like normal, and skip down to the sending part.

// -----------------------------------------------------------------------------------
// startupClient() - will return us a socket that is connected to the server
// specified by serverName, on the port specified by port.  It will return -1
// if an error occurred during the process.
int startupClient(unsigned short port, const char* serverName);

// -----------------------------------------------------------------------------------
// shutdownClient() - will close the socket specified and clean up winsock.
void shutdownClient(int clientSocket);

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>

int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 4 : Flexible Sending - ClientSide\n\n");

	int mySocket;

	// Create our client
	mySocket = startupClient(7654, "localhost");

	if (mySocket == -1) 
	{
		// Hmm... an error occurred.  Let's shutdown our client and bail.
		shutdownClient(mySocket);
		return 1;
	}

	// The number of bytes I send/read ... will also serve as my error code
	int nBytes;

	// Now let's make a buffer for sending our message.  Better make it a big enough 
	// size to send the largest message we would ever want to send.

	#define MAX_MESSAGE_SIZE 4096
	char buffer[MAX_MESSAGE_SIZE];

	// Let's fill in our buffer with some data.  It doesn't matter what data we use, the important part
	// is that we do not pre-determine the size of the data.

	sprintf(buffer, "This is a test of my variable length send buffer");

	// Calculate the size of that data
	unsigned long messageSize = strlen(buffer);

	// Remember to fix our byte ordering to network byte ordering
	messageSize = htonl(messageSize);

	// Send the message size
	nBytes = send(mySocket, (char*)&messageSize, sizeof(messageSize), 0);

	// Check for errors
	if (nBytes == SOCKET_ERROR) 
	{
		printf("Send Failed!\n");
	}

	// Fix our message size back to host ordering because we will need it to send our message
	messageSize = ntohl(messageSize);

	// Send the actual message
	nBytes = send(mySocket, buffer, messageSize, 0);

	// Check for errors
	if (nBytes == SOCKET_ERROR) 
	{
		printf("Send Failed!\n");
	}

	// And that is that.  This concludes the basics of sending and receiving.

	// If you think you know your stuff by now, try replacing the sprintf() with a gets() and type in your 
	// message from the command line.  This way you can see the variable length sending REALLY do its job.
	// If you get even more daring, alter the code on both client and server to have send () / recv() loops and
	// print out all the messages you are sending, instead of having to shutdown and restart the applications
	// each time.

	// Let's not forget to clean up properly.

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
