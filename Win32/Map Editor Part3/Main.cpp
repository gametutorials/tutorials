//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		MapEditor3										 //
//																		 //
//		$Description:	A simple tile based map editor					 //
//																		 //
//***********************************************************************//


#include "main.h"								// Include our main header file

HWND  g_hWnd;									// This is a global handle for the window
HWND  g_hWndTool;								// This is a global handle for the toolbar window
HINSTANCE g_hInstance;							// This holds the global hInstance for UnregisterClass() in DeInit()

CBuffer g_Buffer;								// Create a buffer for the main window
CBuffer g_ToolBuffer;							// Create a buffer for the tool bar window

vector<CTile> g_vTiles;							// Create a list for the map tiles to be loaded
vector<CItem> g_vItems;							// Create a list for the item tiles to be loaded
vector<CMonster> g_vMonsters;					// Create a list for the monster tiles to be loaded
vector<CNpc> g_vNpcs;							// Create a list for the Npc (citizen) tiles to be loaded

SCROLLINFO g_ScrollInfo = {0};
int g_scrollBarPosY = 0;						// Set the current scroll bar position to 0

CMap *g_pCurrentMap = NULL;						// Create a pointer to store the current map

CMap g_Map;										// This stores our map information and functions

CTile *g_pCursorTile = NULL;					// Our polymorphic cursor tile pointer

POINT g_cursorPos = {0};						// This stores our current cursor position

bool g_bLButtonDown = false;					// Whether or not the left mouse button is down


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// In this version of our tutorial series we focus on Loading and Saving map files.  We use
// the .map extension.  The loading and saving algorithm is extremely simple, but you can
// encode it in a more compressed and encrypted format if you would like.  Since we have a 
// list of tile types associated in the map, we just save those list, then when we read
// them in, instead of loading their bitmaps associated with them, those bitmaps are already
// load from the global tile lists so we can just grab that already loaded HBITMAP and store
// it in the loaded tile.  That way we only load the bitmap once, and then share it's
// handle between the global lists and the private members of the map class.  That's why we
// add a function called FindHBitmap().  
// 
// You will notice that in the Items\, Monsters\ and Npcs\ folder there is a text file
// that stores information about the tiles.  We use the function SetTileInfo() to load in
// that information, then store that info with the associated tile.
// 
// If you are unfamiliar with the OPENFILENAME structure in regards to the opening
// and saving dialog box you can check out our Opening Files tutorial in the Win32 section.
// 
// 
//

// Create our global variables for our open file dialog box
OPENFILENAME g_OpenInfo = {0};
char g_szFileName[MAX_PATH] = {0};

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// DRAW CURSOR TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws the tile attached to the cursor
/////
///////////////////////////////// DRAW CURSOR TILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawCursorTile()
{
	// Create a rect to specify what part of our bitmaps we want to draw
	RECT rPortion = {0, 0, TILE_WIDTH, TILE_HEIGHT};

	// Here we update our cursor position and convert it to client 
	// coordinates (our window coordinates), not the screen coordinates.
	GetCursorPos(&g_cursorPos);
	ScreenToClient(g_hWnd, &g_cursorPos);

	// Return from this function if no cursor tile only if not in exit mode
	if(g_pCurrentMap->GetCurrentType() != EXIT_TYPE)
		if(!g_pCursorTile) return;
	
	// If the current type is a normal map tile, draw it with no transparency.
	// We line up the cursor bitmap so that it's dead center on the cursor.
	if(g_pCurrentMap->GetCurrentType() == TILE_TYPE)
		g_Buffer.DisplayBitmap(g_pCursorTile->GetBitmap(), 
							   g_cursorPos.x - TILE_WIDTH/2, g_cursorPos.y - TILE_HEIGHT/2, rPortion); 
	// If we are wanting to insert an exit, just draw a red rectangle
	else if(g_pCurrentMap->GetCurrentType() == EXIT_TYPE)
		Rectangle(g_Buffer.GetHDC(), g_cursorPos.x - TILE_WIDTH/2, g_cursorPos.y - TILE_HEIGHT/2, 
									 g_cursorPos.x + TILE_WIDTH/2, g_cursorPos.y + TILE_HEIGHT/2);
	// Otherwise draw a transparent bitmap for the rest of the tile tiles (items, monsters, etc...)
	else
		g_Buffer.DisplayTransparentBitmap(g_pCursorTile->GetBitmap(), 
										  g_cursorPos.x - TILE_WIDTH/2, g_cursorPos.y - TILE_HEIGHT/2, rPortion); 

	// If the left button is still help down
	if(g_bLButtonDown)
	{
		// Grab the index on our 2D map depending on our cursor position
		int mapIndexX = g_cursorPos.x / TILE_WIDTH;
		int mapIndexY = g_cursorPos.y / TILE_HEIGHT;

		// Insert the tile type into our map according to it's index
		g_pCurrentMap->InsertTile(g_pCursorTile, mapIndexX, mapIndexY); 

		// If it's not a normal tile, don't allow multiple draws per click.
		// If you want to allow rapid drawing of other objects, delete this.
		if(g_pCurrentMap->GetCurrentType() != TILE_TYPE)
			g_bLButtonDown = 0;
	}
}


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws all of the tiles to the screen
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene()
{
	// Make sure we have a current map pointer before we go shootin' blanks
	if(!g_pCurrentMap) return;

	// Here we set the portion of our bitmaps that we want to draw.
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

	// Draw the current cursor tile if chosen
	DrawCursorTile();					

	// Swap the backbuffers to display the bitmaps to the screen
	g_Buffer.SwapBackBuffer(FALSE);
	g_ToolBuffer.SwapBackBuffer(TRUE);
}


///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop and calls our drawing routine
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	// This is where we load our accelerators for keyboard shortcuts
	HACCEL hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;

			// Check if there was keyboard command - if not, process messages like normal
			if(!TranslateAccelerator(g_hWnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);					// Find out what the message does
				DispatchMessage(&msg);					// Execute the message
			}

			RenderScene();								// Since no animation, only render when the user does something
        } 
	}
	
	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}
		

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	else if(menuID == ID_FILE_OPENUP)			// If the user chose File->Open
	{	
		// Have windows bring up the Open File dialog box and Load the file chosen
		if(GetOpenFileName(&g_OpenInfo))
			g_Map.Load(g_OpenInfo.lpstrFile);	
	}
	else if(menuID == ID_FILE_SAVEIT)			// If the user chose File->Save
	{
		// If we have a n valid name for our map already, do a normal save
		if(strlen(g_Map.GetMapName()) > 3)
			g_Map.Save(g_Map.GetMapName());
		else
		{
			// If haven't given our map a name yet, do a "SaveAs" and bring up the save dlg box
			if(GetSaveFileName(&g_OpenInfo))
				g_Map.Save(g_OpenInfo.lpstrFile);
		}
	}
	else if(menuID == ID_FILE_SAVEAS)			// If the user chose File->SaveAs
	{
		// Bring up the save dlg box and allow the user to type in a new map name and save it
		if(GetSaveFileName(&g_OpenInfo))
			g_Map.Save(g_OpenInfo.lpstrFile);
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
			g_pCursorTile = NULL;
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
		case WM_COMMAND:							// Check if we have a command from the user						
			HandleMenu(LOWORD(wParam));				// Pass in the command ID to our menu function
			break;

		case WM_KEYDOWN:
			switch (wParam) 
			{
				case VK_ESCAPE:
					
					// Tell the window we want to QUIT
					PostQuitMessage(0);
					break;
			}
			break;

		case WM_LBUTTONDOWN:						// If we hit the left mouse button
			g_bLButtonDown = true;					// Set our global boolean to true
			break;

		case WM_LBUTTONUP:							// If we let off of the left button
			g_bLButtonDown = false;					// Set our global boolean value to false
			break;

		case WM_RBUTTONDOWN:						// If we click the right button
			if(g_pCursorTile == NULL)				// If we don't have a current cursor tile
			{										
				// Here we delete the tile at our current mouse position from the map
				g_pCurrentMap->DeleteTile(g_cursorPos.x/TILE_WIDTH, g_cursorPos.y/TILE_HEIGHT);
			}
			else									
				g_pCursorTile = NULL;				// Otherwise remove the cursor tile
			break;

		// Don't let the window go inactive
		case WM_NCACTIVATE:
			return DefWindowProc(hWnd, iMsg, TRUE, lParam);

		// This gets rid of unecessary erasing of the window (eliminates flickering)
		case WM_ERASEBKGND:
			return 1;

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
		case WM_LBUTTONDOWN:								// If we click the left button in the tool bar
			g_cursorPos.x = LOWORD(lParam);					// Grab the x and y position of the cursor
			g_cursorPos.y = HIWORD(lParam);
			
			// Here we grab a pointer to the tile type that we clicked on (used for drawing on the cursor)
			g_pCursorTile = g_pCurrentMap->GetCurrentTypeTile(g_cursorPos.y/TILE_HEIGHT + g_scrollBarPosY);
			break;

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
				

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Here we initialize our open file information
	g_OpenInfo.lStructSize = sizeof(OPENFILENAME);				// Set the size of the structure
	g_OpenInfo.nMaxFile = MAX_PATH;								// Set the max characters for a file name
	g_OpenInfo.lpstrFile = g_szFileName;						// Give a string to store the file name
	g_OpenInfo.lpstrFilter = "Map Files (*.map)\0*.map";		// Only accept .map files to load
	g_OpenInfo.hwndOwner = g_hWndTool;							// Assign the window owner and give it desired flags
	g_OpenInfo.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	// Here we set the current pen to red for drawing our exit rectangles
	g_Buffer.SetPen(PS_SOLID,2,RGB(255,0,0));

	// Set the backbuffer to black first (This clears the backbuffer)
	g_Buffer.ClearScreen(BLACK_BRUSH);			
	g_ToolBuffer.ClearScreen(BLACK_BRUSH);			
}


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we added support for the Open, Save and SaveAs menu items.
// If you aren't familiar with the open file dlg box, please see our
// tutorial on this subject in the Win32 section for Opening Files.
// We use the same data structure OPENFILENAME for opening and saving
// our files.
//
// The next tutorial in this series will focus on handling our exit code.
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      
 

