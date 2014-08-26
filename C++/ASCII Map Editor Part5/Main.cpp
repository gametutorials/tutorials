//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii Map Editor (Part 5)						 //
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

// This is the final tutorial in our Ascii Map Editor series.  In this version we added
// the ability to add action keys, added collision detection and a way to copy a normal
// tile from the map and stick it to your cursor.  To add collision detection and action
// keys, we just added 2 variables to our CTile class so every class that inherits from
// that class will also have collision and action key data assigned to them.
//
// * What's an action key? *
// 
// This is just a number that you can assign to a tile, like 466, which is used in your
// game to trigger events.  For instance, let's say that we want to have in our RPG game
// a cut scene (animation scene) when our character maybe steps on a certain tile or
// picks up a certain item.  Maybe a trap might open when you walk over a certain place,
// or if you go into a certain room you want something to happen.  This is all done with
// action keys.  In your game you check if there is an action associated with a tile,
// then act accordingly.  You would have an array that stores if those actions have already
// happened or not.  If you check out our Ascii RPG "GameTutorial" tutorial series, you can
// see the implementation of this.  To add an action key to a tile, you just press 'A' when
// you have a tile attached to your cursor.  You must set the action key before placing a tile.
//
// * How does collision work in our editor? *
//
// This is very simple, since we added a m_bSolid boolean variable to our CTile, we just
// need to set that to true or false.  In our game we check to see if a tile is solid
// before allowing a player to walk over it.  It's that simple.  To make a tile solid,
// you can do 2 things.  1) You can hold down shift when selecting a tile from the editor.
// This will make the tile (only a normal tile) solid.  Every tile you place will keep the
// solid attribute.  If you want to get rid of that attribute, just click on the same tile
// or another tile from the editor without holding down shift.  2)  If you placed tiles
// already and want to make them solid, you can simply make sure you have the TILE_TYPE
// mode selected, get rid of the attached cursor tile and hold down shift and click the
// placed tiles on the map.  This will make every tile you click with shift held down solid.
// Remember, you can only do this when in TILE_TYPE mode and you must not have any tile
// attached to the cursor.
//
// If you are in the normal TILE_TYPE mode and don't have an attached cursor tile, you can
// right click on a tile placed on the map and it will copy that tile to your cursor.
//
// * A new prompt display? *
//
// Yes, we now display a lot more information in our prompt about tiles.  If you select a
// mode and destroy the cursor tile by right clicking, when you go over a tile on the map
// with the cursor it will display some information about that tile.  It will only display
// info in the prompt about tiles that are the same type as the selected mode.
//
// Let's go over the controls for this version:
//
// A - This prompts the user to set the cursor tile's action key.
// L - This prompts the user to load a file.
// S - This prompts the user to save the file.
// I - This selects the item tiles (the character 'I' is displayed to represent an item).
// N - This selects the NPC tiles (like citizens, shopkeepers, etc...) with smiley faces.
// M - This selects the monster tiles, which can be any character to represent them since
//     these characters are defined in the Monsters.txt file in the "image:" spot.
// Space_Bar - This cycles through the different tiles, once that category is chosen.
// Left_Mouse_Button - This inserts a tile into the map.
// Right_MouseButton - This first deletes the current cursor tile, and then deletes tiles
//					   under the cursor on the map.  If you want to delete a monster tile,
//					   you need to have the monster tiles category chosen (M key).  It will
//					   not delete a tile if that tile is not part of the chosen category.
//				     - If you are in TILE_TYPE mode and have no attached cursor it will copy a tile
// SHIFT - This allows the user to place many exits before switching over to the destination map.
//       - If you hold down shift and click on normal tiles in TILE_TYPE mode it will set them solid.
// ESC - Quits
//
// * IMPORTANT * It is important to realize that you can't load a previously created map
// in a previous tutorial because we added new variables to our CTile class.  The data is
// not setup exactly the same so the map will not load correctly.  It's important to always
// have your classes and structures the same in your map editor and your game.  If you just
// HAVE to have an old map you created, just create 2 sets of the classes and load in using
// the old classes, then save and copy the data to the new updated classes and save it.
// It's a dirty fix for saving old data :)  It comes in handy when you realize you need to
// add new data to your game/editor, but have already created the levels/maps.
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


vector<CTile> g_vTiles;							// Create a list for the map tiles to be loaded
vector<CItem> g_vItems;							// Create a list for the item tiles to be loaded
vector<CMonster> g_vMonsters;					// Create a list for the monster tiles to be loaded
vector<CNpc> g_vNpcs;							// Create a list for the Npc (citizen) tiles to be loaded

CMap *g_pCurrentMap = NULL;						// Create a pointer to the current map (for exits)
CMap g_Map, g_ExitMap;							// Create our main map variable and exit map variable

CTile *g_pCursorTile = NULL;					// This stores the current tile we have selected

COORD g_cursorPos = {0};						// This stores the current cursor X and Y position
COORD g_promptPos = {0, MAP_HEIGHT - EDITOR_HEIGHT};	// This stores the prompt starting position

int g_ItemIndex		= 0;						// This is the index for the items list
int g_MonsterIndex	= 0;						// This is the index for the monsters list
int g_NpcIndex		= 0;						// This is the index for the npc index

bool g_bShiftKeyDown = false;					// This tells us if SHIFT is down or not
bool g_bExitMapOpen = false;					// This tell us if our exit map is open


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

///////////////////////////////// INPUT ACTION KEY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function allows the user to input an action key for the current cursor tile
/////
///////////////////////////////// INPUT ACTION KEY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void InputActionKey()
{
	// Create a variable to hold the action key read in from the user
	int actionKey = 0;

	// Get a console output handle, then set the cursor position for our prompt like normal
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// Get a console input handle and set the console to allow keyboard input echoed to the screen
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);			
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);

	// Display our prompt and read in the action key from the user
	cout << "Enter a numeric action key for this tile: ";
	cin >> actionKey;

	// Now we just set the current cursor tile with the action key
	g_pCursorTile->SetActionKey(actionKey);

	// Restore the console mode so that we can use the mouse
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// GET EXIT FILE NAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a prompt for us to input a destination map for exits
/////
///////////////////////////////// GET EXIT FILE NAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool GetExitFileName(char *szName)
{
	// Create a string variable to read in the destination map name
	string strMapName = "";
	
	// Get a console output handle, then set the cursor position for our prompt like normal
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// Get a console input handle and set the console to allow keyboard input echoed to the screen
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);			
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);

	// Display our prompt and read in the destination map name from the user
	cout << "Enter a map to load for the destination: ";
	cin >> strMapName;

	// Extract the map name from the string class variable and store it in
	// a normal char array, then reset the console mode to allow mouse input.
	strcpy(szName, strMapName.c_str());
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

	// This returns a true or false, which tells us if the user typed in a valid map name.
	// I chose a check of 3 characters, which means the map must be at least 3 characters long.
	return (strlen(szName) > 3);
}


///////////////////////////////// HANDLE EXIT TILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles what happens when we set an exit
/////
///////////////////////////////// HANDLE EXIT TILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void HandleExitTiles()
{
	// If we don't have the exit map open
	if(!g_bExitMapOpen)
	{
		char szExitFile[255] = {0}; 
		
		// Bring up the open dialog box and get a map to exit to
		if(GetExitFileName(szExitFile))
		{
			g_ExitMap.Load(szExitFile);					// Open the map to exit to
			g_bExitMapOpen = true;						// State that we now have the exit map open
			g_pCurrentMap = &g_ExitMap;					// Set the current map to our exit map
			g_pCurrentMap->SetCurrentType(EXIT_TYPE);	// Set the current type to the exit type
		}
		else											// If the file is invalid, delete the exits
			g_pCurrentMap->DeleteBlankExits();
	}
	else												// If we already have the exit map open
	{
		// Get the size of the current exit list and get the last exit tile in the list.
		// In other words, get the last tile we just placed on the exit map.
		int size = g_ExitMap.GetCurrentListSize();
		CExit *pExit = (CExit*)g_ExitMap.GetCurrentListTile(size - 1);

		// Set the exits we just placed on the main map with the 
		// new destination info (name, position, etc...)
		g_Map.SetExits(g_ExitMap.GetMapName(), pExit);

		// Restore the current map to our normal map we were working with
		g_pCurrentMap = &g_Map;
		g_bExitMapOpen = false;
	}
}


///////////////////////////////// LOAD OR SAVE A MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the prompt to load or save a map file
/////
///////////////////////////////// LOAD OR SAVE A MAP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadOrSaveAMap(bool bLoad)
{
	string strMapName = "";		// Create a STL string to read in the input from the user
	char szName[255] = {0};		// Create a traditional string of characters for the file name

	// Get an output handle for the console window and set the cursor position for a prompt
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// To clear the prompt, we just print out a bunch of tabs and then reset the cursor position
	cout << "\t\t\t\t\t\t\t\t\t\t";
	SetConsoleCursorPosition(hOutput, g_promptPos);

	// Get an input handle and set the console window to allow input from the user 
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


///////////////////////////////// CHECK KEYBOARD INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the actions when the user types a key from the keyboard
/////
///////////////////////////////// CHECK KEYBOARD INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CheckKeyboardInput(const INPUT_RECORD &InputRecord)
{			
	// If we have an exit map open, don't allow input from the keyboard
	if(g_bExitMapOpen) return;

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

		g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];	// Set the cursor tile to the last tile
		g_pCursorTile->SetChar(image);				// Set the image of our exit tile
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
			// Use the modulus operator to restrict our index to the size of the npc list,
			// then set the cursor tile to the current tile in the npc list.
			g_NpcIndex = (g_NpcIndex + 1) % (int)g_vNpcs.size();
			g_pCursorTile = &g_vNpcs[g_NpcIndex];
		}
	}	
	// Check if we want to load a new map (Pass in true to LoadOrSaveAMap())
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
	
	// If the user presses 'A', we want to allow the them to assign an action key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'A')
	{												
		InputActionKey();
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	

	// If shift is pressed, let's set our flag to true
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT)
		g_bShiftKeyDown = true;
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

	// Don't allow selections from the editor if an exit map is open
	if( (g_cursorPos.Y >= MAP_HEIGHT - EDITOR_HEIGHT) && g_bExitMapOpen) return;

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
				
				// If the user has shift down when choosing a tile from the editor,
				// we want to set that tile as solid, otherwise not solid.
				if(g_bShiftKeyDown)
					g_pCursorTile->SetSolidValue(true);
				else
					g_pCursorTile->SetSolidValue(false);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


			}
		}
		else
		{
			// If we have a tile, let's insert a tile into the map
			if(g_pCursorTile != NULL)
				g_pCurrentMap->InsertTile(g_pCursorTile, g_cursorPos.X, g_cursorPos.Y);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// If the mode is the normal tile mode and the user has shift down
			// when clicking on normal tiles (with no cursor tile) we will set that tile solid.
			else if(g_pCurrentMap->GetCurrentType() == TILE_TYPE)
			{
				if(g_bShiftKeyDown)
				{
					// Set the tile at the cursor's position to solid
					g_Map.SetCollisionTile(true, g_cursorPos.X, g_cursorPos.Y);
				}
			}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	
			// Here we check if our current type is exits
			if(g_pCurrentMap->GetCurrentType() == EXIT_TYPE)
			{
				// If we don't have the shift key down or the exit map is open
				// we want to stop setting exits and handle the exits that were just set.
				if(!g_bShiftKeyDown || g_bExitMapOpen)
					HandleExitTiles();
			}
		}
	}

	// Here we check if the right mouse button was clicked
	if(InputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
	{		
		// Here we either delete a tile on the map, or delete the cursor tile
		if(g_pCursorTile == NULL)				
		{				

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// If mode is the normal tile mode and the user right clicks on a tile,
			// let's copy that tile to the cursor to make our editor a bit more efficient.
			if(g_pCurrentMap->GetCurrentType() == TILE_TYPE)
			{
				// Make sure they right clicked below the editor, not in the editor
				if(g_cursorPos.Y < MAP_HEIGHT - EDITOR_HEIGHT)
				{
					// Here we assign a tile to the cursor, and check if the tile is
					// solid or not.  We then set that value to our cursor tile.  Then
					// finally copy the clicked tile's image to our cursor tile.
					g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];
					bool bIsSolid = g_Map.FindMapTile(g_cursorPos.X, g_cursorPos.Y)->IsSolid();
					g_pCursorTile->SetSolidValue(bIsSolid);
					g_pCursorTile->SetChar(g_Map.GetEditorSelection(g_cursorPos.X, g_cursorPos.Y));	
				}
			}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			
			
			else
			{
				// Delete a tile under our cursor position on the map
				g_pCurrentMap->DeleteTile(g_cursorPos.X, g_cursorPos.Y);
			}
		}
		else									
			g_pCursorTile = NULL;

		// Check if the current tile type is exits
		if(g_pCurrentMap->GetCurrentType() == EXIT_TYPE)
		{
			// Check if our destination map is open and we need to abort to the previous map
			if(g_bExitMapOpen)
			{
				// To abort, we need to set the current map pointer back and reset the flag
				g_pCurrentMap = &g_Map;	
				g_bExitMapOpen = false;
			}

			// Go through and delete all the useless exits and redraw the screen
			g_pCurrentMap->DeleteBlankExits();
			g_pCurrentMap->Draw();
		}
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

	// Check to see if a key was let up on the keyboard
	if(InputRecord.EventType == KEY_EVENT && !bKeyDown)
	{
		// Check if the key that was let up was the shift key, then reset the flag
		if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SHIFT)
			g_bShiftKeyDown = false;
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
// We added a bunch of little scattered checks in and out of our
// input code.  We can now copy normal tiles by right clicking on them.
// We can also add collision to our map tiles, along with action keys.
//
// This concludes our basic Map Editor series.  Now you might be
// thinking, "Over???  What?!?!!?  We barely did anything?  what about
// *** THIS *** functionality or *** THIS *** functionality, or even
// *** THIS *** option???  This map editor sucks man!"  Hehe, well I
// understand what your feeling, but that's all up to YOU!  The rest
// of the options we want you to add yourself :).  Here is your base.
//
// Some extra things that you might want to add to this editor are:
//
// 1) A better visual interface to choose between different tiles
//    and a nice way to show the information about those tiles
//    before and after you chose them.  
// 
// 2) You will also want to allow the user to select a section of
//    map with the cursor and copy and paste that section, just like
//    in a normal paint program.
// 
// 3) You can also add support for different size maps.
// 
// 4) It would be nice to allow maps to attach maps to other maps.
//    This is useful if you create a scrolling map
// 
// 5) You could add support for older maps by placing a version at
//    the front of each .map file.
// 
// 6) Some Pink Floyd music in the background couldn't hurt either :)
//   
// Just remember that this tutorial series is just the bare minimum to
// get a working map editor for a game.  This is in no way finished or
// beautiful.  This is the most effective way to learn in our opinion.
// I know that when I learn something I want the bare bones and not
// look at more complication where you can get potentially confused.
//
// Good luck on your editions.  Let us know what you come up with!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
