#ifndef _MAIN_H				// If we haven't included this file							
#define _MAIN_H				// Set a flag saying we included it

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <vector>			// Include the STL vector class information
#include <iostream>			// Include the standard C++ file
#include <string>			// We need this for the string class
#include <windows.h>		// We need to include windows.h for win32 calls
#include "bitmap.h"			// Include the bitmap function prototypes first
#include "resource.h"		// Include our resource file for our menu
using namespace std;		// Include the standard namespace	

#include "tile.h"			// Include our tile information

#define SCREEN_WIDTH 640								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 480								// We want our screen height 600 pixels

#define SCROLL_BAR_OFFSET 16							// The pixel offset for the scroll bar
#define MENU_OFFSET 16									// The pixel offset for the menu bar

extern HWND  g_hWnd;									// This is the handle for the window
extern HWND  g_hWndTool;								// This is the handle for the toolbar window
extern HINSTANCE g_hInstance;							// This holds our window hInstance

// Create a double buffer class variable for both our windows
extern CBuffer g_Buffer;
extern CBuffer g_ToolBuffer;

// Create map class variables for our main map's information 
extern CMap g_Map;
extern CMap *g_pCurrentMap;

// These are our lists for our global tile data
extern vector<CTile> g_vTiles;
extern vector<CItem> g_vItems;
extern vector<CMonster> g_vMonsters;
extern vector<CNpc> g_vNpcs;


// The function prototype for our WndProc (The function that handles all the windows messages)																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		

// This function is our call back window procedure for our tool bar window
LRESULT CALLBACK WndProcPopup(HWND, UINT, WPARAM, LPARAM);

// This initializes our main application specific data
void Init(HWND hWnd);

// This controls our main program loop
WPARAM MainLoop();

// This frees memory and quits the program
void DeInit();

#endif


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// Nothing changed in this file since the last tutorial.
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
