//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii Map Editor (Part 3)						 //
//																		 //
//		$Description:	A simple ASCII text based map editor			 //
//																		 //
//***********************************************************************//

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include "main.h"								// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the third part in our ASCII based map editor series.  In this tutorial we
// go over the code to provide functionality that allows the user to Load() and Save() .map
// files.  This is done by the 'L' and the 'S' keys.  By pressing these the user will be
// able to type in a file name to load or save.  If the file can't be found, a message box
// will pop up and say there was an error.  Here are the keys for this tutorial:
// 
// L - This prompts the user to load a file.
// S - This prompts the user to save the file.
// I - This selects the item tiles (the character 'I' is displayed to represent an item).
// N - This selects the NPC tiles (like citizens, shopkeepers, etc...) with smiley faces.
// M - This selects the monster tiles, which can be any character to represent them since
//     these characters are defined in the Monsters.txt file in the "image:" spot.
// Space_Bar - This cycles through the different tiles, once that category is chosen.
// Left_Mouse_Button - This inserts a tile into the map
// Right_MouseButton - This first deletes the current cursor tile, and then deletes tiles
//					   under the cursor on the map.  If you want to delete a monster tile,
//					   you need to have the monster tiles category chosen (M key).  It will
//					   not delete a tile if that tile is not part of the chosen category.
//  
// * So, how does saving and loading work? *
// 
// Well, it's best understood if you go look at the code, but here is an overview of the
// logic that we will use.  Okay, so you know how our map has lists of tiles right?  For
// instance, there is a m_vItems, m_vMonsters, ... list right?  Well, those lists store 
// all of the tiles on the map, every last one.  Since we don't have any complicated
// data stored, all we need to do is just go through a for() loop for each list, where it
// uses a function like fwrite() to write to a file (in the case of saving) or read from
// the file using fread() to read from the file (in the case of loading).  If we save
// in the same format and the same order, we load the file in exactly the same order and
// format.
//
// * So how do you know how much to load? *
// 
// This is important for nearly every type of format - When should you stop??  Well,
// before you load the file, in order to know how much memory to allocate so you can
// efficiently load in the file, you create what we call a "header".  This head stores
// the data which tells us information about the file, not the actual data.  For
// example, for our .map file we create a header that stores the sizes for each list.
// Here is an example of a line of code for one part of the header:
//
// fwrite(&tileSize, sizeof(int), 1, fp);
//
// That just writes to the current open file (fp) an integer that stores how many
// tiles are in the map.  This is of course normal tiles.  We would write other
// sizes for the items, monsters, npcs and the exits.  We write them in the same
// order as we would read them in.  Check out CMap::Save() for the order we write it.
//
// So, once we read in all the sizes for the lists, we can allocate memory for our
// lists before we read in the data.  The same thing works for bitmaps and other
// file formats.  You need to know how many pixels (the size of the picture) you will
// read, as well as how many vertices to read in for a 3D model.  Some file formats
// use things calls "chunks".  You can check out our .3ds loader tutorial in the OpenGL
// section to see how chunks work.
//
// Next, once we know the number of tiles that we are going to load, we just do a
// loop to read in that data, or if you have a normal array you (not STL vectors)
// you can just read it all in with one call to fread().
//
// If you want to test the loading function, you can load the Default.map file
// that is included in this tutorial.  This is a simple example of some graphic
// effects you can do with ASCII.  Please, ... go and do likewise :)
//
// This tutorial doesn't need too much explanation since not a lot was added, so
// let's do a back flip into the Chucky Cheese balls of code :)
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


vector<CTile> g_vTiles;							// Create a list for the map tiles to be loaded
vector<CItem> g_vItems;							// Create a list for the item tiles to be loaded
vector<CMonster> g_vMonsters;					// Create a list for the monster tiles to be loaded
vector<CNpc> g_vNpcs;							// Create a list for the Npc (citizen) tiles to be loaded

CMap *g_pCurrentMap = NULL;						// Create a pointer to the current map (for exits)
CMap g_Map;										// Create our main map variable

CTile *g_pCursorTile = NULL;					// This stores the current tile we have selected

COORD g_cursorPos = {0};						// This stores the current cursor X and Y position
COORD g_promptPos = {0, MAP_HEIGHT - EDITOR_HEIGHT};	// This stores the prompt starting position

int g_ItemIndex		= 0;						// This is the index for the items list
int g_MonsterIndex	= 0;						// This is the index for the monsters list
int g_NpcIndex		= 0;						// This is the index for the npc index


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our application-specific data
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init()
{
	// Set the map to default
    g_Map.SetDefault();

	// By default set the current tile type to map tiles
	g_Map.SetCurrentType(TILE_TYPE);

	// Set our current map to ....  our current map :)
	g_pCurrentMap = &g_Map;
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// LOAD OR SAVE A MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the prompt to load or save a map file
/////
///////////////////////////////// LOAD OR SAVE A MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadOrSaveAMap(bool bLoad)
{
	string strMapName = "";		// Create a STL string to read in the input from the user
	char szName[255] = {0};		// Create a traditional string of characters for the file name

	// In this function we setup a prompt for the user to type in the file name to
	// either save or load.  If you want to load a file, you pass in "true", otherwise
	// we use "false" to indicate that we want to save a file.  This cuts down on code
	// since a lot of the code is the same for each action.  This function doesn't
	// actually do the saving and loading code, but handles the input from the user.
	// The save and load code is in our CMap::Load() and CMap::Save() functions that
	// we call from here.

	// Get an output handle for the console window and set the cursor position for a prompt
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// To clear the prompt, we just print out a bunch of tabs and then reset the cursor position
	cout << "\t\t\t\t\t\t\t\t\t\t";
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// Get an input handle and set the console window to allow input from the user 
	// and echo it to the screen.  This is important.  If you don't set this mode, 
	// the user won't see what they are typing.
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);			
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);

	// Below we check if we want to load or save a file, then display the appropriate message
	if(bLoad)
		cout << "Enter a map to Load: ";
	else
		cout << "Enter a map to Save: ";

	// Here we get the map name that we want to save or load
	cin >> strMapName;

	// Then we extract the string from the string class and put it in a normal array or characters
	strcpy(szName, strMapName.c_str());

	// Next we either load or save the map entered by the user, depending on bLoad
	if(bLoad)
		g_Map.Load(szName);
	else
		g_Map.Save(szName);

	// Next, we restore the console mode for getting mouse and keyboard input and redraw the screen
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
	g_Map.Draw();	
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// CHECK KEYBOARD INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the actions when the user types a key from the keyboard
/////
///////////////////////////////// CHECK KEYBOARD INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CheckKeyboardInput(const INPUT_RECORD &InputRecord)
{				
	// If the user hits the escape key
	if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
	{												
		exit(0);					// Quit the program				
	}								// If the user hits the I key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'I')
	{										
		g_ItemIndex = 0;							// Reset the current index for the item list
		g_pCursorTile = &g_vItems[g_ItemIndex];		// Set the cursor tile to the first item
		g_Map.SetCurrentType(ITEM_TYPE);			// Set the type of list to an item type
	}								// If the user hits the M key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'M')
	{	
		g_MonsterIndex = 0;							// Reset the current index for the monster list
		g_Map.SetCurrentType(MONSTER_TYPE);			// Set the type of list to an monster type
		g_pCursorTile = &g_vMonsters[g_MonsterIndex];  // Set the cursor tile to the first monster
	}								// If the user hits the N key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'N')
	{										
		g_ItemIndex = 0;							// Reset the current index for the npc list
		g_pCursorTile = &g_vNpcs[g_ItemIndex];		// Set the cursor tile to the first npc
		g_Map.SetCurrentType(NPC_TYPE);				// Set the type of list to an npc type
	}								// If the user hits the E key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'E')
	{	
		CHAR_INFO image = {'E', FOREGROUND_RED};	// Create an image for exits

		g_pCursorTile->SetChar(image);				// Set the image of our exit tile
		g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];	// Set the cursor tile to the last tile
		g_Map.SetCurrentType(EXIT_TYPE);			// Set the type of list to an exit type
	}								// If the user hits the space bar
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
	{	
		if(g_Map.GetCurrentType() == ITEM_TYPE)		// If we have items selected
		{
			// Use the modulus operator to restrict our index to the size of the item list,
			// then set the cursor tile to the current tile in the item list.
			g_ItemIndex = (g_ItemIndex + 1) % (int)g_vItems.size();
			g_pCursorTile = &g_vItems[g_ItemIndex];
		}
		else if(g_Map.GetCurrentType() == MONSTER_TYPE)	// If the user has the monsters selected
		{
			// Use the modulus operator to restrict our index to the size of the monster list,
			// then set the cursor tile to the current tile in the monster list.
			g_MonsterIndex = (g_MonsterIndex + 1) % (int)g_vMonsters.size();
			g_pCursorTile = &g_vMonsters[g_MonsterIndex];
		}
		else if(g_Map.GetCurrentType() == NPC_TYPE)		// If the user has the npcs selected
		{
			// Use the modulus operator to restrict our index to the size of the npclist,
			// then set the cursor tile to the current tile in the npc list.
			g_NpcIndex = (g_NpcIndex + 1) % (int)g_vNpcs.size();
			g_pCursorTile = &g_vNpcs[g_NpcIndex];
		}
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	// Now that we can load and save, let's add the keyboard functionality to
	// allow the user to do that.  This is done with the 'L' and 'S' key.

	// Check if we want to load a new map (pass in true to LoadOrSaveAMap())
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'L')
	{												
		LoadOrSaveAMap(true);						
	}
	// Check if we want to save the current map (Pass in false to LoadOrSaveAMap())
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'S')
	{												
		LoadOrSaveAMap(false);
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}

///////////////////////////////// CHECK MOUSE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the actions when the user uses the mouse
/////
///////////////////////////////// CHECK MOUSE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CheckMouseInput(const INPUT_RECORD &InputRecord)
{
	// Get the current position of the mouse in the console window
	g_cursorPos = InputRecord.Event.MouseEvent.dwMousePosition;

	// If the user hits the left mouse button
	if(InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
	{	
		// Here we check if the user chose a tile from our editor.  
		if(g_cursorPos.Y > MAP_HEIGHT - EDITOR_HEIGHT)
		{		
			// Get the character chosen from the editor
			CHAR_INFO chosenTile = g_Map.GetEditorSelection(g_cursorPos.X, g_cursorPos.Y);	
			CHAR_INFO cursorTile = chosenTile;

			// If we already have a tile selected, let's grab it's image instead 
			if(g_pCursorTile != NULL)
				cursorTile = g_pCursorTile->GetChar();
					
			// Check if the user clicked the color tiles
			if(g_cursorPos.X >= 15 && g_cursorPos.Y == (MAP_HEIGHT - 1))
			{
				CHAR_INFO newTile;				// Create a new tile to work with for color

				// Check if the user clicked the background colors, otherwise it was foreground
				if(g_cursorPos.X > 30) 
					newTile = g_Map.AddNewBackground(cursorTile, chosenTile);
				else
					newTile = g_Map.AddNewForeground(cursorTile, chosenTile);
					
				// Apply the new tile with it's new color to our cursor.  
				g_vTiles[g_vTiles.size() - 1].SetChar(newTile);
			}
			else
			{
				// Otherwise, if we didn't choose a color, then we must 
				// have chosen a new tile character from the editor.  
				chosenTile.Attributes = cursorTile.Attributes;
				g_vTiles[g_vTiles.size() - 1].SetChar(chosenTile);

				// Let's set the cursor to a normal tile index and type
				g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];
				g_Map.SetCurrentType(TILE_TYPE);
			}
		}
		else
		{
			// If we have a tile, let's insert a tile into the map
			if(g_pCursorTile != NULL)
				g_pCurrentMap->InsertTile(g_pCursorTile, g_cursorPos.X, g_cursorPos.Y);
		}
	}

	// Here we check if the right mouse button was clicked
	if(InputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
	{		
		// Here we either delete a tile on the map, or delete the cursor tile
		if(g_pCursorTile == NULL)				
		{				
			// Delete a tile under our cursor position on the map
			g_pCurrentMap->DeleteTile(g_cursorPos.X, g_cursorPos.Y);
		}
		else									
			g_pCursorTile = NULL;
	}
}


//////////////////////////////// CHECK INPUT /////////////////////////////////
/////
/////	This check for input from the user
/////
//////////////////////////////// CHECK INPUT /////////////////////////////////

bool CheckInput()						
{													
	INPUT_RECORD InputRecord;							// This structure holds our user input details
	DWORD Events=0;										// This holds the number of events for input
	int bKeyDown = 0;									// This tells us if we pressed DOWN a key
				
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);		// Create a handle for checking input
														
	// Read in the input from the user, storing the information into the InputRecord structure.
	ReadConsoleInput(hInput, &InputRecord, 1, &Events);
													
	// For Windows 2000 and up we need to check the keydown so we don't move twice
	bKeyDown = InputRecord.Event.KeyEvent.bKeyDown;

	// If we just hit a key from the keyboard, handle the input accordingly.
	if(InputRecord.EventType == KEY_EVENT && bKeyDown)
	{
		CheckKeyboardInput(InputRecord);
		return true;
	}

	// If we used the mouse in any way, let's handle those events
	if(InputRecord.EventType == MOUSE_EVENT)		
	{
		CheckMouseInput(InputRecord);
		return true;
	}

	// We didn't do anything so don't draw again
	return false;									
}


//////////////////////////////// MAIN /////////////////////////////////
/////
/////	This is our program's entry point, the MAIN() function :)
/////
//////////////////////////////// MAIN /////////////////////////////////

int main()
{
	// Initialize our data
	Init();

	// Draw the map to the screen
	g_Map.Draw();

	// Do an infinite loop until we exit the program by ESC or Control-C
	while(1)
	{
		// If we used the mouse or the keyboard, let's draw the map again
		if(CheckInput())
		{
			// Draw our current map that is open
			g_pCurrentMap->Draw();
		}
	}
	
	// Return a success with no problems
	return 1;
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// That's it!  Not much added to this file huh?  We added a LoadOrSaveAMap()
// function that just prompts the user to enter a file name to either
// save or load.  We then added checks for when the user hits the 'L' or 'S'
// key to load or save their map.
// 
// The next tutorial adds support for adding exits to our maps.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
