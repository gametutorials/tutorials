//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		RPG (Part 6)									 //
//																		 //
//		$Description:	A basic example of a 2D RPG						 //

#include "Main.h"


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This file holds the 2 functions that allows us to load and save game files.
// Each one of them are public and part of our global g_SaveLoad object.
// We save our game files as .sav files.
//
//


///////////////////////////////// LOAD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads a saved game file from a .sav file
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
	
	// Next we can assign the action keys array to the current one
	g_ActionKeys.SetActionKeys(actionKeys);

	// Since we have the map name, let's load the last map the player was at
	g_Map.Load(szMapName);

	// Before loading the player info, let's save the players image so we don't have to load it
	HBITMAP hPlayerImage = g_Player.GetImage();

	// Before loading in the player, let's erase all it's data to make it pure :)
	memset(&g_Player, 0, sizeof(CPlayer));

	// Restore the image now, since it won't be loaded when we call LoadPlayer()
	g_Player.SetImage(hPlayerImage);

	// Now we just call the player's load function and bla-bling! - Done
	g_Player.LoadPlayer(fp);

	// Close our file that we opened, a success!
	fclose(fp);

	// The load was a success
	return true;	
}


///////////////////////////////// SAVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This saves a game file as a .sav file
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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// The functions in this file handle the saving and loading of our
// .sav game files.  Each one of these functions are public and can 
// be used from the g_SaveLoad global object.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials  
