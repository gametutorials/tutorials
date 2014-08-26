//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii RPG (Part 6)								 //
//																		 //
//		$Description:	An RPG with ASCII graphics						 //
//																		 //
//***********************************************************************//

#include "Main.h"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This file holds the 3 functions that allows us to load and save game files.
// Each one of them are public and part of our global g_SaveLoad object.
// We need a way to input the file to save or load, so we added a prompt
// function, which uses our menu class to get the input from the user.
// The user can only input normal letters and numbers.  They can backspace too.
//
//


///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a saved game file
/////
///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CSaveLoad::Load(char *szFile)
{
	// Here we add the .sav file extension to the game file's name
	if(!strstr(szFile, kSaveExtension))
		strcat(szFile, kSaveExtension);

	// Open our map that we want to load in "read" and "binary" mode
	FILE *fp = fopen(szFile, "rb");

	// If we couldn’t load the file, report an error message
	if(!fp)
	{
		MessageBox(NULL, "Can't Find File", "Error", MB_OK);
		return false;
	}

	char szMapName[MAX_PATH] = {0};
	int actionKeys[kMaxActionKeys] = {0};

	// The first thing we have in the save game file format
	// is the map name that the player was on, then the action
	// keys array.  Let's load those in now.
	fread(szMapName, sizeof(char), MAX_PATH, fp);
	fread(&actionKeys, sizeof(int), kMaxActionKeys, fp);
	
	// Since we have the map name, let's load the last map the player was at
	g_Map.Load(szMapName);

	// Next we can assign the action keys array to the current one
	g_ActionKeys.SetActionKeys(actionKeys);

	// Before loading in the player, let's erase all it's data to make it pure :)
	memset(&g_Player, 0, sizeof(CPlayer));

	// Now we just call the player's load function and bla-bling! - Done
	g_Player.LoadPlayer(fp);

	// Close our file that we opened, a success!
	fclose(fp);

	// The load was a success
	return true;	
}


///////////////////////////////// SAVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This saves a game file
/////
///////////////////////////////// SAVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CSaveLoad::Save(char *szFile)
{
	 // Check if the file doesn't have a .map extension.  If not, add it at no extra charge :)
	if(!strstr(szFile, kSaveExtension))
		strcat(szFile, kSaveExtension);

	// Open our file to "write" and in "binary".
	FILE *fp = fopen(szFile, "wb");

	// If we couldn’t create or open the file report an error
	if(!fp)
	{
		MessageBox(NULL, "Can't Write File", "Error", MB_OK);
		return false;
	}

	// The first thing we save is the current map's name and the action keys array
	fwrite(g_Map.GetMapName(), sizeof(char), MAX_PATH, fp);
	fwrite(g_ActionKeys.GetActionKeys(), sizeof(int), kMaxActionKeys, fp);
	
	// Lastly, we just call the player's save function and...  we're done :)
	g_Player.SavePlayer(fp);

	// Close the file, the file is saved!
	fclose(fp);

	// We saved the file successfully
	return true;
}


///////////////////////////// PROMPT FOR FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This prompts the user to type in a file name to either save or load
/////
///////////////////////////// PROMPT FOR FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CSaveLoad::PromptForFile(bool bIsLoading)
{
	// What this function does it pop up a dialog box with
	// a cursor to allow the user to type in a name.  That name
	// will be used to either load or save a file.  This depends
	// on bIsLoading.  If it's true, then we load, otherwise save it.
	// We only will allow the user to type in numbers or characters.

	// Since this is a menu, let's reset the current menu data before we start
	g_Menu.ResetButtons();
	g_Menu.ResetMenuChoice();

	// Here we draw a dialog box to surround the prompt
	g_Menu.DrawBox(kLoadPromptWidth, kLoadPromptHeight, kLoadPromptX, kLoadPromptY);

	// Let's give the cursor position a padding of 2 away from the left side of the prompt
	int currentX = kLoadPromptX + 2;
	char szFile[MAX_PATH] = {0};

	// To display a cursor, we just have a '_' character after the string.
	// As the user types in characters we will add the _ character afterwards.
	g_Menu.DrawString("_", 1, currentX, kLoadPromptY + 2);

	while(g_Input.GetKeyCode() != VK_ESCAPE && g_Input.GetKeyCode() != VK_RETURN)
	{
		// If the user didn't do any input, just continue and keep looping
		if(!g_Input.CheckInput())
			continue;

		// If the user hit a key
		if(g_Input.IsKeyboardEvent() && g_Input.IsKeyDown())
		{
			// We restrict the user to only type in normal letters and numbers
			if((g_Input.GetKeyCode() >= 'A' && g_Input.GetKeyCode() <= 'Z') ||
			   (g_Input.GetKeyCode() >= '0' && g_Input.GetKeyCode() <= '9'))
			{
				// Let's not add any more characters to the file name if it's bigger than 30
				if((int)strlen(szFile) > 30)
					continue;

				//let's create a string and add the character they typed in, lowercase
				char szInput[3] = {0};
				sprintf(szInput, "%c", g_Input.GetKeyCode());

				// We only want to lowercase the character if it's a letter, but
				// if it's a number we don't want to do anything.  isalpha() tells
				// us if the character is a letter or not.
				if(isalpha(szInput[0]))
					szInput[0] = _tolower(szInput[0]);

				// Now we need to add the cursor '_' to the end of the current name
				strcat(szFile, szInput);
				strcat(szInput, "_");
				
				// Now we draw the current string to the prompt and increase it's x position
				g_Menu.DrawString(szInput, 2, currentX, kLoadPromptY + 2);
				currentX++;
			}
			// If the uses hits the back space key, let's erase the last character
			else if(g_Input.GetKeyCode() == VK_BACK)
			{
				// Get the current index into the file string
				int currentIndex = currentX - (kLoadPromptX+2);

				// Check so we don't go outside the bounds of our string array
				if( currentIndex > 0)
				{
					// Delete the last character in the file by putting the
					// NULL character at the end.  Remember that there is always
					// an extra character on strings, the \0 special NULL character.
					// If we delete a character, let's decree the position and draw a _.
					szFile[currentIndex - 1] = '\0';
					currentX--;
					g_Menu.DrawString("_", 2, currentX, kLoadPromptY + 2);
				}
			}
		}
	}

	// If the user hit return and not escape, we want to load or save that file name
	if(g_Input.GetKeyCode() == VK_RETURN)
	{		
		if(bIsLoading)
			Load(szFile);
		else
			Save(szFile);
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// The functions in this file handle the main interface for
// loading and saving game files.  Each one of these functions are
// public and can be used from the g_SaveLoad global object.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
