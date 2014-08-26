//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		Text Adventure									 //
//																		 //
//		$Description:	This shows how to look at objects in your world	 //
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
// This tutorial is the second of the 3 part series.  In this tutorial we will be able to
// put things to look at in our rooms.  We are adding a new command called "view" that
// will allow us to type in an object or keyword to look at.  After we type "view", then
// hit return, it will prompt us to type in the key word that we want to look at.  This
// can only be one word because we are using "cin" to read in the data.  You can use
// getline() for multiple words, but be prepared to sort out the bugs of using getline()
// and "cin" at the same time.  
//
// We add a new block to our room description: "<look>"
// This will store the number of keywords in the room, and their names.  Then
// we create a header similar to the room header to store the keywords description.
// More of this will be explained throughout the tutorial and at the end.
//
// Here is a list of all the commands:	look view north south east west help quit.  
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
// To fully understand this tutorial, you will need to be comfortable with arrays.
// We use an array to hold the keywords stored in each room (if any).  Check out our
// tutorials on arrays if you need some refreshing.
//
//

// This is a #define for our game file.  The level data will be read from a text file called World.txt.
#define GAME_FILE "World.txt"

// These defines are to make our return values from GetInput() more clear.  
#define STILL_PLAYING	1
#define QUIT			0

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This define is used to restrict the maximum amount of things to look at in a room.
// I chose 3 as an arbitrary number.  You can increase this to what ever you want.
// This means that we will only be able to look at 3 different things in a room.
// If we have more than 3 keywords in the game file, it will read in only 3 and ignore the rest.
#define MAX_LOOKS		3

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is our room structure.  This holds all the information about the current room.
// Notice that we added 2 new variables to our tRoom structure.  These will hold the
// information about keywords that we will be able to "view".
struct tRoom
{										
	string strCurrentRoom;				// This stores the name of the current room we are in (I.E. "Hallway")
	string strRoomDescription;			// This holds the description of the current room			
	string strRoomNorth;				// This holds the name of the room that is to the north					
	string strRoomEast;					// This holds the name of the room that is to the north			
	string strRoomSouth;				// This holds the name of the room that is to the north				
	string strRoomWest;					// This holds the name of the room that is to the north			

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we create an array with the size of MAX_LOOKS (3).  That means we can hold
	// 3 keywords max for each room.  For example, "floor", "creature" and "darkness".
	// We then need a variable to store the description of that keyword, like we do for
	// the room description.  Once we choose a keyword to look at, we first make sure
	// it's in the strLookArray, then search the game file for it's description.

	string strLookArray[MAX_LOOKS];		// This holds the names of the things to look at in the room				
	string strLookDescription;			// This holds the description of the thing we look at

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

};


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// CHECK LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function searches the strLookArray to make sure we looked at something valid
/////
///////////////////////////////// CHECK LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CheckLook(tRoom &room, string strLook)
{
	// This function is called after we type in "view", then choose the keyword
	// we desire to look at.  It then searches for the strLookArray to make sure
	// what we wanted to look at is in the room.  If it is, it returns true, otherwise false.
	//
	// Here is an example run through:  Say we read in the keywords for a room in a log cabin.
	// There are 2 keywords: "fork" and "chair"
	// Let's say I choose to look at "bed".  It will then go through the whole strLookArray,
	// not find "bed", then return false, which will then display a mistake error to the screen.

	// Go though all the slots in our strLookArray and check if the word we are trying to
	// look at (strLook) is in this room's keywords.
	for(int i = 0; i < MAX_LOOKS; i++)			
	{
		// If the desired word is in this index of the look array, return true!
		if(strLook == room.strLookArray[i])			
			return true;						
	}	

	// If we get here, we know that we didn't find the word in the list, so return false.
	return false;								
}


///////////////////////////////// DISPLAY LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function displays the description of thing we are looking at
/////
///////////////////////////////// DISPLAY LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DisplayLook(string strLookDescription)
{
	// This function is only called IF the object/thing we looked at was in
	// the strLookArray list of keywords for the room.  This is just like DisplayRoom().

	// Print the look description out to the screen
	cout << endl << strLookDescription << endl;			
}


///////////////////////////////// GET LOOK INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function finds and reads in the description for the desired keyword
/////
///////////////////////////////// GET LOOK INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void GetLookInfo(ifstream &fin, tRoom &room, string strLook)
{
	// Create a string to hold the contents of each line that we read in
	string strLine = "";								

	// Set the file pointer at the beginning of the file and clear() the EOF (End of File) flag 
	fin.seekg(NULL,ios::beg);						
	fin.clear();

	// This function is very similar to GetRoomInfo().  We do the same types of things.
	// The algorithm to get the look description is this:  Go through every line
	// in the game file and check if it's the look block with our desired description.
	// Say we are in the room titled, "Stairway", and we want to look at the object, "knife".
	// Well, the look block will look like this: <Stairway|knife>
	// That way we can have multiple objects/things of the same name, but in different rooms.
	// If we didn't do it this way it might grab the description of another room's knife.
	// I chose to put a "|" (pipe - shift + '\') between the room name and keyword.
	// This is just more clear to me that we are looking at the room "Stairway" with a
	// keyword "knife" in that room.  You can use any syntax you want to make it understandable.

	// Here we want read in a line at a time from the file until we find the desired room
	while(getline(fin, strLine, '\n'))				
	{
		// Check if the current line read in is the desired look header we want.
		// Remember that we need to add the brackets <> and '|' between the room
		// name and the keyword.
		if(strLine == "<" + room.strCurrentRoom + "|" + strLook + ">")						
		{
			// If we get here, we just read in the look header (IE, "<Room1|jacket>").
			// Now let's read in the look description.  Just like the room description,
			// we do a getline() that reads all the characters of the description until
			// we hit a '*' character, which tells us we are done with the description.
			getline(fin, room.strLookDescription, '*');	

			// Leave this function since we are done reading in the description.
			return;									
		}
	}

	// If we get here, something's wrong and we didn't read the description... Check the game file.
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// DISPLAY ROOM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function displays the current room description to the screen
/////
///////////////////////////////// DISPLAY ROOM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DisplayRoom(tRoom &room)
{
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
	string strRoom = "<" + room.strCurrentRoom + ">";

	// Set the file pointer at the beginning of the file and clear() the EOF (End of File) flag
	fin.seekg(NULL,ios::beg);						
	fin.clear();										

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			// This is where we read in the keywords for the current room.
			// Let's take a look at the first room's look block: <look>  1 window
			// We obviously need to read past the "<look>" word as we did with all the others,
			// but then we need to read in how many keywords there are in this room.  That
			// is what the "1" is for.  Since there is only 1 keyword, "window", we put
			// a "1" right after the "<look>" word.  If we had 2 keywords like "window" and
			// "mansion", we would put a "2" instead of a "1" like so:  <look> 2 window mansion
			// Below we use the "strTemp" string to read past the "<look>" word, then use
			// a local created integer "count" to read in the number of keywords.
			// Before we read in the keywords, we want to initialize the strLookArray.
			// That way if one room only has one keyword, we won't be able to look at
			// 2 of the 3 keywords in the last room.  This would cause a weird problem.
			// Once we have the number of keywords, we need to do a loop that reads the
			// keywords into the strLookArray a word at a time.  Notice that we mod (%) the
			// count by MAX_LOOKS.  This is so we don't read in more than 3 keywords.  If
			// we read in 4 our program would crash because we went outside the array boundaries.

			// Create local variables to read in the number of keywords and for a index counter
			int count = 0, i = 0;

			// Read past the "<look>" word and then store the number of keywords into "count"
			fin >> strTemp >> count;		

			// Go through all the indices of the array and set them to nothing
			for(i = 0; i < MAX_LOOKS; i++)			
			{
				// Initialize the current index to nothing
				room.strLookArray[i] = "";		
			}
			
			// Loop through and read in the keywords until we have them all.  We also
			// make sure that we don't read in more than our strLookArray can hold by the %.
			for(i = 0; i < count % MAX_LOOKS; i++)			
			{
				// Store the current keyword into the current index of our strLookArray
				fin >> room.strLookArray[i];		
			}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
	// Check if the room we are going to is valid. If it's not, then we want to 
	// display a message and return.  "None" is used to indicate that there is no room.
	if(strRoom == "None")					
	{
		// Display a message that we can't go in that direction, then leave this function
		cout << "You can't go that way!" << endl;	
		return;									
	}
		
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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	else if(strInput == "view")								// Check if the user typed "view"
	{
		// If we choose "view", we then want to prompt the user to type in the
		// keyword that they want to look at.  Next, we check if that keyword
		// is in our list of keywords for the room.  If it is, we read in the 
		// description from the game file and display it, otherwise display 
		// an error message saying there is nothing to look at with that name.

		// Display the prompt to type in something to look at, then read it in
		cout << "What do you want to look at? ";	
		cin >> strInput;							
				
		// Check if what we typed in was valid in the room
		if(CheckLook(room, strInput))				
		{											
			// Read in the description for the keyword we typed in, then display it
			GetLookInfo(fin, room, strInput);
			DisplayLook(room.strLookDescription);							
		}
		else										
		{		
			// Display an error message saying there is nothing in the room like that
			cout << "I don't see anything like that..." << endl;
		}
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
		cout << endl << "Commands: look view north south help quit" << endl;
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

	// Read in the starting room and store it in our strCurrentRoom variable.
	fin >> room.strCurrentRoom >> room.strCurrentRoom;

	// Pass in our file point and our room data so that we can read in and store all the room data
	GetRoomInfo(fin, room);							

	// Once the room data is read, we want to display the current room description
	DisplayRoom(room);									

	// Start our main game loop
	while(1)										
	{
		// Call our GetInput() function to check the user input and make sure we didn't quit.
		if(GetInput(fin, room) == QUIT)			
			break;	// Quit the main loop
	}
	
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
// This concludes our second tutorial of the 3 part series.  Now we can look
// at objects/things in our rooms.  To look at something, type in "view" and hit return.
// You will then get a prompt to enter the keyword to look at.  Then you
// type in the keyword and hit return.  If the keyword is valid, then it should
// display the description to the screen.  I placed brackets '[' ']' around each
// of the keywords to make the obvious.  You may or may not want to have these,
// depending on if you want it to be harder for the user to get hints or clues on things.
//
// Let's go over the new syntax added to the game file.  Our room block now looks like
// this:
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
// Everything else is the same except for the "<look>" data.  This stores the
// number of keywords in the current room, and their names.  In the above example,
// we have "1" keyword, and it's name is "window".  That means that we can look
// at a "window".  What happens when we look at a window?  Well, the game then searches
// the game file for this block:
//
// <Middle|window>
// The light pierces your eyes, causing you to wince in pain each time you look directly at the window.*
//
// The look header has the room's name and the keyword, with some brackets and a
// separator between the room name and keyword.  This is just the syntax I chose, you
// can change it to what ever makes it more clear for you when creating levels.
// The reason why we associate the room name with the keyword is because we want to
// be able to have keywords with the same name in different rooms.  If we had 2 rooms
// that had a window to look at in it, it would just find the first window in the game
// file and never get to the second room's window description (IE <window).  This way,
// if the room "Middle" has a window, and the room "Top" has a window, we can distinguish
// between: "<Middle|window>" and "<Top|window>"
//
// The next line after the look header is the actual description we want.  Like the
// room description, it has a '*' at the end of it to show that we are done.  This way
// we can have multiline descriptions.  
//
// It might be a good idea to convert all the data to lowercase when reading it in,
// just in case you don't capitalize one letter like this case: "<middle|window>".
// When editing text files this happens all the time.  Leave the descriptions the 
// same of course.
//
// Hopefully the comments around the new functions above are enought to explain what's
// going on.  If not, pop us an email for a larger explanation.  We aim to please :)
// The next tutorial will allow us to actually put some enemies in our world and fight them.
// I hope that you are starting to get idea's on how you can put other things in your world,
// such as items for an inventory, other players to talk too, trap doors, etc...
//
// Till next time!
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
