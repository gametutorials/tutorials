/****************************************************
	Networking Tutorials #2
	Making the Connection - ClientSide
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// - connecting 2 computers for communication
// - gethostbyname()
// - connect()

// Let us quickly recap all the functions we have used so far.
// WSAStartup() and WSACleanup() - startup and cleanup winsock
// socket() - creates a socket we can use to communicate with
// bind() - binds a socket to a port so we can communicate with a program
// listen() - sets up a socket to receive connections
// accept() - actually retrieves a connection from another program

// What we have at this point is one program (the sever) waiting for someone to connect.
// In this part of the tutorial we will create the application that connects to the server.

// We begin as we typically do, we will use the same libraries, so
// let's make the appropriate includes and references.

// To test the client and server tutorials, you will want to start the server
// executable (part1.exe).  This will wait for the client to connect.  You then
// want to start the client application (part2.exe), which is this application.
// You will see that a socket is opened and the server and client connected.

// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>

// Include needed library for winsock
#pragma comment(lib, "ws2_32.lib")

int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 2 : Making the Connection - ClientSide\n\n");

	// An error code we will use to get more information about our errors
	int error;

	// We will still use winsock for our network communication, so startup winsock like normal.

	// The winsock data structure
	WSAData wsaData;

	// Startup winsock
	error = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Check for errors
	if (error == SOCKET_ERROR)
	{
		printf("Could Not Start Up Winsock!\n");
		return 1;
	}

	printf("WinSocket started\n");

	// We will still use a socket to communicate with, so create a socket just like we did before.

	// Create my socket
	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	// Make sure nothing bad happened
	if (mySocket == SOCKET_ERROR) 
	{
		printf("Error Opening Socket!\n");
	}

	printf("Socket Opened!\n");

	// Up to this point everything is pretty much the same, but at this point in part 1, we did a bind()
	// and setup the server socket for listening.  But, this is the client application.  We want to CONNECT
	// to the server, not ACCEPT incoming connections.  Though much like the server, we will fill in a
	// sockaddr_in structure with the appropriate information about the application we want to connect to,
	// but then make a call to connect() instead.

	// We need some special information though.  Specifically, we need the location of the server.  We
	// need the IP address of the server computer.  Typically we don't always know the IP of a computer, 
	// especially once we distribute an application.  That IP address could also change if we switched 
	// internet providers or added more server computers.  So though we COULD use the IP (i.e. 192.169.1.100), 
	// we typically use the web address instead (www.gametutorials.com).

	// Since we are running the server on our own computer, what is our internet address?  Well, luckily
	// we don't have to have this exact information.  There is a universal name we can use when referencing our
	// own computer.  This name is "localhost".

	// The server name
	char server_name[40] = "localhost";

	// If you are on a network and you want to run the client on another computer, you can get our IP from
	// running "ipconfig" from the command line, and enter that IP address instead of the word "localhost".

	// Well, now that we have the name of the server, we have to translate that name into something our computer 
	// can use.  We have to use a special structure for this, the "host entry" structure.

	struct hostent *host_entry;

	// The hostent structure contains the name of the host and the IP address of that host (among some other things).
	// We can fill all this information by calling gethostbyname().  This will lookup the name of the server
	// we provided, and return all the other information we need.  

	// Setup the host entry
	host_entry = gethostbyname(server_name);

	// Check for errors:
	// Note, at this point that gethostbyname() returns a pointer to a hostent structure.  So if an error occurs, the
	// function will return a NULL.  This is different from our previous winsock functions that returned SOCKET_ERROR.
	if (host_entry == NULL) 
	{
		printf("Could not find host!\n");
	}

	// Now that we have the address of the server, we have to set up a sockaddr_in structure that will describe
	// the type of connection we are trying to make.

	// Our sockaddr structure
	struct sockaddr_in server;

	// Fill in the server info
	server.sin_family = AF_INET;
	server.sin_port = htons((unsigned short) 7654);
	server.sin_addr.s_addr = *(unsigned long*) host_entry->h_addr;

	// This is much the same as the server.  We are making an internet connection, so we use AF_INET.  We specify
	// the port in network byte ordering.  Instead of using INADDR_ANY, we specify an address.  This is the
	// address we retrieved from the hostent structure.

	// We now have all the information we need to connect to the server.  We make the connection with a call to
	// connect().  Connect takes 3 arguments.  The first is the socket we will communicate with, the second is
	// the sockaddr structure that describes our connection.  The third is simply the sizeof the sockaddr structure.

	// Connect to the server
	error = connect(mySocket, (sockaddr*)&server, sizeof(server));

	// Check for errors
	if (error == SOCKET_ERROR) 
	{
		printf("Error connecting to server!\n");
	}

	// Assuming our server is running, we should have made a connection to the server at this point.  The server will
	// receive the connection, and continue with the program after the accept() call.  Since that is all the server was
	// designed to do, it will cleanup itself and shutdown.  Since this is all we have intended with this part of the
	// client tutorial, we will do the same.

	// Close our socket
	closesocket(mySocket);

	printf("Socket Closed\n");

	// Shut down winsock
	WSACleanup();

	printf("WinSocket Shutdown\n");

	printf("Press any key to continue ...\n");
	getchar();
	
	return 0;
}

/*----------------------------*\
|  Done by:  redKylde		   |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
