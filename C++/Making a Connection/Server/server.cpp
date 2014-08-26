/****************************************************
	Networking Tutorials #2
	Making the Connection - ServerSide
	by redKlyde

****************************************************/

// Things that will be covered in this tutorial:
// - connecting 2 computers for communication
// - bind()
// - listen()
// - accept()

// Ok.  Now that we know how to setup winsock in a project, and properly open
// and close sockets, the next step is getting two computers to connect with each other.

// Before we go any further, we should pause for a second and elaborate on a concept that should
// be familiar to you, but just in case it isn't ...  We have seen the words
// "client" and "server" thrown about in the names of these tutorials.  This tutorial is in fact titled
// "Making the Connection - ServerSide", but it may not be clear what a server is.  If
// not, it would be somewhat pointless to explain how we make this work in code, without
// explaining what we are doing in the first place.  So we elaborate:  The concept of client
// server architecture is that we have one computer who provides a service for another computer.
// The computer providing the service is what we call the "server", and the computer requesting
// service is called the "client".  This is a simplified explanation, but this is the basis for
// our communication.  What we will do is setup one program to wait for someone to request a
// service (aka. a connection).  This is what we will do in this part of the tutorial.  We will set up a
// server to receive connections.  In the next part, we will setup a client to connect to our server.
// This is the reason there are 2 parts to the next few tutorials.  We have to have one program to
// provide a service, and one program to request service.  Note: this is not the ONLY architecture
// for networking computers, but it is the one most often used when creating networked programs/games, so
// this is the route we will explore.

// One more quick note before we get started.  When networking, we generally want to talk to
// computers that do not likely have the same hardware architecture as ours.  Because of this,
// we can run into byte ordering problems between systems.  Byte ordering commonly refers to
// the way in which our computer stores numbers.  Like when we make an integer 54, some
// systems will actually store the small end in memory first.  So if we were to open up the computer
// and read its memory, we would see the number 45 instead.  This may seem weird, but the
// computer is designed to work this way.  Still, other computers work as we would expect them to,
// storing 54 in memory.  Well, what happens if I send you an integer from my system that uses a
// different byte ordering than your system?  If I send 54, then you will interpret that number as
// actually 45.  This will definitely cause problems.  Thankfully, we don't have to program the solution
// to this.  A standard byte ordering was agreed upon for all networking.  Because of this, we
// have a few handy functions to convert between host byte ordering and network byte ordering.

// To test the client and server tutorials, you will want to start the server
// executable (part1.exe - this application).  This will wait for the client to connect.  
// You then want to start the client application (part2.exe).  You will see that a socket 
// is opened and the server and client connected.

// These functions are:
// - htons() - host to network short
// - htonl() - host to network long
// - ntohs() - network to host short
// - ntohl() - network to host long

// In order to use these functions just pass in the numerical value in it's appropriate ordering, and
// these functions will convert them back and forth between network ordering and the native machine's
// byte ordering.  Thankfully, we don't have to solve this problem, but we do have to remember it
// when we send integer values across the network.

// That being said, lets get all the setup stuff out of the way (since we covered it in tutorial #1),
// and get right to business.

// Winsock for our network communication
#include <winsock2.h>
// Stdio for I/O
#include <stdio.h>

int main() 
{
	printf("Welcome to redKlyde's Networking Tutorials!\n");
	printf("Tutorial # 2 : Making the Connection - ServerSide\n\n");

	// An error code we will use to get more information about our errors
	int error;

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

	// Create my socket
	int mySocket = socket(AF_INET, SOCK_STREAM, 0);

	// Make sure nothing bad happened
	if (mySocket == SOCKET_ERROR) 
	{
		printf("Error Opening Socket!\n");
	}

	printf("Socket Opened!\n");

	// Up to this point it SHOULD be stuff you are familiar with from tutorial #1.
	// If any of this looks confusing, now is the right time to go back and review
	// the first tutorial because it doesn't get any simpler after this.

	// So now that we have a socket open, we actually want to do something
	// with it.  Since we are writing the server, what we want to do is set up our socket to
	// "listen" for incoming connections.  Generally, by "connection" we mean other
	// computers, but in this case, we mean specifically other computers running OUR client
	// application.  Once we are "listening" for connections, we want to "accept" them, and
	// then send some data back and forth.  All this may seem pretty trivial, but there are
	// some specific steps involved.  There is also a reason why I highlighted the words "listen"
	// and "accept".  As it turns out, these are the actual names of the functions we will use.
	// So the next time you hear a bunch of programmers yappin' about listening and accepting,
	// you will know they are talking about this process.

	// What do we have to do to "listen" for connections?  Well, there is a precursor step
	// which we will go through now.  This step is called "binding", and appropriately has a 
	// corresponding call to bind().

	// The bind() step however, requires the introduction of a new data structure, and a little
	// extra explanation as to what exactly is going on.  We will look at the code first, and
	// then discuss what is going on inside your computer.  Look for the explanation after this code:

	// Our new structure: the socket address structure.
	struct sockaddr_in server;

	// Fill the address structure with appropriate data
	server.sin_family = AF_INET;
	server.sin_port = htons(7654); // Remember the byte ordering
	server.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket
	error = bind(mySocket, (sockaddr*)&server, sizeof(server));

	// Check for an error
	if (error == SOCKET_ERROR) 
	{
		printf("Bind Failed!\n");
	}

	// OK!!! WHAT DID WE JUST DO?!?! ... ... Very legitimate question.  To explain this fully
	// requires some explanation of our computer works.  If this is COMPLETELY 
	// uninteresting, just skip this explanation, and know that bind() makes our computer
	// ready to receive data on the socket we created.  For the interested ... please continue.

	// A computer has a list of what are called "ports".  These ports are not like the 
	// printer port, or serial port we commonly think of.  These "ports" are largely
	// imaginary (meaning, they have no physical form).  We can't touch a port, or take one
	// out of the computer, or get more ports if we want them.  These "ports" are what the
	// computer uses to talk with other computers.  When we create a socket that to
	// receive data with, we have to associate that socket with a port.  Then, when other computers
	// want to talk with this program, they will send data to the port that we associated with our
	// program.  This is what the bind() call does.  It takes our socket (mySocket), and "binds" it
	// to the port we specify (7654).  So now when our client wants to send us data, they will send it
	// to port 7654, and we will receive the data on our socket (mySocket).  This will be covered in
	// the ClientSide section of this tutorial, so I won't go into further detail about the client, but at
	// least you know what is going on.

	// So what about that structure, the sockaddr_in?
	// The sockaddr_in structure is a description of what type of connections and information we
	// will be accepting on our socket.  We fill this structure with the appropriate information
	// for the type of communication we will be using, and then use that data to bind() the socket.
	// This way our system knows how to handle incoming data.

	// The members of sockaddr_in we used and why:
	// server.sin_family = AF_INET;
	//    This specifies (much like our socket), that we will be using the Internet Protocols
	// server.sin_port = htons(7654);
	//    This specifies the port that we will be receiving data.
	// server.sin_addr.s_addr = INADDR_ANY;
	//    This tells our system to accept a connection on ANY Internet interface on system.
	//    The different possibilities for interfaces are beyond the scope of this tutorial.  You
	//    Will have to venture on your own to find out more.  INADDY_ANY will do everything
	//    we could ever want it to.

	// Then of course the bind() call.  This takes the sockaddr_in structure we described, and
	// associates the port with the socket, and we are now ready to listen for connections.

	// Another good question at this point would be: "What is significant about the port:7654?"
	// This is an excellent question, and the answer is ... nothing!  We could have picked just about
	// ANY number.  We say "just about" because there are some ports that won't work.  These
	// ports are ones already reserved by the system.  Like for instance port 80.  Port 80 is reserved
	// for http!  So unless we want to get into the system, shut down http, and then claim it for our
	// program, the bind() call will FAIL if you specify port 80!  There are some other ports that are
	// reserved as well.  So how can I know my port is safe?  Well ... a good start would be to stay
	// above 2024.  There isn't any magical number where reserved ports ALWAYS stop and available ports
	// ALWAYS begin.  This is largely dependant on your system. You could even find a port above 2024
	// that is already in use by another program.  So, to start, just pick any ole port and run your program.
	// If you run into problems, then just pick another port and keep going.  For this tutorial, we'll just pick 7654.

	// Now that we can bind a socket to a port, we are ready to listen for connections!  This is done with the
	// listen() call.  It takes 2 parameters.  The first parameter is the socket we want to use to listen with.  The
	// second is the "backlog".  The backlog is simply the number of connections that can "backup" in your system.
	// I will explain this better at the end of this tutorial, for now, we will just put in a number for convenience.

	// Listen for connections
	error = listen(mySocket, 5);

	// Check for errors
	if (error == SOCKET_ERROR) 
	{
		printf("Listen Failed!\n");
	}

	// Pretty simple huh?  Now we have created a socket, bound that socket to a port, and are listening
	// on that socket for connections, what is left?  Well, we have to "accept" a connection before
	// we can do anything with it.  We do this with a call to accept().  Much like our other network functions,
	// accept() takes our socket as its first parameter.  The return value of accept() is a new socket.  This socket
	// is a connection with the client.  Lets go ahead and look at the code, then some further explanation.

	// A quick message so we know how our program is doing
	printf("Waiting for a client to connect ...\n");

	// We need a socket for our client
	int clientSocket;

	// Accept a client
	clientSocket = accept(mySocket, 0, 0);

	// Check for errors
	if (clientSocket == SOCKET_ERROR) 
	{
		printf("Accept Failed!\n");
	}

	// Note: The second and third parameters are optional for our purposes.
	// You could pass in a sockaddr struct for the second parameter, and the size of the sockaddr struct
	// for the third.  When a connection is accepted, the sockaddr struct would be filled with information
	// about the connecting client.  Since we are using TCP, all we NEED to communicate is the clientSocket.
	// We pass in 0, 0 for both parameters, cause we don't really care about the information.

	// Let me revisit something I stated previously.  I mentioned the backlog parameter of the listen() call.
	// I used the number 5 for this parameter without much explanation.  Well, now that we have seen the
	// accept() function, this backlog will make a little bit more sense.  See, once we mark a socket for listening
	// it can begin accepting connections from that moment forward.  So, let's say we didn't IMMEDIATELY call
	// accept().  Let's even say you put it off for 10 or so minutes.  If a client tried to connect, his connection would
	// NOT fail even though we hadn't called accept() yet.  His connection attempt would be logged until we called
	// accept(). Then we would retrieve the socket that communicates with that client from the "backlog".  The backlog
	// parameter is the number of clients you can keep waiting before calling accept().  If for some reason
	// the number of clients requesting a connection exceeds this number, THEN the client would error because
	// a connection could not be established.

	// Ok!  That's making a connection.  You are now ready to start sending and receiving ... but I will leave that
	// for tutorial #3.  Let's clean up appropriately and look at part 2 where we setup the client to connect
	// to our server.

	// Close our socket
	closesocket(mySocket);

	printf("Socket Closed\n");

	// Shut down winsock
	WSACleanup();

	printf("WinSocket Shutdown\n");

	printf("Press any key to continue ...");
	getchar();
	
	return 0;
}

/*----------------------------*\
|  Done by:  redKylde		   |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
