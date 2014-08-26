//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii Map Editor (Part 2)						 //
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

// This tutorial is the second part in our ASCII based map editor series.  In this tutorial we
// go over the code to provide functionality that allows the user to insert different tile
// types in the map.  This could be either a normal tile, an item, npc or monster.  To make
// the selection of different tiles easy (as to not complicate the tutorial) I decided to
// have the user use these keys:
// 
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
// When inserting tiles, you have to recognize that if you hold down the mouse it will
// insert many tiles until you let go of the mouse.  So, if you only want to insert one tile,
// do not hold down the mouse.
// 
// * So, where does the tile info come from? *
// 
// If you look in this projects directory, you'll notice that there is three text files
// for the item, npc and monster info.  If you open those up, you will see something like:
// 
// monster > image: $ life: 32 strength: 5 speed: 1
// 
// That says that the monsters name is "monster" and the '>' character just is a spacer,
// we don't use it.  The image of that monster is the '$' character.  It's stats for
// life, strength and speed are 32, 5 and 1.  We read in the monster's name, image, life,
// strength and speed from the monsters.txt file.  The same goes for the other tiles.
// 
// If you have looked at the text adventure tutorial series you will have good idea on
// how to read in and parse data from a file, but in this series we use fscanf() to simply
// read in the data.  We also use fgets() to grab a string from a file.
// 
// Probably the hardest part of this tutorial is just reading in the data from the files,
// but that's not so bad, so don't worry about getting confused in this version.
// 
// In the next tutorial series we will focus on loading and saving maps.  This is also
// very simple, since all of the data is already set up and we just need to call some
// fread() and fwrite() functions to read and write the data.
// 
// Let's jump into the code!
//
//

// This is a global position for our prompt that we use to display the current tile's name.
// We create a prompt right below the main map characters, where there is black blank space.
COORD g_promptPos = {0, MAP_HEIGHT - EDITOR_HEIGHT};

// These variables are used to store the current index into the current list we are using.
// For instance, if we are selecting items, and we hit space bar to cycle through the
// items list, then we need a variable to store which item index we are looking at.
int g_ItemIndex		= 0;						// This is the index for the items list
int g_MonsterIndex	= 0;						// This is the index for the monsters list
int g_NpcIndex		= 0;						// This is the index for the npc index

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


vector<CTile> g_vTiles;							// Create a list for the map tiles to be loaded
vector<CItem> g_vItems;							// Create a list for the item tiles to be loaded
vector<CMonster> g_vMonsters;					// Create a list for the monster tiles to be loaded
vector<CNpc> g_vNpcs;							// Create a list for the Npc (citizen) tiles to be loaded

CMap *g_pCurrentMap = NULL;						// Create a pointer to the current map (for exits)
CMap g_Map;										// Create our main map variable

CTile *g_pCursorTile = NULL;					// This stores the current tile we have selected

COORD g_cursorPos = {0};						// This stores the current cursor X and Y position


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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// Below is the code that handles what happens when the user hits the keyboard.
		// What we usually do for selecting tile categories to place is just set the
		// cursor tile to the appropriate tile, then set the new tile type.

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

		// The exits aren't working yet, but let's set up the structure for when we
		// implement them in the fourth version of our tutorial series.  Since exits
		// have no information like items and monsters right away, we fill in it's basic
		// information and instead of using the g_vExits list, we just use the same tile
		// index as the normal tiles use, which is the last index into the g_vTiles array.
		// This is because there is no exit until we create one, so this just works out good.

		g_pCursorTile->SetChar(image);				// Set the image of our exit tile
		g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];	// Set the cursor tile to the last tile
		g_Map.SetCurrentType(EXIT_TYPE);			// Set the type of list to an exit type
	}								// If the user hits the space bar
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
	{	
		// In this part we want to cycle through the current list of tiles when the
		// user hits the space bar.  This is only applicable if the user has the item,
		// monster or npc list selected, otherwise the exit and normal tiles just do nothing.
		if(g_Map.GetCurrentType() == ITEM_TYPE)		// If we have items selected
		{
			// Here is where we do a little tricky math using the modulus character (%).
			// We want to increase the current index by 1 to cycle through, but we don't
			// want to have the index go over the size of the list, otherwise it would 
			// access a bad number into the array right?  Well, remember, arrays are 0 based
			// so we need to make sure the index never equals the size() of the list.  We can
			// do this by using the modulus operator.  What this does is return the remainder
			// of division.  So, if you do:  6 % 5 it should return a 1, since 6 / 5 is 1, with
			// a remainder of 1 right?  Well, when using the modulus operator, if the left hand
			// number is below the right hand number, it just returns the left hand number.
			// So , 1 % 5 = 1.  That means, when the left hand side becomes the same number
			// as the right hand side, it should return a 0:  5 % 5 = 0.  There's no remainder.
			// We can use this to set our index to: index = (index + 1) % size;  This will always
			// make sure that we never go over or become the size, because it will return 0, which
			// will start over again.  This is the same thing as just doing an if() statement that
			// checks if the current index is >= to the size, then setting it to 0.  However,
			// modulus is less code and a lot more practical.

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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// If we get here in the code, that means that we want to insert a tile
			// into the map.  First we check to see if there is even a tile attached to
			// the cursor, and if so, we pass in our cursor tile and it's inserted position.
			if(g_pCursorTile != NULL)
				g_pCurrentMap->InsertTile(g_pCursorTile, g_cursorPos.X, g_cursorPos.Y);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		}
	}

	// Here we check if the right mouse button was clicked
	if(InputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
	{		


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		
		// In this part we want to either delete a tile under the cursor, or delete
		// the cursor tile itself.  If there is no cursor tile, then we assume they
		// want to delete a tile on the map.  If there is a cursor tile, we just set
		// the cursor tile to NULL.  Remember, we only can delete items, npcs and monsters,
		// and only when we have that category selected by hitting (M, N, or I).
		if(g_pCursorTile == NULL)				
		{				
			// Delete a tile under our cursor position on the map
			g_pCurrentMap->DeleteTile(g_cursorPos.X, g_cursorPos.Y);
		}
		else									
			g_pCursorTile = NULL;	

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// In this file we added the functionality to choose which tile type
// want to insert into the map, as well as the ability to insert or
// delete a tile.
// 
// Here is the commands for this version:
//
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
// The next tutorial in this series will go over loading and saving maps.
// Can you barely wait!!??!!??!  You are almost done!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
