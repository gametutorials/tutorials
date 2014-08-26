#ifndef _MAIN_H
#define _MAIN_H

#include <vector>							// This is needed for the STL vector class
#include <iostream>							// Include our basic C++ header file
#include <string>							// This is included to use the STL string class
#include <windows.h>						// This is for the windows console functions
using namespace std;						// We are using the standard namespace for C++

#include "Tile.h"							// Include our tile information for our variables

// Create externed map class variables for our main map's information
extern CMap g_Map;

// Create a extern to our current map pointer and the cursor tile pointer
extern CMap *g_pCurrentMap;
extern CTile *g_pCursorTile;

// Create a extern to our global variable that stores our current cursor position
extern COORD g_cursorPos;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
extern COORD g_promptPos;			// This stores the default position for our prompt
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


// These are externs to our lists for our global tile data
extern vector<CTile> g_vTiles;
extern vector<CItem> g_vItems;
extern vector<CMonster> g_vMonsters;
extern vector<CNpc> g_vNpcs;


#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we added a variable to store the prompt position,
// which displays message about the tiles and will later allow us
// to type in names of maps to load and save.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
