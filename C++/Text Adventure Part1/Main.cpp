//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		Text Adventure									 //
//																		 //
//		$Description:	This shows how to walk around in a text world	 //
//																		 //
//***********************************************************************//

#include <iostream>							// Include this for the cin/cout classes
#include <fstream>							// Include this for the ifstream class
#include <string>							// Include this for the string class
#include <windows.h>						// Include this for the Sleep() function
using namespace std;						// Use the standard namespace

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// * INTRO *
//
// This tutorial is the first of a 3 part series.  In this tutorial we will be reading the rooms
// from our World.txt file.  The file is set up so that we can add unlimited rooms to our game
// without ever having to touch a bit of code. We will be able to move around the rooms with
// collision detection and view the room descriptions.  
//
// The commands in this part are:	look north south east west help quit.  
// Below is an explanation of what each command does:
//	
// "look"  - This will display the room's description
// "north" - This will move the player north if there is a valid room
// "south" - This will move the player south if there is a valid room
// "east"  - This will move the player east if there is a valid room
// "west"  - This will move the player west if there is a valid room
// "help"  - This will display the list of available commands
// "quit"  - This will quit the game
//
// The next tutorial will allow us to look at certain objects in the room.  The remaining
// tutorial will have monsters in certain rooms, and will allows us to kill or be killed by them.
// These tutorials are not intended to create a robust game, but to give you the basics to add the
// other details yourself.  Sometimes you just need a push to get your creative juices flowing.  I 
// am quite proud of the modularity of this tutorial and how you can create a whole new game by just
// changing the data in the text file.  This way, if you create a new game you just need to distribute
// the .exe once, and then the text files for each new game you make.  This is also a cool thing to do
// with a little brother or sister who can't program, but want to make their own games.  Just teach them
// how to setup the text file.  You will notice how little of code there is to create such a cool result.
//
// In order to follow this tutorial you will need to be comfortable with functions, references, while loops,
// structures, and especially file input.  Check out the C++ tutorials on these subjects at www.GameTutorials.com
// for more examples and to gain a better understanding of these concepts if you find yourself lost in this tutorial.
//


// This is a #define for our game file.  The level data will be read from a text file called World.txt.
// You can change this to what ever name you want, most likely the name of your game.  It is a good idea
// to change the extension to something like .lev or something, so it's not so obvious that it's a text file.
// *Important* - Make sure that this file is in the same directory as the .exe when running it outside VC++.
#define GAME_FILE "World.txt"

// These defines are to make our return values from GetInput() more clear.  GetInput will return QUIT if we
// typed "quit", otherwise it will return STILL_PLAYING to let our main loop know to keep going.  It's a good
// idea to always use flags and not numbers.  That is what separates good, clean, readable code from the bad.
#define STILL_PLAYING	1
#define QUIT			0

// This is our room structure.  This holds all the information about the current room.
// We need to know what the current room's name is, the current room's description, and 
// finally, what room names are off in each direction.  We do not read in the whole level
// in the beginning.  We just need to read in the current room's information.  Then, when
// we move in a direction, we just search the game file (World.txt) for the room name that is
// in the direction we went, then read it's information and start all over.  Notice that we
// put a 't' in from of "Room" to show that it's a structure.  This is a good idea to get in the habit of doing.
// If this was a class, we would call it "CRoom".  That is what most professionals use.  Below we also put
// "str" in from of our variable names that are of type "string".  Some people use "sz" (An old C notation).
struct tRoom
{										
	string strCurrentRoom;				// This stores the name of the current room we are in (I.E. "Hallway")
	string strRoomDescription;			// This holds the description of the current room			
	string strRoomNorth;				// This holds the name of the room that is to the north					
	string strRoomEast;					// This holds the name of the room that is to the north			
	string strRoomSouth;				// This holds the name of the room that is to the north				
	string strRoomWest;					// This holds the name of the room that is to the north			
};


///////////////////////////////// DISPLAY ROOM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function displays the current room description to the screen
/////
///////////////////////////////// DISPLAY ROOM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DisplayRoom(tRoom &room)
{
	// This function will be used to show the room description.  It will get called
	// every time we enter "look" or we enter into another room.

	// Just like the function says, we cout the room description of the current room
	cout << room.strRoomDescription << endl << endl;		
}


///////////////////////////////// GET ROOM INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the information for the desired room (strRoom) from the game file
/////
///////////////////////////////// GET ROOM INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void GetRoomInfo(ifstream &fin, tRoom &room)
{
	// Create some temporary strings for reading in data from world.txt
	string strLine = "";
	string strTemp = "";

	// Here we create a string that will store the room name with the '<' and '>' brackets around it.
	// We need to do this because in the text file we are reading from, it has the room blocks with
	// brackets around it.  This makes it more readable as a header.  You can take them off if you want.
	// So, by adding < and > to each side of room.strCurrentRoom, it would turn "Middle" into "<Middle>".
	string strRoom = "<" + room.strCurrentRoom + ">";

	// First we want to return the file pointer to the beginning of the file again.
	// This way we get a clean start for searching the file for the designed room block.

	// Set the file pointer at the beginning of the file and clear() the EOF (End of File) flag
	fin.seekg(NULL,ios::beg);						
	fin.clear();										

	// Next, we want to start looking for the room and read in it's data. Here is the plan:
	// Since we start at the beginning of the file each time, we want to read in each line
	// of the file, starting at the beginning, and then check if that line is equal to the
	// room block that we are looking for, for instance "<Middle>".  If it is, then we want
	// to read in the Middle room's description.  We do this by doing a getline() and stop
	// reading characters when we hit a '*' symbol, which should be placed at the end of
	// every room description in the text file.  This allows us to read in multiple lines
	// of text for the room description, but we needed a character to tell us when to stop reading.
	// We will store the room description paragraph in our room.strRoomDescription variable.
	// After we have the room description, we want to read in the rooms that are north, south,
	// east and west from the room being read in (I.E. <Middle>).  The room names are stored
	// after the direction block, like: <north> Top.   This tells us that to the north there is
	// a room called "Top".  Of course these rooms aren't good names, but in a game you can make
	// them more descriptive like "Hallway", "Library", etc...  
	// We want to store the room names in each of the associate variables, depending on the direction.
	// That means, for instance, that the "Top" in <north> Top should be stored in strRoomNorth.
	// Since we don't want the "<north>" string, we can't use getline(), otherwise it would read
	// in the whole line.  To get around this, we read in one word at a time.  When we read
	// in the first word, it will be "<north>".  Then, when can finally read in the next word
	// which will store the name of the room.  
	// This is coded below with:  fin >> strTemp >> room.strRoomNorth;
	// We use strTemp to read in the "<north>" string, and then store the next word in strRoomNorth.
	// We need to do this for every direction.  Once we finish reading in the last direction (west),
	// we return from the function because we no longer need to read from the file anymore.  We
	// then display the current room description (strRoomDescription) and we are now in the new room.

	// Here we want read in a line at a time from the file until we find the desired room
	while(getline(fin, strLine, '\n'))				
	{
		// If the current line is equal to the room that we are looking for (I.E. <Middle>), then read the room info
		if(strLine == strRoom)					
		{
			// Read in the room description until we hit the '*' symbol, telling us to stop
			getline(fin, room.strRoomDescription, '*');	

			// Read past the direction blocks (I.E. <north>) and store the room name for that direction
			fin >> strTemp >> room.strRoomNorth;				
			fin >> strTemp >> room.strRoomEast;				
			fin >> strTemp >> room.strRoomSouth;				
			fin >> strTemp >> room.strRoomWest;				

			// Stop reading from the file because we got everything we wanted; the room info was read in.
			return;									
		}
	}

}


///////////////////////////////// MOVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function checks if we can move, and if so, moves us to the desired room
/////
///////////////////////////////// MOVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Move(ifstream &fin, tRoom &room, string strRoom)
{
	// This function was created to reduce the amount of code in the GetInput() function.
	// Since each direction had to do the same checks and routines, I figured I would create this.
	// This takes the file pointer, the room data and the we are attempting to go too (I.E. "Middle").

	// Check if the room we are going to is valid. If it's not, then we want to 
	// display a message and return.  "None" is used as a room name in the text file to
	// indicate that there is NO room in that direction.  This is our simple, yet smart collision detection.
	if(strRoom == "None")					
	{
		// Display a message that we can't go in that direction, then leave this function
		cout << "You can't go that way!" << endl;	
		return;									
	}
		
	// If we get here the room name must be valid and we should move to that room.
	// First we want to set the current room we are in to the new room, then read in
	// the new room's data, then display it's room description to the screen.

	// Set the current room to the new room we are moving too
	room.strCurrentRoom = strRoom;

	// Pass in our file pointer, the room info so that we read in the new room's data
	GetRoomInfo(fin, room);		

	// Display the current room
	DisplayRoom(room);								
}


///////////////////////////////// GET INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads the input from the user and acts accordingly
/////
///////////////////////////////// GET INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int GetInput(ifstream &fin, tRoom &room)
{
	// This is the main control function that is called every time in the game loop.
	// This displays a prompt, asks for the user's input, then handles the result of what they typed.
	// If we wanted to quit the game, we return QUIT, otherwise return STILL_PLAYING.

	// Create a variable to hold the user's input
	string strInput = "";

	// Display a simple prompt
	cout << endl << ": ";							

	// Read in the user's input
	cin >> strInput;									

	if(strInput == "look")									// Check if the user typed "look"
	{
		DisplayRoom(room);									// Display the current room's description
	}
	else if(strInput == "north")							// Check if the user typed "north"
	{
		Move(fin, room, room.strRoomNorth);					// Move to the room that is to the north (if it's a valid move)
	}
	else if(strInput == "east")								// Check if the user typed "east"
	{
		Move(fin, room, room.strRoomEast);					// Move to the room that is to the east (if it's a valid move)
	}
	else if(strInput == "south")							// Check if the user typed "south"
	{
		Move(fin, room, room.strRoomSouth);					// Move to the room that is to the south (if it's a valid move)
	}
	else if(strInput == "west")								// Check if the user typed "west"
	{
		Move(fin, room, room.strRoomWest);					// Move to the room that is to the west (if it's a valid move)
	}
	else if(strInput == "quit")								// Check if the user typed "quit"
	{
		cout << "Did you give up already?" << endl;			// Display a quit message
		return QUIT;										// Return QUIT to stop the game and end the program
	}
	else if(strInput == "help" || strInput == "?")			// Check if the user typed "help" or "?"			
	{														// Display a list of commands
		cout << endl << "Commands: look north south help quit" << endl;
	}
	else													// Otherwise we didn't recognize the command typed in
	{
		cout << endl << "Huh???" << endl;					// Display a message indicating we didn't understand what the user wants
	}

	// Return the default value saying that we still are playing
	return STILL_PLAYING;
}


///////////////////////////////// MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function is the start of our program
/////
///////////////////////////////// MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int main()
{
	// Create our file pointer that will be opening and reading the file
	ifstream fin;
	// Create our room structure.  This will hold all of our room data.
	tRoom room;

	// Open the World.txt file.  This will put the file pointer at the beginning of the file.
	fin.open(GAME_FILE);							

	// Check if the file was found.  If it wasn't, we want to quit the program
	if(fin.fail())									
	{												
		// Display a error message and return -1 (Quit the program)
		cout << "Unable to find World.txt" << endl;
		return -1;									
	}

	// If we get here we know that we found our game file, so let's read in the starting room.
	// The starting room should be at the very top of the game file like so:  <Start> Middle.
	// The "<Start>" word is just to make the game file more readable.  We don't care about that
	// when we are reading in the starting room, we just want the "Middle" part.  
	// Since we don't want the "<Start>" string, we can't use getline(), otherwise it would read
	// in the whole line.  To get around this, we read in one word at a time.  When we read
	// in the first word, it will be "<Start>".  Then, when can finally read in the next word
	// which will store the name of the room that we will start at.  We will store the starting
	// room in our room.strCurrentRoom variable.  Notice that we use it twice below?  Well, the first
	// time we read in the "<Start>" word, which is then stored in room.strCurrentRoom.  Then we write
	// over that with "Middle".  It's the same thing as if we did;  cin >> num >> num. If we typed in
	// 10, hit return, then 12, hit return, ... num would equal 12 because it over wrote the 10.

	// Read in the starting room and store it in our strCurrentRoom variable.
	fin >> room.strCurrentRoom >> room.strCurrentRoom;

	// Pass in our file point and our room data so that we can read in and store all the room data
	GetRoomInfo(fin, room);							

	// Once the room data is read, we want to display the current room description
	DisplayRoom(room);									

	// Below is our main game loop.  We have an infinite loop, with our GetInput() function
	// returning a QUIT or a STILL_PLAYING value.  If it == QUIT then we break from the main loop.
	// So far, out game loop is just asking input from the user.  There is no intro's, cut scenes,
	// level changes, etc...  This is the most basic game loop.

	// Start our main game loop
	while(1)										
	{
		// Call our GetInput() function to check the user input and make sure we didn't quit.
		// We pass in our file pointer and our room data so that we don't create any global variables.
		// These variables are passed by reference so that they can be passed to others functions inside
		// of GetInput().  If you can avoid using globals you should.
		if(GetInput(fin, room) == QUIT)			
			break;	// Quit the main loop
	}
	
	// If we get here the game is over and we must have quit.  We now want to do our clean up.
	// We need to close the file we have open and then do a little delay before the program quits.
	
	// Close the file
	fin.close();	
	
	// Delay the program for 3 seconds before quitting.
	Sleep(3000);

	// Return from main (Quit the program)
	return 0;										
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This concludes our first tutorial of the 3 part series.  Soon everyone will be dying
// to play your new text adventure!  Then every week you can create a new game by changing
// the text file.
//
// Let me give a recap on how the game file is set up:
//
// <Start> Middle				// This is the room that we will start at
//
// The next block is the room block.  The name of the room is surrounded in <> brackets
// to show that it's a header.  This makes it easy to spot.  The next few lines is the room
// description.  After the room description there is a '*' symbol to show that it's done.
// that way we can do a getline() and get multiple lines of text.  The next 4 lines hold
// the rooms that are in each direction.  If there is NOT a room in a direction, then instead
// of putting a room name, we use the word "None".  If we try to go north and the <north> block
// has the room name "None", it will tell us that we can't go that way.  That's it!  We can add
// as many rooms as we want this way without touching any code.  This makes it so the code is
// not dependant on names of the rooms.  That is why we read in the start room.  It's incredibly
// modular.
//
// <Middle>						
// You step into a house where you are nearly
// blinded by the light shinning through the window.
// You have been searching for your friend who was captured.
// The trail has led you to this mansion.
// There is a door in every direction.*
// <north> Top
// <east>  Right
// <south> Bottom
// <west>  Left
//
// Let's go through a test run on how this will work.  Let's say we start of the game.  We will
// read in the starting room as "Middle".  Then we search through the entire file for a room
// header called "<Middle>" (remember we add the <>'s to the room name when searching).  We read
// in the data for the room and then display the room description to the screen.  It should look like this:
//
// You step into a house where you are nearly
// blinded by the light shinning through the window.
// You have been searching for your friend who was captured.
// The trail has led you to this mansion.
// There is a door in every direction.
// 
// :
//
// There is a prompt (:) a couple line down that awaits our input.  Say we type "north" and hit return.
// Well, since we have the room name that is stored in our strRoomNorth variable of our room object,
// we make sure it isn't "None".  We know that it's "Top", so now we need to reset the file pointer to
// start at the beginning of the file.  We are searching for the line; "<Top>".  We check the first line,
// no... that's that starting room, then the next, no... that's a empty line, then the next, No... that's
// the "<Middle>" room, etc.. Eventually we get to the line that says: "<Top>".  We then read in it's data
// because we know that we found the right room.  See Below:
//
// <Top>
// As you get closer to the second story of the mansion, the sounds of someone
// struggling can be heard more distinctly.  There is no turning back now.
// One door is to the south and one is to the north up a stairwell.*
// <north> End
// <east>  None
// <south> Middle
// <west>  None
//
// Now we are in the "Top" room.  We display the room description to the screen and once again await
// below at the prompt for user input.  Our only options are to go south and north.  As you can see, 
// east and west have "None" as the room name.  That means we can't go that way.  Notice that there is a
// room with the name of "End".  This will ALWAYS be the last room in our game.  That way you can always
// have an ending room for the game to be over, for mazes and such.  We could have made a block at the
// top of the game file like the <start>:   <end> Attic
// Then we could check if the strEndRoom == strCurrentRoom, but I like the checking for a room name "End" better.
// We don't check for "End" in this tutorial.  I will leave that up to you to decide how you want your game to work.
// It's just an idea, perfect for maze games.
//
// This is pretty much it.  Very simple, yet VERY cool.  The next tutorial will focus on looking at objects in
// the room.  I call them look descriptions.  This works almost identical to the GetRoomInfo() function.
// Here is a sneak peek for the way it's set up in the game file:
//
// <Middle>
// You step into a house where you are nearly
// blinded by the light shinning through the [window].
// You have been searching for your friend who was captured.
// The trail has led you to this mansion.
// There is a door in every direction.*
// <north> Top
// <east>  Right
// <south> Bottom
// <west>  Left
// <look>  1 window
//
// <Middle|window>
// The light pierces your eyes, causing you to wince in pain each time you look directly at the window.*
//
// Can you guess how it works?  You read in how many look descriptions (1 in this room's case), then do
// a for loop to read in as many words as the number of look descriptions.
//
// Then, when they type "view" it will ask them what they want to look at.  If they type in window, it will
// concatenate '<' + room.strCurrentRoom + '|' + strInput + '>' into one string (<Middle|window>), then search for it as we
// do the room header.  We then grab the look description as we do the room description, then display it.  See if you
// can implement this before looking at the next tutorial.
//
// I hope this helps spark your creativity, till next time!
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
