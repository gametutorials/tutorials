//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		MapEditor5										 //
//																		 //
//		$Description:	A simple tile based map editor					 //
//																		 //
//***********************************************************************//

#include "main.h"								// Include our main header file

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is the final tutorial in our Map Editor series.  In this version we added
// the ability to add action keys, added collision detection and a way to copy a normal
// tile from the map and stick it to your cursor.  To add collision detection and action
// keys, we just added 2 variables to our CTile class so every class that inherits from
// that class will also have collision and action key data assigned to them.
// You will notice that the art and text files changed to reflect our RPG tutorial series game.
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
// happened or not.  If you check out our RPG "GameTutorial" tutorial series, you can
// see the implementation of this.  To add an action key to a tile, you just press the space
// bar when you have a tile attached to your cursor.  You must set the action key before 
// placing a tile.  A message box will pop up and you just type in the action key click OK.
//
// * How does collision work in our editor? *
//
// This is very simple, since we added a m_bSolid boolean variable to our CTile, we just
// need to set that to true or false.  In our game we check to see if a tile is solid
// before allowing a player to walk over it.  It's that simple.  In order to tell which
// tiles are solid and which ones aren't, we already added that to our tiles by the name
// of the bitmap.  You will notice that there is a _w or an _f attached to the name of
// the tile bitmaps.  The _w means that it's a wall and is solid, where the _f means that
// it's a floor tile and you can walk over it.  We also have the option for _d for doors.
// You can add your own types that you would be using in your project or game.  If you
// don't like the idea of having to specify the solid value in the art file, you can just
// add the functionality to maybe hold down shift, or some key, then click on the tile you
// want to be solid.  This was done in the ASCII Map Editor.  Either way is fine and is
// a preference for the developers of the maps.  Being specified in art makes it so you
// don't have to always make everything solid that needs to be solid manually.  This can
// save time and mistakes.  If you want to change the solid value of a tile, that option
// can be easily added in by doing the opposite thing, you click to switch the tile's
// solid value.  Whatever way you want will work just fine.  
//
// If you are in the normal kTileType mode and don't have an attached cursor tile, you can
// right click on a tile placed on the map and it will copy that tile to your cursor.
//
// * A new message box? *
// 
// When you have a tile attached to your cursor you can now press the space bar to set
// the action key for that tile.  A message box will appear and you can enter in a number
// that will be associated with that tile.  You must set the action key of a tile before
// you place it on the map, while it's still attached to your cursor.
//
// Check the help menu for more details on controls.
//
// * IMPORTANT * It is important to realize that you can't load a previously created map
// in a previous tutorial because we added new variables to our CTile class.  The data is
// not setup exactly the same so the map will not load correctly.  It's important to always
// have your classes and structures the same in your map editor and your game.  If you just
// HAVE to have an old map your create, just create 2 sets of the classes and load in using
// the old classes, then save then copy the data to the new updated classes and save it.
// It's a dirty fix for saving old data :)  It comes in handy when you realize you need to
// add new data to your game/editor, but have already created the levels/maps.
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
CMap g_Map, g_ExitMap;							// Create a map class for our main map and exit map

CTile *g_pCursorTile = NULL;					// Our polymorphic cursor tile pointer
POINT g_cursorPos = {0};						// This stores our current cursor position

bool g_bLButtonDown = false;					// Whether or not the left mouse button is down
bool g_bExitMapOpen = false;					// This tells us if the exit map is being drawn
bool g_bShiftKeyDown = false;					// This tells us if the user is holding down the shift key

// Create our global variables for our open file dialog box
OPENFILENAME g_OpenInfo = {0};
char g_szFileName[MAX_PATH] = {0};

HWND g_hDlgWnd = NULL;


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
		// Bring up the open dialog box and get a map to exit to
		if(GetOpenFileName(&g_OpenInfo))
		{
			g_ExitMap.Load(g_OpenInfo.lpstrFile);			// Open the map to exit to
			g_bExitMapOpen = true;							// State that we now have the exit map open
			g_pCurrentMap = &g_ExitMap;						// Set the current map to our exit map
			g_pCurrentMap->SetCurrentType(EXIT_TYPE);		// set the current type of this map to exit tiles
		}
		else												// If we don't open a file delete the created exits
			g_pCurrentMap->DeleteBlankExits();
	}
	else													// If we already have the exit map open
	{
		// Get the size of the current exit list and get the last exit tile in the list.
		// In other words, get the last tile we just placed on the exit map.
		int size = g_ExitMap.GetCurrentListSize();
		CExit *pExit = (CExit*)g_ExitMap.GetCurrentListTile(size - 1);

		// Set the exits we just placed on the main map with the new destination info (name, position)
		g_Map.SetExits(g_ExitMap.GetMapName(), pExit);

		// Restore the current map to our normal map we were working with
		g_pCurrentMap = &g_Map;
		g_bExitMapOpen = false;
	}
}


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
		if(g_pCurrentMap->GetCurrentType() != TILE_TYPE)
			g_bLButtonDown = 0;

		// Here we check if our current type is exits
		if(g_pCurrentMap->GetCurrentType() == EXIT_TYPE)
		{
			// If we don't have the shift key down or the exit map is open
			// we want to stop setting exits and handle the exits just set.
			if(!g_bShiftKeyDown || g_bExitMapOpen)
				HandleExitTiles();
		}
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// If the dialog box is open, then we want to make it the center of focus
			if(!g_hDlgWnd)
				SetFocus(g_hWnd);
        } 
	}
	
	// Let's destory the dialog box window if it has a valid handle
	if(g_hDlgWnd)
	{
		DestroyWindow(g_hDlgWnd);
		g_hDlgWnd = NULL;
	}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
	if(g_bExitMapOpen) return;					// Don't let the user access the menu if the exit map open

	if(menuID == ID_FILE_QUIT)					// If the user chose File->Quit
	{
		PostQuitMessage(0);						// Quit the program
	}
	else if(menuID == ID_FILE_OPENUP)			// If the user chose File->Open
	{	
		// Have windows bring up the Open File dialog box and Load the file chosen
		if(GetOpenFileName(&g_OpenInfo))
			g_Map.Load(g_OpenInfo.lpstrFile);	
	}
	else if(menuID == ID_FILE_SAVEIT)			// If the user chose File->Save
	{
		// If we have a valid name for our map already, do a normal save
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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
						  
							// Here we add new directions for our help menu
						   "\t  To set tiles as solid, be sure to add _w for wall, where _f is floor.\n\n"
						   "\t  To add an action key to a tile, select the tile then hit the space bar.\n\n"

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/////////////////////////////////// DLG PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function processes the messages that come to and from our dialog box
/////
/////////////////////////////////// DLG PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	// This stores the action key that the user typed in
	int actionKey = 0;

	// Let's check each message
    switch (message) 
    { 
		case WM_COMMAND:					// If there was input from the user
            switch (LOWORD(wParam))			// Check the message ID
            { 
                case IDOK:					// If the user clicked the OK button

					// Here we extract the action key from the edit field.
					actionKey = GetDlgItemInt(hWnd, IDC_EDIT1, NULL, TRUE);

					// We then set the action key for the current tile attached to our cursor.
					// After we received the action key, we don't need the dialog box any
					// more, so let's destroy it and set the handle to NULL.
					g_pCursorTile->SetActionKey(actionKey);
					DestroyWindow(g_hDlgWnd); 
					g_hDlgWnd = NULL; 
					return TRUE;

				case IDCANCEL:				// If the user clicked the cancel button

					// Destroy the window and set the handle to the dialog box to NULL
					DestroyWindow(g_hDlgWnd); 
					g_hDlgWnd = NULL; 
					return TRUE; 
        } 
        default:							// Default a return false, no important input
            return FALSE; 
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

				case VK_SHIFT:						// If the shift key is down
					g_bShiftKeyDown = true;			// Set the flag so we know for exit setting
					break;


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

				case VK_SPACE:						// If the space bar was pressed
					// If we already have the dialog box open, or we have no cursor tile, stop
					if(g_hDlgWnd || !g_pCursorTile) break;

					// Here we create the action key dialog box with 1 line of code.  This
					// pulls the resource ID IDD_DIALOG1 to use as the dialog box.  I created
					// this dialog box in Visual Studio's resource editor.  We also pass in
					// the function pointer, DlgProc(), which will be used to handle the dialog
					// box's messages.
					g_hDlgWnd = CreateDialog(g_hInstance, 
											 MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)DlgProc);

					// Now we want to show the dialog box on the screen
					ShowWindow(g_hDlgWnd, SW_SHOWNORMAL);
					break;	

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


			}
			break;

		case WM_KEYUP:								// If we let up a key
			switch (wParam) 
			{
				case VK_SHIFT:						// Check if that key we let go was the shift key
					g_bShiftKeyDown = false;		// Let us know that the user isn't using shift anymore
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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// This is where we want to do our checks to see what we should do when
			// the right mouse button is clicked.  If we don't have a tile attached to
			// our cursor, and the current type is TILE_TYPE, then we want to get a copy
			// of the current tile underneath the cursor.  This will then attach to our
			// cursor.  This makes it so we don't have to always choose tiles from the
			// tool bar.  If we already have a tile attached, however, we will just delete
			// the cursor tile.
			if(!g_pCursorTile && g_pCurrentMap->GetCurrentType() == TILE_TYPE)
			{
				// This returns a pointer to a tile under our cursor and attaches it
				g_pCursorTile = g_Map.GetTile(g_cursorPos.x/TILE_WIDTH, g_cursorPos.y/TILE_HEIGHT);
			}
			else									
				g_pCursorTile = NULL;				// Otherwise remove the cursor tile

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

            
			// If we are in TILE_TYPE, then this next thing won't apply since we don't delete them
			if(g_pCursorTile == NULL)				// If we don't have a current cursor tile
			{										
				// Here we delete the tile at our current mouse position from the map
				g_pCurrentMap->DeleteTile(g_cursorPos.x/TILE_WIDTH, g_cursorPos.y/TILE_HEIGHT);
			}

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
			break;

		// Don't let the window go inactive
		case WM_NCACTIVATE:
			return DefWindowProc(hWnd, iMsg, TRUE, lParam);

		// This gets rid of unnecessary erasing of the window (eliminates flickering)
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
				
	// Here we initialize our open file information
	g_OpenInfo.lStructSize = sizeof(OPENFILENAME);				// Set the size of the structure
	g_OpenInfo.nMaxFile = MAX_PATH;								// Set the max characters for a file name
	g_OpenInfo.lpstrFile = g_szFileName;						// Give a string to store the file name
	g_OpenInfo.lpstrFilter = "Map Files (*.map)\0*.map";		// Only accept .map files to load
	g_OpenInfo.hwndOwner = g_hWndTool;							// Assign the window owner and give it desired flags
	g_OpenInfo.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;

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
// In this tutorial we added support for action keys and collision.
// To add an action key to a current selected tile, just hit space
// bar and type in the action key you want into the dialog box.
// To do collision, just put a '_w' on the bitmap file name.
// Otherwise, if you want that tile to not be solid, just do '_f'.
//
// This concludes our basic Map Editor series.  Now you might be
// thinking, "Over???  What?!?!!?  We barely did anything?  what about
// *** THIS *** functionality or *** THIS *** functionality, or even
// *** THIS *** option???  This map editor sucks man!"  Hehe, well I
// understand what your feeling, but that's all up to YOU!  The rest
// of the options we want you to add yourself :).  Here is your base.
// Some things you might want to add is undo and redo support.  Also,
// scrolling for larger maps.  Animated tiles would be easy to add too.
//
// Good luck and may the programming force be with you!  You can do it!!!
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials
