//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		Ascii Map Editor (Part 1)						 //
//																		 //
//		$Description:	A simple Ascii text based map editor			 //
//																		 //
//***********************************************************************//

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include "main.h"								// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the first part in our ASCII based map editor series.  In this tutorial we
// go over how to setup our editor and show how to select tile characters and change their colors.
// There is no real functionality in this tutorial because I don't want to overwhelm the newbies
// with tons of code.  Thus, we made it into a tutorial series :).  Make sure you understand
// how to get input and draw to the screen in a console window.  If you don't know how to do
// this, please refer to our tutorials on these topics.  Also, make sure you understand the
// basics of inheritance with classes.
// 
// This tutorial is primarily based off of the 2D Map Editor tutorial series for Win32.
// If you want to do something like this in 2D, check out that tutorial series.
// The code is probably 90% the same, so it will be an easy transition to make a 2D map editor.
//
// * What is an ASCII Map Editor? *
//
// So, what is a ASCII tile based map editor?  I am not sure if you ever played the old ASCII
// adventure games like Moria or Nethack, but these were very popular for their time.  There was
// no graphics using conventional pixels, but they created worlds using just the ASCII characters
// on the keyboard.  We have about 255 characters that we can use in our ASCII set.  ASCII stands
// for American Standard Code for Information Interchange.  Originally, in 1963 when these
// characters were first used, there was just 7 bits, 0 to 127, which were the normal English
// keys, but later it turned into 8 bits, which allowed for international languages like European,
// and Scandinavian languages.  You will notice many of these characters when you run the editor.
// Anyways, using these 255 characters we can actually create beautiful worlds that actually look
// like we are using graphics.  At Digipen, Institute of Technology (A video game programming school),
// one of their first projects is to create a game using no graphics at all, but just using text.
// The thought process is that before one can program cool graphical games, they need to have a
// basic understanding of simple concepts in programming like creating maps, checking input from
// the user, saving, loading, inventory systems, lists and other important things that aren't
// tied so much to graphics.  Sometimes people will see pretty colors and want to jump into making
// the next quake engine, but they have no idea how to even create a character, load a map or use
// any sort of list system.  DON'T become one of those programmers, it's annoying to others :)
// ASCII games are a great start to test your skills before you move into advanced graphics.  If
// you can create a game using ASCII, you can very easily make the transition to graphics and not
// have to worry about the logic programming.  In no time you will be making a ASCII Final Fantasy :)
//
// As you make more games you will want to move onto more complicated and cooler style games 
// which require a more complicated and cooler editor.  This tutorial series is intended to help 
// you get started with a basic editor, then more on from there.  This is not intended to be a
// robust or even pretty editor, but a starting ground for someone who has never made anything 
// like this before.  I encourage you to go VASTLY beyond what is taught in this series.  We will
// provide an example architecture, then you should go on from there to suit it to your game's need
// and make it pretty, like your mom :)
//
// * What does this tutorial do? *
//
// In this tutorial we just display a console window, fill it up with default tiles of grass, 
// then draw a simple editor section.  We then fill that section up with tiles to choose from.  
// You can choose between the different ASCII characters, and if you want to change the foreground
// or background, you can click on the colors at the bottom of the screen.  The first 16 colors are
// for the foreground (the character's color), where the next 16 colors are for the background.
// So far, all you can do in this tutorial is just click on the tiles on the bottom and change their
// colors, but you can't place anything on the map yet.  That will be in the next tutorial.
// Hitting ESCAPE will close the program.
// 
// Enough talking (typing), let's jump into the code!
//
//

// This is our global lists for our tool bar.  These lists hold the tiles for the
// map tiles, the items, the monsters and the NPC's (Non Player Characters).
// These are not the lists that hold information for the current map, but we choose from
// these lists and then place the chosen tile on our map.  The map tile information is
// held in the g_Map variable, which is defined a bit farther down below.
// We will be using STL Vectors for our lists.  If you are not familiar with this
// please look at our C++ Vectors tutorial to understand their usage.  It's basically just 
// an easy way to make a list of something, so you don’t have to always write your own list code.
vector<CTile> g_vTiles;							// Create a list for the map tiles to be loaded
vector<CItem> g_vItems;							// Create a list for the item tiles to be loaded
vector<CMonster> g_vMonsters;					// Create a list for the monster tiles to be loaded
vector<CNpc> g_vNpcs;							// Create a list for the Npc (citizen) tiles to be loaded

// Since we will be loading 2 maps at the same time later for exits we create a pointer
// which stores the current map we are working with.  That way it cuts down on code.
// We currently only load one map in this tutorial, but instead of change it all around
// in the next tutorials, I figured we could start out using this pointer.. "z'ok?"  :)
CMap *g_pCurrentMap = NULL;						

// This is our map class which holds all the information for the current map.  When we load
// a map, all that information is stored here.  This is the most important class in our editor.
CMap g_Map;

// We will be using a bit of complicated code in this series, which allows us to choose different
// tiles that are inherited from the base class, CTile.  That way we don't need to have a
// variable for each type of tile that we are working with.  We can just cast this pointer
// to the appropriate type, like a CItem, CMonster, etc...  This pointer stores the current
// tile that the user has chosen.  We just will insert this tile into the map if the user
// clicks the right mouse button (but not till the next tutorial :)
CTile *g_pCursorTile = NULL;					

// This stores the current cursor position in our console window
COORD g_cursorPos = {0};						


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
	// In this tutorial version we won't be doing much with keyboard
	// input, except when they hit ESC we will quit the program.  Let's
	// just set this up for the next tutorial so you can see how we will
	// handle input later.  It's nice and clean now so it's easier to understand :)

	// If the user hits the escape key
	if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
	{												
		exit(0);					// Quit the program				
	}								// If the user hits the I key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'I')
	{										
		// Select the item tiles
	}								// If the user hits the M key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'M')
	{	
		// Select the monster tiles
	}								// If the user hits the N key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'N')
	{										
		// Select the npc tiles
	}								// If the user hits the E key
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == 'E')
	{	
		// Select exit tiles
	}								// If the user hits the space bar
	else if(InputRecord.Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
	{	
		// Cycle through the items/monsters/npc tiles
	}
}

///////////////////////////////// CHECK MOUSE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the actions when the user uses the mouse
/////
///////////////////////////////// CHECK MOUSE INPUT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CheckMouseInput(const INPUT_RECORD &InputRecord)
{
	// In this function we just do some basic checking of the mouse.  We check
	// if the user uses the mouse to select a tile from the editor.  If they do,
	// we then select the tile and change the color if desired.

	// Get the current position of the mouse in the console window
	g_cursorPos = InputRecord.Event.MouseEvent.dwMousePosition;

	// If the user hits the left mouse button
	if(InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
	{	
		// Here we check if the user chose a tile from our editor.  The map height
		// only goes up to 45, and the editor height is 5, which gives us space to
		// display the tiles to choose from.  If the y value is greater than the map
		// height, we select a tile, otherwise we assume that the user wants to insert
		// a tile on the map (which we do in the next tutorial).
		if(g_cursorPos.Y > MAP_HEIGHT - EDITOR_HEIGHT)
		{		
			// In these 2 lines of code we get the character that the user chose from
			// the editor, and then copy it into another variable for the cursor, since
			// we will maybe be doing some math to extract the foreground or background color.
			// GetEditorSelection just is a data access function into the m_screenBuffer array
			// which stores all of our basic map tile images.  This buffer stores what' drawn
			// to the screen.
			CHAR_INFO chosenTile = g_Map.GetEditorSelection(g_cursorPos.X, g_cursorPos.Y);	
			CHAR_INFO cursorTile = chosenTile;

			// If we already have a tile selected, let's grab it's image instead 
			if(g_pCursorTile != NULL)
				cursorTile = g_pCursorTile->GetChar();
					
			// Here we check to see if where we chose was in the editor section that
			// has the foreground and background colors.  It's at the very bottom of
			// the screen and through the x indices 15 through 45.  The first 16 are
			// for the foreground colors, and the next 16 are for the background colors.
			if(g_cursorPos.X >= 15 && g_cursorPos.Y == (MAP_HEIGHT - 1))
			{
				CHAR_INFO newTile;				// Create a new tile to work with for color

				// What we do below is check if the x index is greater than 30, which means
				// that the user wants to change the BACKGROUND color of the current character.
				// If not, we chose a foreground color tile.  We then do some math to extract
				// the background or foreground color from the chosen tile and apply it to
				// the current cursor tile.  For more info on this math, check out the 
				// functions AddNewBackground() and AddNewForeground().
				if(g_cursorPos.X > 30) 
					newTile = g_Map.AddNewBackground(cursorTile, chosenTile);
				else
					newTile = g_Map.AddNewForeground(cursorTile, chosenTile);
					
				// Apply the new tile with it's new color to our cursor.  Notice that
				// we use the last tile in our list (size() - 1).  We just added an extra
				// tile in CMap::SetDefault() so that we could assign it to the cursor.
				// The rest of the g_vTiles are the editor tiles that we choose from.
				g_vTiles[g_vTiles.size() - 1].SetChar(newTile);
			}
			else
			{
				// Otherwise, if we didn't choose a color, then we must have
				// chosen a new tile character from the editor.  We don't want to
				// change the attributes (colors) of the current cursor tile, so
				// we assign the chosen tile's colors to the cursor tile's colors.
				// That way we only change the color of the tile when the user does
				// it themselves.  It's just a preference thing.  We then set the current
				// cursor tile to the newly chosen tile.  Remember, the cursor tile is
				// a pointer to the last tile in our g_vTiles list, so we change that and
				// it changes the g_pCursorTile.
				chosenTile.Attributes = cursorTile.Attributes;
				g_vTiles[g_vTiles.size() - 1].SetChar(chosenTile);

				// Just to show that the g_pCursorTile points to the last tile
				// in the global tiles list, let's set it again and set the type
				// of tile that we are working with to a normal TILE_TYPE.
				g_pCursorTile = &g_vTiles[g_vTiles.size() - 1];
				g_Map.SetCurrentType(TILE_TYPE);
			}
		}
		else
		{
			// Insert the chosen tile into the map
		}
	}

	// Here we check if the right mouse button was clicked, which would in a later
	// tutorial, delete the cursor tile and a tile on the map.
	if(InputRecord.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
	{											
		// Delete the current cursor tile or delete the tile under the cursor	
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
													
	// Now we check if there was a keyboard or mouse event.  So that CheckInput() didn't
	// get too huge and ugly, I created separate functions for checking the keyboard and
	// mouse input.  This is how you keep you code clean and easily decipherable.
	// We want to return a true if the user did something so we know that we need to draw again.

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
			// We use the current map pointer since later we will be placing
			// exits on different maps, which will mean we have multiple maps
			// open at the same time, and we want to draw the current one we
			// are working with.
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
// This file is pretty simple, we just set up the main structure of
// the program.  We check input, handle a little bit of that input,
// then draw the map to the screen if the user did anything with input.
// We didn't necessarily use all of the data in this tutorial, but we
// have set up the data for the next tutorials.
//
// In the next tutorial we will focus on how to draw the desired tile,
// whether it be a tile, item, monster or npc.  This will be pretty simple.
// Are you excited!?!?!?!?
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
