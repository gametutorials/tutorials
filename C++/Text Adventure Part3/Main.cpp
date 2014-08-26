//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		Text Adventure									 //
//																		 //
//		$Description:	This shows how to add and fight monsters		 //
//																		 //
//***********************************************************************//

#include "main.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// * INTRO *
//
// This tutorial is the third and final part of the series.  In this tutorial we add monsters
// to our rooms.  You will also be able to fight the monsters, at your own risk of course.
// There will be a new and final command added to our list, "status".  Since we are fighting
// big scary monsters, we need to know how far we have until we end up in Hades lap.  Other
// information is given with this command such as the player's name, weapon and it's damage.
//
// A new block is added to our room information: "<enemy>"
// This stores the name of the monster, otherwise it has "none" to indicate no monsters.
// Like the look keywords, we search for the monsters information with their name.
// To make the tutorial less complicated, I chose to have the creature attack you
// once you stepped into the room.  I am sure it will be easy to imagine how much
// better the game you make will deal with the fighting.  One other left undone matter,
// you can continue to walk in and out of a room fighting the same monster over and over again.
// This is also something you will most likely want to change.  Ideas will be presented
// at the bottom of this tutorial to help you with where you would go from here.
//
// Here is a list of all the commands:	look view status north south east west help quit.  
// Below is an explanation of what each command does:
//	
// "look"   - This will display the room's description
// "view"   - This will allow you to look at a special object/thing in the room
// "status" - This will display the player's current information to the screen
// "north"  - This will move the player north if there is a valid room
// "south"  - This will move the player south if there is a valid room
// "east"   - This will move the player east if there is a valid room
// "west"   - This will move the player west if there is a valid room
// "help"   - This will display the list of available commands
// "quit"   - This will quit the game
//
// To fully understand this last tutorial, you will need to be comfortable with classes.
// We use 2 classes for the player and monster.  If you aren't comfortable with the basics
// of classes, be sure to check out our tutorials on such subjects.  
//
//


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// GET MONSTER INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the monster's data from the game file
/////
///////////////////////////////// GET MONSTER INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void GetMonsterInfo(ifstream &fin, tRoom &room, string strMonsterName)
{
	// Create a string to hold the contents of each line that we read in
	string strLine = "";								

	// Set the file pointer at the beginning of the file and clear() the EOF (End of File) flag 
	fin.seekg(NULL,ios::beg);						
	fin.clear();

	// It seems like a good idea to set the name of the new monster here, since we
	// obviously know there is a monster in the current room now, and we have it's name.
	room.monster.SetName(strMonsterName);

	// Here we want read in a line at a time from the file until we find the desired room
	while(getline(fin, strLine, '\n'))	
	{
		// Just like the look header, we check for the monster header the exact same way.
		// Below we check if the current line we read in was the header we desire.
		// An example header would be: <Right|goblin>
		// The room name is "Right" and the monster's name is "goblin".  In steps it becomes:
		//
		// "<" + room.strCurrentRoom + "|" + strMonsterName + ">" = 
		// "<room.strCurrentRoom|strMonsterName>" = 
		// "<Right|goblin>"
		//
		
		// Check if the current line read in is the monster header
		if(strLine == "<" + room.strCurrentRoom + "|" + strMonsterName + ">")
		{
			// One of the differences from getting the monster info and getting the look
			// descriptions is that the monsters have 3 blocks of information.
			// Let's look at a full monster block:
			//
			// <Right|goblin>   - This is the header we search for
			// <Health> 125     - This is the health of the monster
			// <Damage> 12      - This is the damage the monster inflicts on the player
			// <Attack> The goblin claws your face.*    - This is the attack message
			//
			// If you know how to read in a room block, this works just the same.
			//
			
			// Create an integer to store the health and damage
			int data = 0;

			// Read in the health of the monster
			fin >> strLine >> data;

			// Pass in the data to our monster member function, SetHealth(), to store the health
			room.monster.SetHealth(data);

			// read in the damage of the monster and assign it to the monster
			fin >> strLine >> data;
			room.monster.SetDamage(data);

			// Read past the "<Attack>" word
			fin >> strLine;

			// Read the attack description until we hit a '*'
			getline(fin, strLine, '*');

			// Assign the attack message to our monster
			room.monster.SetAttackMessage(strLine);

			// Stop reading from the file
			return;
		}
	}
}


///////////////////////////////// GET PLAYER INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the player's data from the game file
/////
///////////////////////////////// GET PLAYER INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void GetPlayerInfo(ifstream &fin, CPlayer &player)
{
	// Create some local variables to store what we read in from the file
	string strWord;
	int data = 0;

	// Unlike the other Get*Info() functions, we don't need to search for the
	// player data.  We know that it's right at the beginning of the file:
	//
	// <Name>   Troy		- This is the player's name
	// <health> 275			- This is the player's health
	// <weapon> dagger		- This is the player's weapon
	// <damage> 21			- This is the players IQ, .. oh, I mean weapon's damage.
	//
	// Just like every block of data in the text file, the player data is read the same.
	// You will want to read past the first word, then store what's after it.
	//
		
	// Set the file pointer at the beginning of the file and clear() the EOF (End of File) flag 
	fin.seekg(NULL,ios::beg);
	fin.clear();
	
	// Read in the player's name
	fin >> strWord >> strWord;

	// Set the player's name by it's data access member function
	player.SetName(strWord);

	// Store the first word, then use the integer to store the health.
	fin >> strWord >> data;

	// To set the player's health, pass it into SetHealth()
	player.SetHealth(data);

	// Read in and store the player's weapon name
	fin >> strWord >> strWord;
	player.SetWeapon(strWord);

	// Read in and store the players damage
	fin >> strWord >> data;
	player.SetDamage(data);
}


///////////////////////////////// GET MONSTER INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the monsters data from the game file
/////
///////////////////////////////// GET MONSTER INFO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DisplayPlayer(const CPlayer &player)
{
	// When we hit "status", it will call this function to print out our player's details.
	// Notice that the data access functions are being used, instead of player.strName.
	// This might seem silly now, but it is a safer way to program.  A "const" is also
	// put in front of the parameter "CPlayer &player" to ensure that we don't accidentally
	// change anything.  References are being used so that the player structure is not 
	// copied onto the stack, but a pointer or "reference" to the data is used instead.

	// Display our player's status to the screen
	cout << "Name: "   << player.GetName()   << endl;
	cout << "Health: " << player.GetHealth() << endl;
	cout << "Weapon: " << player.GetWeapon() << endl;
	cout << "Damage: " << player.GetDamage() << endl;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// CHECK LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function searches the strLookArray to make sure we looked at something valid
/////
///////////////////////////////// CHECK LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CheckLook(tRoom &room, string strLook)
{
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

	// Here we want read in a line at a time from the file until we find the desired look header
	while(getline(fin, strLine, '\n'))				
	{
		// Check if the current line read in is the desired look header we want.
		if(strLine == "<" + room.strCurrentRoom + "|" + strLook + ">")						
		{
			// Now let's read in the look description.  Just like the room description,
			getline(fin, room.strLookDescription, '*');	

			// Leave this function since we are done reading in the description.
			return;									
		}
	}

	// If we get here, something's wrong and we didn't read the description... Check the game file.
}


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
			
			// Loop through and read in the keywords until we have them all.  
			for(i = 0; i < count % MAX_LOOKS; i++)			
			{
				// Store the current keyword into the current index of our strLookArray
				fin >> room.strLookArray[i];		
			}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			// With a new "<enemy>" block added to our room block, more data will need
			// to be read in each time we enter a new room.  This is read just like the
			// directions: <enemy> goblin
			// We will read past the first word, then read and store the next word "goblin".
			// When there is not a monster in the room, "none" will substitute the monster's name.
			// Once we determine if there is a monster in the room, we need to read in it's
			// data immediately afterwards because he is probably going to clobber us.

			// Read in the monsters name, if any
			fin >> strTemp >> strTemp;

			// If there was no monster in the room, strTemp will have read "None"
			if(strTemp == "None")
			{
				// Set our monster flag to false and return from the function
				room.bMonsterInRoom = false;
				return;
			}
			
			// We must have a monster in the room, so let's set the monster flag to true
			room.bMonsterInRoom = true;

			// Next, a call to GetMonsterInfo() with the monster name passed in will
			// search for the monster header, then read in the monster's info.
			GetMonsterInfo(fin, room, strTemp);

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

int GetInput(ifstream &fin, tRoom &room, CPlayer &player)
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
	else if(strInput == "view")								// Check if the user typed "view"
	{
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

	else if(strInput == "status")							// Check if the user typed "status"
	{
		DisplayPlayer(player);								// Display the player's current status
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
		cout << endl << "Commands: look view status north south help quit" << endl;
	}
	else													// Otherwise we didn't recognize the command typed in
	{
		cout << endl << "Huh???" << endl;					// Display a message indicating we didn't understand what the user wants
	}

	// Return the default value saying that we still are playing
	return STILL_PLAYING;
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// ATTACK PLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function will handle the battle between the monster and player
/////
///////////////////////////////// ATTACK PLAYER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int AttackPlayer(tRoom &room, CPlayer &player)
{
	// This is probably the most important and complicated function in this tutorial.
	// In order to simplify the tutorial so no one will get lost, the battle sequence 
	// is NOT random and does NOT take off random damages.  Though this would have been 
	// pretty easy to add, I chose to leave it out.  It will give you something to do 
	// yourself that isn't too hard.  Let's go over the algorithm that is used for battle:

	// while(there is a monster in the room)
	// { 
	//    - The monster attacks first and subtracts their damage from the player's health
	//    - The player then attacks and subtracts his/her damage from the monster's health
	//  
	//	  - Now we check: if(the player is dead), we return PLAYER_IS DEAD and the game is over
	//    - Otherise, we check:  if(the monster is dead), display a victory message and stop
	//    - If the player and monster are still alive, we want to loop again from the top
	// }
	//
	// Pretty simple huh?  It doesn't get any easier than that.  It should be fun to see what
	// you guys come up with for your fighting sequences.
	//

	// Check if there is a monster in the room
	while(room.bMonsterInRoom)
	{
		// Display the attack message of the monster, right before it attacks us
		cout << room.monster.GetAttackMessage() << endl;

		// Subtract the monster's damage from our health because we just got hit.
		// The result of "player.GetHealth() - room.monster.GetDamage()" will be then
		// passed into SetHealth(), which will change the players health to a lower number.
		player.SetHealth(player.GetHealth() - room.monster.GetDamage());
		
		// Display our current health
		cout << "You now have " << player.GetHealth() << " health." << endl;

		// Display our attack message before we pound the enemy
		cout << "You attack the "  << room.monster.GetName() 
			 << " with your "      << player.GetWeapon() << " for " 
			 << player.GetDamage() << " hit points!"     << endl;

		// Subtract our damage from the monster's health
		room.monster.SetHealth(room.monster.GetHealth() - player.GetDamage());

		// Displa the monsters status
		cout << "It now has " << room.monster.GetHealth() << " health." << endl;

		// We our health is less than 0 (we died)...
		if(player.GetHealth() <= 0)
		{
			// Display a defeating message and quit the game
			cout << "You are dead!" << endl;
			return PLAYER_IS_DEAD;
		}
		// Else if the monster is dead...
		else if(room.monster.GetHealth() <= 0)
		{
			// Display our victory and set the monster flag to false (no more monster)
			cout << "You killed the " << room.monster.GetName() << "!" << endl;
			room.bMonsterInRoom = false;		
		}

		// Put in a delay between each round
		Sleep(1500);
	}

	// The monster didn't stand a chance, so we return a flag that says we are still alive
	return PLAYER_STILL_ALIVE;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Create an instance of our player class to hold our player's data
	CPlayer player;

	// Since the player's data is stored at the beginning of the text file,
	// there is no searching to be done.  We can just read it in immediately.
	GetPlayerInfo(fin, player);

	// We better initialize the monster flag to be false.  It's a
	// good idea to memset()/initialize the room structure in the beginning.
	room.bMonsterInRoom = false;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
		if(GetInput(fin, room, player) == QUIT)			
			break;	// Quit the main loop


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

		// This function is called to handle the battle scenes.  If there is
		// a monster in the current room, this function will loop continuously
		// until either the player or monster are dead.  If you die, the game is over,
		// otherwise you keep on truckin'.
		if(AttackPlayer(room, player) == PLAYER_IS_DEAD)
			break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
// There was a lot of new functions and changes added to this tutorial.  Hopefully
// it didn't go too busy for anyone.  Let me review what was added to this tutorial:
//
// - First off, we moved all of our prototypes, structures/classes and constants to
// a header file.  You can be expected to write a good game without clean code.  It's
// like trying to study in a messy room with Pantera booming in your ears, though some
// people can do it and enjoy it that way...  
//
// --------------- New Stuff Added ----------------
//
// - In our Main.h header file, we added 2 new classes for the player and monster:
// CMonster and CPlayer.  An instance of CMonster was added to our tRoom structure,
// along with a boolean bMonsterInRoom, which tells us if we have an enemy in the room.
//
// - Next, we added some new functions to read in the player and monster data:
// GetMonsterInfo(), GetPlayerInfo() and DisplayPlayer().  
//
// - Being that we had more data to read in, more code was added to GetRoomInfo() for
// the monster data.
//
// - The "status" command was added to our list, enabling us to see our stats and info.
//
// - The attack loop was stored in AttackPlayer(), which is called in our main() game loop.
//
// - We now read in the player's stats in the beginning, before anything else.
//
// ------------------------------------------------
//
// Once you get here you must be excited to start going off and implementing your
// own ideas into your game, if not, I hope you aren't lost or think I such a coding. :)
// I can think of MANY different things you could do from here on out.  Let's list
// some ideas for where you go from here:
//
// 1) [Random damage taken off when you attack] - This would be easy to add, probably  
// in fact the first thing that I would add.  If you want to incorporate intelligence
// coordination and other variables into a equation, that is great!
//
// 2) [Allow the user to run or fight] - You don't always want to fight a monster when
// you go into a room.  Maybe they get one hit off of you, but then you get a chance to
// run or fight to the death.
// 
// 3) [Keep track of which monsters died so they don't reappear] - This is pretty 
// important to a good game.  It isn't fun if monsters always stay in the same place.
// Perhaps you could not have to add monsters in the text file, but make them random
// in the game, unless there was a special boss in certain rooms.
//
// 4) [Create an inventory system and add items to rooms] - You are most likely going
// to want to have a way to pick up items and store them.  This way you can increase
// your armor, weapon, health, rejuvenate, etc...
//
// 5) [Create a trap door system] - Maybe some rooms have a trap door in them, that if
// you enter, look at something special, pick up something, or kill a monster, you will
// then be transported to another room automatically.
//
// 6) [Put NPC's (None Player Characters - people) in the rooms to talk and interact with] - 
// A game is boring without people to talk to too, try adding some in, and have a "talk" command.
//
// 7) [Allow people to join your party] - Just like every role-playing game, it would be great
// to allow party members to join and fight with you.
//
// 8) [Create rooms that transport you to another game file] - You can have multiple worlds,
// with each one in their own text file.
//
// 9) [Allow the player to save and load] - No game is complete without saving and loading.
//
// 10) [Allow magic and spells] - You can get really creative with magic in your game.
//
//
// These ideas should make your game really fun!  If you finish those, try converting the
// game to ASCII graphics, so that you can actually walk around with the arrow keys.  Color
// is always a wonderful idea to make the text not so hard to read and more interesting.
// If you need any help implementing these things, send me an email and I will point you in
// the right place, even help out with the algorithm if needed.  Please let me know what you
// think of this game tutorial.
//
// Good Luck!
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
