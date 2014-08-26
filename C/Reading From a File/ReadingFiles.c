//**********************************************************************************//
//																					//
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -					//
//																					//
//		$Author:		Ben Humphrey	digiben@gametutorials.com					//
//																					//
//		$Program:		ReadingFiles											    //
//																					//
//		$Description:	This gives several ways to read from a file				    //
//																					//
//**********************************************************************************//


// In this tutorial we show how to read data from a file.  This is very useful for reading
// data that was previously saved and needs to be loaded back in to a progam.  For example, 
// if we are making a game we need to save and load the player's data and such so we know 
// the player's last position, health and level.  Also, when writing a text editor we need 
// to read in text files. We show you in this tutorial how to load a player's name, health 
// and gold from a text file.  Take a look below to read our tutorial on reading from files.


// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>										// We need to include this for printf() and fgetc().

void main()												// A standard and plain main (hey! that rhymed!) :)
{														// Start of our program
	FILE *fp;											// This new variable is for reading in files.  'fp' stands for filepointer.
	char ch;											// We need a pointer because fopen() returns an ADDRESS to a file.
	char szLine[80]={0};								// We then take that address, which is where in memory the file is stored, and read from it.
	int health=0, gold = 0;								// There are numerous file functions, but fopen is the simplest, and the worst.
														// "fopen()" loads the whole file into memory.  If the file is really big, this is a bad idea.
														// In other words, a MEMORY HOG! :)
														// We create a character to read from the file character by character, letter by letter, or number by number :)
														// We also will create a string of 80 characters long.  Remember, use {0} to initialize arrays.
														// I created 2 integers which we will use later for reading in the gold and health of the player.
														
	fp = fopen("Stats.txt", "r");						// Here we receive the address of where the file resides in memory.  "fopen()" returns the address.
														// fopen() needs a file name for the first parameter, and a letter.
														// There are MANY letters you could type in.  "r" is read-only (which we are doing).
														// If we wanted to write to the file, we would use "w" for 'write'.
														// If we want to read or write in binary we would use "rb" or "wb".  There are a bunch more too.. Check MSDN (Online Help).

														
	if(fp == 0)											// But before we start reading .. we need to check IF there is even a file there....
	{													// If fp == 0 it means fopen() couldn't find the file, or some other error.  Remember it needs to be in the same directory as the .EXE or project.
		printf("ERROR: Could not find Stats.txt!\n");	// Print out an error message.  This is very important, especially when getting into huge projects.
														// There is so many times where I have spent wasted time trying to find out why my program was crashing.
		return;											// Return is like a 'break', but it's for functions, not loops.  We just can't return something to it.  
	}													// Return will quit from the current function, in this case it's main.  So the program is now ended.
														
	printf("\n");										// Just to space things out when we display this to the screen.

														// Here we just do a while loop to get every character in the file.  EOF stands for "End Of File".
	while((ch = fgetc(fp)) != EOF)						// fgetc() takes the 'fp' (file pointer) and returns a character at a time.  "ch" then receives the character.
		printf("%c", ch);								// Here we print out the character that "ch" equals.  This results in printing the whole documents to the screen.

														// At the end of every file the computer puts a symbol to show it's the end of the file.
														// That character is '\0',  (Null Terminator) like on the end of strings.
														// In the system header files it's #define'd as EOF, meaning End-Of-File.
														// I put parenthesis around (ch = fgetc(fp)) so that the result of the statement could then be checked against EOF.
														// Remember, '!=' means "does not equal".  So the while loop will continue until we hit the EOF.

	printf("\n");										// Just to space things out when we display this to the screen.

	rewind(fp);											// Remember we used this to clear the INPUT buffer?
														// We can now use this to set the file pointer to the beginning of the file again.
														
														// This is another way to read from a file.  "fgets()" gets a bunch of characters at a time.
	while(fgets(szLine, 80, fp))						// "fgets()" needs (a string to store the characters read from the file, how many characters to read at a time, and the file pointer)
		printf("%s", szLine);							// I chose 80 because that is a WHOLE line, so we are reading a line at a time.  fgets() stores the amount of characters you chose in "szLine".
														// We then print out the 80 characters stored in szLine.  Remember "%s" stands for string.
														// This loop will quit when fgets() returns NULL or EOF.  (NULL means nothing or no value...)
														// We can just put the function in the while loop like this "while(fgets(etc))" because
														// it's going to be returning strings.  Once it hit's the end of the file, it will return EOF (End-Of-File).
														// EOF is a NULL character, which in a statement, amounts to zero.  So while() will fail and the program will quit after printing the file to the screen.
	
	printf("\n");										// Just to space things out when we display this to the screen.

	rewind(fp);											// Rewind the file pointer back to the beginning so that I can show you another way to read information.
														
														// Remember scanf()?  fscanf() is just like scanf(), but it's reading from a FILE, and you can specify what you want to read in.
														// fscanf() needs (the file pointer, the string to look for and a "%<type>" to read in", and a variable to store the information).
														// fscanf() is a cool function.  If we just want to get our player's stats, and now read in character by character...
														// We just give it some text to look for in the file, like "Player: ", which is in our file.. then we know there is a name AFTER "Player:".
														// So we tell fscanf() that we want to get the name by putting a "%s" (which means a string) after the "Player:" like so: "Player: %s\n"
														// We want to put a "\n" also after the %s because that tells fscanf() to go to the next line of the file.
														// Now, the file pointer points to the next line in the file.  By the way, when anything is read in with a file pointer,
														// The file-pointer is incremented (meaning increased) the amount it read in.  If it reads in a character, it's now looking at the next character.
														// The file-pointer never goes back unless you rewind() it, or close and reopen the file.
	fscanf(fp, "Player: %s\n", szLine);					// Here we read in the Name of the player.
	fscanf(fp, "Health: %d\n", &health);				// Here we read in the "health" of the player.  Notice the "&" (address) operator.  We need to give fscanf() the memory location of "health" because it needs to change it.
	fscanf(fp, "Gold: %d\n", &gold);					// Here we read in the "gold" of the player into the variable "gold".  We also use the "&" to give permission (the memory address) to change the variable "gold".

														// Notice there isn't a "&" before the string "szLine"?  You don't need to put "&" before an array, it is already considered a pointer in memory.
														// And since a string is an array of characters, it's considered a pointer.  It's confusing.
														// It will just have to be one of those things you except for now.  But search it out!

	printf("The Player's name is:   %s\n", szLine);		// Here we print out the Player's name that we retrieved from the file.
	printf("The Player's health is: %d\n", health);		// Now we print out the Health of the player that we got from the "Health: 50" part of the file.
	printf("The Player's gold is:   %d\n", gold);		// Then we print out the amount of gold the player has, which we retrieved from the line "Gold: 20" in the file "stats.txt".

														// fclose() passes in the file pointer and closes the file for us. (very important)
	fclose(fp);											// We must always close the file after we are done with it.  It frees memory and makes it so we can access it again.

}														// End of our program

//////////////////////////// Quick notes on fscanf() //////////////////////////

// We could also do this with fscanf():
														// If we set our file up like this: "Player: Adol 50 20"
// fscanf(fp, "Player Info: %s %d %d", szName, &health, &gold);
														// We could just read it in on one line.
														// Here is another cool thing with fscanf()... spaces in a file don't matter...
														// Say we have a file that looks like this "Player:         Adol"
// fscanf(fp, "Player: %s", szLine);					// This will read in that line.  fscanf() searches for a space and keeps looking until it finds a character.	
														// It doesn't matter if it's tabs or spaces, you don't need to be accurate, just include ONE space and it will search until it gets to text.
														// Cool huh?  This comes in very useful for reading in player save files and config information.
														// I believe you could also do this:
// fscanf(fp, "Player: %s\nHealth: %d\nGold: %d\n", szLine, &health, &gold);
														// It will read in our whole file in just one line.  Once again, "\n" means a new line.  Every text file has \n at the end of each line to let programs know the line is finished.

// © 2000-2005 GameTutorials
