//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		MapEditor1										 //
//																		 //
//		$Description:	A simple tile based map editor					 //
//																		 //
//***********************************************************************//


#include "main.h"								// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the first part in our tile based map editor series.  In this tutorial we
// go over how to setup our windows and switch between different tile types to choose from.
// There is no functionality in this tutorial because I don't want to overwhelm the newbies
// with tons of code.  Thus, we made it into a tutorial series :).  You can download the final
// version of this tutorial series and see what functionality will be done when this series is
// over.  You can then open Default.map to test it out.
// 
// So, what is a tile based map editor?  Think back to many of the older RPG/Adventure games
// that you have played.  You will probably notice that a lot of those games are created by
// small tiles (i.e. 32 by 32 pixel bitmaps) which make up the maps or levels.  By making many
// tiles of the same scenery you save on memory and such.  This way you also don't need to draw
// a huge bitmap on the screen.  With computers now days we don't really need to worry much about
// those sorts of things, but tile based editors can always make map creation easier, if you aren't
// an artist.  Even the most unartistic programmer can make a map out of tiles :)  
//
// As you make more games you will want to move onto more complicated and cooler style games 
// which require a more complicated and cooler editor.  This tutorial series is intended to help 
// you get started with a basic editor, then more on from there.  This is not intended to be a
// robust or even pretty editor, but a starting ground for someone who has never made anything 
// like this before.  I encourage you to go VASTLY beyond what is taught in this series.  We will
// provide an example architecture, then you should go on from there to suit it to your game's need
// and make it pretty, like your mom :)
//
// In this tutorial we just display a window, fill it up with default tiles of grass, then draw a
// simple tool window.  We then fill that tool window up with tiles to choose from.  There are 
// folders that we will load these tiles from (i.e. Tiles\, Items\, Monsters\, Npcs\).  The default
// is the map tiles, but if you look at the menu you will find that there are items, monsters, 
// citizens (NPC's) and exits to choose from.  Exits don't have tiles, but instead just draw a
// rectangle.  In this tutorial you won't be able to choose the tiles.  In the next tutorial we
// will go over the code to implement this.  You will notice also that other than Quit, none of
// the menu options work.  This will also be implemented in the following tutorials.
// Note that this tutorial comes partly from the Tile Maze tutorial in the Win32 section.
// 
// Enough talking (typing), let's jump into the code!
//
//

HWND  g_hWnd;									// This is a global handle for the window
HWND  g_hWndTool;								// This is a global handle for the toolbar window
HINSTANCE g_hInstance;							// This holds the global hInstance for UnregisterClass() in DeInit()

// Here we create a class of CBuffer which is in charge of our double buffering
// and handling our bitmap functions.  We create a buffer for each window (main and tool window).
CBuffer g_Buffer;								// Create a buffer for the main window
CBuffer g_ToolBuffer;							// Create a buffer for the tool bar window

// This is our global lists for our tool bar.  These lists hold the tiles for the
// map tiles, the items, the monsters, the NPC's (Non Player Characters) and the exits.
// These are not the lists that hold information for the current map, but we choose from
// these lists and then place the chosen tile on our map.  The map tile information is
// held in the g_Map variable, which is defined a bit farther down below.
// When we load in the types of tiles (from Tiles\, Items\, Monsters\ etc...)
// We will be using STL Vectors for our lists.  If you are not familiar with this
// please look at our C++ Vectors tutorial to understand their usage.  It's basically just 
// an easy way to make a list of something, so you don’t have to always write your own list code.
vector<CTile> g_vTiles;							// Create a list for the map tiles to be loaded
vector<CItem> g_vItems;							// Create a list for the item tiles to be loaded
vector<CMonster> g_vMonsters;					// Create a list for the monster tiles to be loaded
vector<CNpc> g_vNpcs;							// Create a list for the Npc (citizen) tiles to be loaded

// This stores our scroll bar information.  For more info on scroll bars 
// check out our scroll bar tutorial.
SCROLLINFO g_ScrollInfo = {0};
int g_scrollBarPosY = 0;						// Set the current scroll bar position to 0

// Since we will be loading 2 maps at the same time later for exits we create a pointer
// which stores the current map we are working with.  That way it cuts down on code.
// We currently only load one map in this tutorial, but instead of change it all around
// in the next tutorials, I figured we could start out using this pointer.. "z'ok?"  :)
CMap *g_pCurrentMap = NULL;						

// This is our map class which holds all the information for the current map.  When we load
// a map, all that information is stored here.  This is the most important class in our editor.
CMap g_Map;


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws all of the tiles to the screen
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene()
{
	// In this function we draw the tiles on the map and in the tool bar, along with
	// the bitmap attached to the cursor when choosing a tile to place on the map.
	// We also swap the back buffers to the front so we don't get flickering on the screen.

	// Make sure we have a current map pointer before we go shootin' blanks
	if(!g_pCurrentMap) return;

	// Here we set the portion of our bitmaps that we want to draw.
	// Since there is no animation in our tile editor we need only display 1 tile.
	RECT rPortion = {0, 0, TILE_WIDTH, TILE_HEIGHT};

	// Next we draw the current map (Default is just all green grass tiles)
	g_pCurrentMap->Draw();

	// Grab the size of the current tool bar type list (Not our map's placed tile lists)
	int size = g_pCurrentMap->GetCurrentTypeSize();

	// Go through our tiles in the tool bar and draw them, but subtract the scroll bar position
	// from the size.  This makes it so we only draw the tiles that can be seen in the tool bar.
	for(int i = 0; i < size - g_scrollBarPosY; i++)
	{
		// Depending on "i" and the scroll bar position, get the current tile's bitmap to draw
		HBITMAP bitmap = g_pCurrentMap->GetCurrentTypeTile(i + g_scrollBarPosY)->GetBitmap();

		// If the tool bar is showing map tiles then draw the tiles with no transparency
		if(g_pCurrentMap->GetCurrentType() == TILE_TYPE)
			g_ToolBuffer.DisplayBitmap(bitmap, 0, TILE_WIDTH * i, rPortion);  
		else
			// Draw the current tiles with a transparency (white) (i.e. items, monsters, etc...)
			g_ToolBuffer.DisplayTransparentBitmap(bitmap, 0, TILE_WIDTH * i, rPortion);  
	}

	// Now swap the backbuffer to display it to the front and clear the screen only on the 
	// tool bar window, since the main window always redraws itself with tiles beneath.
	// The tool bar window has transparent tiles so it would look weird if we didn't clear it.
	g_Buffer.SwapBackBuffer(FALSE);
	g_ToolBuffer.SwapBackBuffer(TRUE);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////



///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop and calls our drawing routine
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// This is where we load our accelerators.  That means that we want to be able to hit
	// Control+(something) and have it choose from the menu depending on what we want.
	// For instance, CTRL+Q will quit, CTRL+O will open a file, etc...  We defined our accelerators
	// in MapEditor.rc.  I used Microsoft Visual Studio's resource editor, but you can open up
	// MapEditor.rc with notepad and add them yourself.
	HACCEL hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
			
			// Check if there was keyboard command - if not, process messages like normal
			if(!TranslateAccelerator(g_hWnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);					// Find out what the message does
				DispatchMessage(&msg);					// Execute the message
			}

			RenderScene();								// Since no animation, only render when the user does something
        } 
	}
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}
		

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// CHECK TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function puts a check mark in the menu on the selected tile type
/////
///////////////////////////////// CHECK TILE TYPE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CheckTileType(int menuID)
{
	// Grab a handle to our menu
	HMENU hMenu = GetMenu(g_hWnd);

	// Uncheck all of the menu items for the Tiles menu
	CheckMenuItem(hMenu, ID_TILES_TILES, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_TILES_ITEMS, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_TILES_MONSTERS, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_TILES_CITIZENS, MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_TILES_EXITS, MF_UNCHECKED);

	// Put a check by the desired menu item
	CheckMenuItem(hMenu, menuID, MF_CHECKED);
}


///////////////////////////////// HANDLE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the actions when a menu item is chosen
/////
///////////////////////////////// HANDLE MENU \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void HandleMenu(int menuID)
{		
	if(menuID == ID_FILE_QUIT)					// If the user chose File->Quit
	{
		PostQuitMessage(0);						// Quit the program
	}
	else if(menuID == ID_FILE_OPENUP)			// If the user chose File->Open
	{	
		// Will fill in at a later tutorial
	}
	else if(menuID == ID_FILE_SAVEIT)			// If the user chose File->Save
	{
		// Will fill in at a later tutorial
	}
	else if(menuID == ID_FILE_SAVEAS)			// If the user chose File->SaveAs
	{
		// Will fill in at a later tutorial
	}
	else if(menuID == ID_FILE_RESET)			// If the user chose File->Reset
		g_Map.SetDefault();						// Destroy all the tiles on the map and start over
	else if(menuID == ID_TILES_TILES)			// If the user chose Tiles->Tiles
		g_Map.SetCurrentType(TILE_TYPE);		// Set the tool bar tile type to tiles
	else if(menuID == ID_TILES_ITEMS)			// If the user chose Tiles->Items
		g_Map.SetCurrentType(ITEM_TYPE);		// Set the tool bar tile type to items
	else if(menuID == ID_TILES_MONSTERS)		// If the user chose Tiles->Monsters
		g_Map.SetCurrentType(MONSTER_TYPE);		// Set the tool bar tile type to monsters
	else if(menuID == ID_TILES_CITIZENS)		// If the user chose Tiles->Citizens
		g_Map.SetCurrentType(NPC_TYPE);			// Set the tool bar tile type to citizens
	else if(menuID == ID_TILES_EXITS)			// If the user chose Tiles->Exits
		g_Map.SetCurrentType(EXIT_TYPE);		// Set the tool bar tile type to exits (no tiles of course)
	else if(menuID == ID_HELP_ABOUT)			// If the user chose Help->About
		// Instead of creating a help dialog box, we can just use a simple system MessageBox() call
		MessageBox(g_hWnd, "www.GameTutorials.com Map Editor\n\n"
						   "\t- Choose the desired tile type from the Tiles menu,\n"
						   "\t  then click on a tile in the tile window. You can\n"
						   "\t  then draw the tiles on the map with the left mouse button.\n\n"
						   "\t- Right click to remove the current cursor tile.\n\n"
						   "\t- To delete a item:monster:citizen:exit, make sure you choose\n"
						   "\t  that type from the menu, then right click on the tile.\n\n"
						   "\t- When setting an exit you can hold down shift to place more exits.\n"
						   "\t  Once you place an exit without holding the shift key you will be\n"
						   "\t  prompted to choose the destination map.  Then place one tile for\n"
						   "\t  the position where the character will end up.\n\n"
						   "\t- Hit Esc to quit the program.", "About", MB_OK | MB_ICONINFORMATION);

	// Over assuming that a tile type was changed, reset the scroll bar size.
	// The -1 is because the tool bar window's title size offsets it a bit (being smaller).
	g_ScrollInfo.nMax = g_Map.GetCurrentTypeSize() - MAP_HEIGHT - 1;
	
	// If there isn't enough (or any) tiles to fill up the tool bar window, disable the scroll bar
	if(g_ScrollInfo.nMax < 0)
	{
		g_ScrollInfo.nMax = 0;
		EnableScrollBar(g_hWndTool, SB_VERT, ESB_DISABLE_BOTH);
	}
	else
		EnableScrollBar(g_hWndTool, SB_VERT, ESB_ENABLE_BOTH);	// Enable the scroll bar if it's needed

	// Reset the scroll bar info
	g_scrollBarPosY = 0;
	g_ScrollInfo.nPage = 0;

	// Update the current scroll bar information with no need to redraw (FALSE)
	SetScrollInfo(g_hWndTool, SB_VERT, &g_ScrollInfo, FALSE);
	ShowScrollBar(g_hWndTool, SB_VERT, TRUE);

	// If we chose a tile type, put a check box next to it
	switch(menuID)
	{
		case ID_TILES_TILES: 
		case ID_TILES_ITEMS:
		case ID_TILES_MONSTERS:
		case ID_TILES_CITIZENS:
		case ID_TILES_EXITS:
			CheckTileType(menuID);
			break;
	}
}


///////////////////////////////// HANDLE SCROLL BAR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the actions when the scroll bar is used
/////
///////////////////////////////// HANDLE SCROLL BAR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void HandleScrollBar(WPARAM wParam)
{
	switch(LOWORD(wParam)) 
	{
		case SB_LINEUP: case SB_PAGEUP:					// If we choose the up button or click above the middle button
			g_scrollBarPosY = g_scrollBarPosY - 1;		// Decrease the scroll bar position
				break;

		case SB_LINEDOWN: case SB_PAGEDOWN:				// If we choose the down button or click below the middle button
			g_scrollBarPosY = g_scrollBarPosY + 1;		// Increase the scroll bar position
				break;

		case SB_THUMBPOSITION: case SB_THUMBTRACK:		// If we dragged the middle button (thumb)
			g_scrollBarPosY = HIWORD(wParam);			// Update it's new position
				break;

		default:
			break;

	}
	
	SCROLLINFO scrollInfo;								// Create a new temporary scroll bar info variable

	scrollInfo.cbSize = sizeof(SCROLLINFO);				// Initialize it's size
	scrollInfo.fMask = SIF_POS | SIF_RANGE;				// Set the flags so that we get the position and range updates
	scrollInfo.nPage = 1;								// Set the page size to 1 (a page increases by 1 tile)
			
	GetScrollInfo(g_hWndTool, SB_VERT, &scrollInfo);	// Retrieve the current scroll info

	if(g_scrollBarPosY != scrollInfo.nPos)				// If the scroll bar position has changed
	{
		if(g_scrollBarPosY > scrollInfo.nMax)			// If we have gone past the first page of the scroll bar
			g_scrollBarPosY = scrollInfo.nMax;			// Set the current position
		else if(g_scrollBarPosY < scrollInfo.nMin)		// If go below the minimum value of the scroll bar set it back
			g_scrollBarPosY = scrollInfo.nMin;

		scrollInfo.nPos = g_scrollBarPosY;				// Set the current scroll position and then update the scroll bar
														
		SetScrollInfo(g_hWndTool, SB_VERT, &scrollInfo, TRUE);

		InvalidateRect(g_hWndTool, NULL, FALSE);		// Update the tool bar as we scroll with the thumb button
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


/////////////////////////////////// WND PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the messages that come to and from our window
/////
/////////////////////////////////// WND PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Check which message was passed in
    switch (iMsg)											
    {

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
		case WM_COMMAND:							// Check if we have a command from the user						
			HandleMenu(LOWORD(wParam));				// Pass in the command ID to our menu function
			break;
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		case WM_KEYDOWN:
			switch (wParam) 
			{
				case VK_ESCAPE:
					
					// Tell the window we want to QUIT
					PostQuitMessage(0);
					break;
			}
			break;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
		// Since we created a child window for our tool bar, we want both windows
		// to be constantly active.  By return TRUE in DefWindowProc() we can have
		// the window not go inactive when we click on another window.
		case WM_NCACTIVATE:
			return DefWindowProc(hWnd, iMsg, TRUE, lParam);

		// By return 1 when this message is called we eliminate have our window background
		// redrawn.  This makes it so we don't see flicker when our tool bar window is
		// dragged over our main window.  Cool little trip huh :)
		case WM_ERASEBKGND:
			return 1;
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// This message is sent when the window is destroyed
		case WM_DESTROY:									
			
			// Tell the window we need to close
			PostQuitMessage(0);							
			break;
		
		// Process/handle the default messages	
		default:
			return DefWindowProc (hWnd, iMsg, wParam, lParam);			
	}											
	
	// Return zero when we handle a message
	return 0;													
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// WND PROC POPUP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the messages of our popup tool bar window
/////
///////////////////////////////// WND PROC POPUP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WndProcPopup(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Check which message was passed in
    switch (iMsg)											
    {
		// Check if we need to handle a scroll bar message
		case WM_VSCROLL:
			HandleScrollBar(wParam);
			break;

		// By sending TRUE in DefWindowProc in this message we always maintain an active window
		case WM_NCACTIVATE:
			return DefWindowProc(hWnd, iMsg, TRUE, lParam); 

		// This message is sent when the window is destroyed
		case WM_DESTROY:									
			
			// Tell the window we need to close
			PostQuitMessage(0);							
			break;
			
		// Process/handle the default messages	
		default:
			return DefWindowProc (hWnd, iMsg, wParam, lParam);			
	}													

	// Return zero when we handle a message
	return 0;															
}

///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes our application-specific data
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	// Set our global window handle to our main window
	g_hWnd = hWnd;

	// Create our double buffering for each window
	g_Buffer.CreateDoubleBuffering(hWnd);
	g_ToolBuffer.CreateDoubleBuffering(g_hWndTool);

	// Set the map to default
    g_Map.SetDefault();

	// Load the tiles into the global lists and then get the scroll bar max scroll position
	g_ScrollInfo.nMax = LoadTiles();

	// By default set the current tile type to map tiles
	g_Map.SetCurrentType(TILE_TYPE);

	// Set our current map to ....  our current map :)
	g_pCurrentMap = &g_Map;

	// Initialize the scroll bar information
	g_ScrollInfo.cbSize = sizeof(SCROLLINFO);
	g_ScrollInfo.nMin = 0; 
	g_ScrollInfo.nPage = 1; 
	g_ScrollInfo.fMask = SIF_PAGE | SIF_RANGE;
	SetScrollInfo(g_hWndTool, SB_VERT, &g_ScrollInfo, FALSE);
				
	// Set the backbuffer to black first (This clears the backbuffer)
	g_Buffer.ClearScreen(BLACK_BRUSH);			
	g_ToolBuffer.ClearScreen(BLACK_BRUSH);			
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// This file is pretty simple, just mixing a menu bar, scroll bar
// and create window tutorial together.  Nothing too difficult.  However,
// we do implement accelerators for our menus.
//
// In the next tutorial we will focus on how to choose the desired tile
// and how to draw them on the map.  This will be pretty simple to do.
//
// You will notice that there is unique naming conventions for the tiles
// stored in Tiles\ directory.  This way we can tell which tile is a floor
// tile that can be walked on, or whether it is a wall tile that we can't
// walk on.  It makes it easy and modular for checking collision detection.
//
// There is also info files in the Items\ and Monsters\ and Npcs\ folders.
// These info files are used to store the objects information.  We will
// load these in at a later tutorial.  Can't you wait!?!?!?!? :)
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
 
