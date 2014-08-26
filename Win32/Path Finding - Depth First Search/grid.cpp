#include "grid.h"

// Initialize our static member variables -- Each CCell will be 32x32 pixels 
int CCell::width = 32;
int CCell::height = 32;

// Initializes all the cells in a CGrid to "open"
void CGrid::init()
{
	// Fill the entire CGrid to OPEN
	for(int i = 0; i < ROW_MAX; i++)
	{
		for(int j = 0; j < COL_MAX; j++)
		{
			cells[i][j].setVal(OPEN); // Cells should all start as "open"
			cells[i][j].setVisited(false); // No cells start out as visited
		}
	}

	dest_found = false; // Haven't found the destination yet

}

// Sets the value at grid box location (x,y) to "val"
void CGrid::setCellVal(int x, int y, int val)
{
	// Debug error checking
	assert((x >= 0) && (x < COL_MAX));
	assert((y >= 0) && (y < ROW_MAX));
	assert((val >= OPEN) && (val <= DEST));

	cells[y][x].setVal(val);
}

// Gets the value at grid box location (x,y)
int CGrid::getCellVal(int x, int y)
{
	// Debug error checking
	assert((x >= 0) && (x < COL_MAX));
	assert((y >= 0) && (y < ROW_MAX));

	return cells[y][x].getVal();
}

// Get the position we're path finding from (start location)
bool CGrid::getStartPos(int &x, int &y)
{
	// Find the START position in the CGrid if it's been set
	for(int i = 0; i < ROW_MAX; i++)
	{
		for(int j = 0; j < COL_MAX; j++)
		{
			if(cells[i][j].getVal() == START)
			{
				// Notice how we keep the (x,y) values "flip-flopped" so to stay
				// in line with the rest of the get and set functions
				x = j;
				y = i;
				return true;
			}
		}
	}

	return false;
}

// Get the position we're path finding to (ending location)
bool CGrid::getDestPos(int &x, int &y)
{
	// Find the DEST position in the CGrid if it's been set
	for(int i = 0; i < ROW_MAX; i++)
	{
		for(int j = 0; j < COL_MAX; j++)
		{
			if(cells[i][j].getVal() == DEST)
			{
				// Notice how we keep the (x,y) values "flip-flopped" so to stay
				// in line with the rest of the get and set functions
				x = j;
				y = i;
				return true;
			}
		}
	}

	return false;
}


// Draws the grid to the specified location and device context with
// (x_pos,y_pos) representing the upper-left hand corner of where to draw from
// If the path finding has been completed, draw() also draws the path
// from the start CCell to the destination CCell
void CGrid::draw(HDC dest_dc, int x_pos, int y_pos)
{
	// Debug error checking
	assert(dest_dc != NULL);

	// First draw the values of the grid squares
	drawValues(dest_dc,x_pos,y_pos);

	HPEN hpen;
	HPEN old_pen;

	// Create a "red pen" to draw the grid with
	hpen = CreatePen(PS_SOLID,2,RGB(255,10,10));

	// Select into the destination device context
	old_pen = (HPEN)SelectObject(dest_dc,hpen);

	int grid_wid = getWidth(); // Get the total grid width in pixels
	int grid_hgt = getHeight(); // Get the total grid height in pixels

	// Draw the vertical lines
	for(int x = x_pos; x <= (grid_wid + x_pos); x += CCell::width)
	{
		MoveToEx(dest_dc,x,y_pos,NULL);
		LineTo(dest_dc,x,y_pos + grid_hgt);
	}

	// Draw the horizontal lines
	for(int y = y_pos; y <= (grid_hgt + y_pos); y += CCell::height)
	{
		MoveToEx(dest_dc,x_pos,y,NULL);
		LineTo(dest_dc,x_pos + grid_wid,y);
	}

	// Put back the original pen
	SelectObject(dest_dc,old_pen);
	DeleteObject(hpen); // Delete the HPEN we made

	// If we found the destination -- Draw the path as well
	if(dest_found)
	{
		drawPath(dest_dc,x_pos,y_pos);
		Sleep(200);
			return;
	}
}

// This is where all the magic happens -- depthFirstSearch() is a recursive method that
// will get us from our starting pos to our ending pos
/*
	Here's the how a depth first search works:

	We begin at whatever our start position is -- Then we start following a path -- We 
	keep following one path throughout the entire grid until we reach it's conclusion,
	which is either a dead end OR the destination.  If it's a dead end, we back up
	and follow another path (never to follow the same path, or visit the same CCell, twice)

	So it put it another way, whenever a CCell is visited during the search, recursively
	visit all of it's unvisited neighbors.  Effectively follow one "path" throughout the 
	entire CGrid to it's conclusion (to it's full depth) then back up and follow another
	path, and so on until the destination position is reached.

	That's it -- Really the depth-first search is quite a simple concept -- Notice this
	WILL NOT find the shortest path from start to finish.  It's possible that by coincidence
	it is shortest path, but in no way is that guaranteed.  Also although this is a recursive
	function, if it's called with out there being a valid path, it's smart enough to eventually
	stop once it's tried to get to the destination position.

	The BEST way you're going to really see what is going on, is trace through an example
	in the debugger.  I suggest changing the ROW_MAX and COL_MAX of the CGrid to say 3.
	Then start the debugger and trace through one complete path find.

	For this specific rendition of the DFS algorithm we are ONLY allowing north, south,
	east, and west directions of movement -- You could easily add more, but you need at 
	least these four basic directions of movement to be able to move anywhere on the CGrid
*/
void CGrid::depthFirstSearch(int src_x, int src_y)
{
	// If we find the destination CCell we are done.  Thus we need to 
	// push the destination indices (destination CCell) onto the stack, set the "dest_found"
	// flag to true, and return
	if(cells[src_y][src_x].getVal() == DEST)
	{	
		COORD cell_indices = {src_x,src_y};
		path_nodes.push(cell_indices); // Push the destination position on the stack

		dest_found = true;
			return;
	}

	// Everytime we visit a cell we need to record that fact
	cells[src_y][src_x].setVisited(true);

	// Loop through all the possible CCell's
	for(int dest_y = 0; dest_y < ROW_MAX; dest_y++)
	{
		for(int dest_x = 0; dest_x < COL_MAX; dest_x++)
		{
			// If find an adjacent CCell (ie a CCell that is one CCell north, south, east,
			// or west of our current position)
			if(isAdjacent(src_x,src_y,dest_x,dest_y))
			{
				// And if that CCell is open (ie it hasn't been visited and it's not a wall)
				if(isOpen(dest_x,dest_y))
				{
					// Continue searching from the adjacent cell
					depthFirstSearch(dest_x,dest_y);

					// Once we have found the destination CCell, start recording
					// the (x,y) indexes in the CGrid -- Remember recursive functions
					// keep local copies of src_x and src_y -- So when we return immediately
					// after finding the destination position, src_x and scr_y will equal
					// the CGrid indexes of the destination position.  These will get
					// pushed onto our path_nodes stack FIRST.  Then we'll return
					// The next call to depthFirstSearch() will have (src_x,src_y) equal
					// a CCell's indexes that are exactly one CCell away from the destination
					// position.  This will the get pushed onto the stack second.  This 
					// continues until we work our way backward to the start CCell.
					if(dest_found)
					{
						// Save off the grid cell indices
						COORD cell_indices = {src_x,src_y};
						path_nodes.push(cell_indices);
							return;
					}
				}
			}
		
		} // end of for(int dest_x = 0; dest_x < COL_MAX; dest_x++)

	} // end of for(int dest_y = 0; dest_y < ROW_MAX; dest_y++)

} // end of void depthFirstSearch(int src_x, int src_y)


////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Private Methods

// This draws the CGrid with each CCell's representative color value
// BLUE == SOLID CCell (you can think of this as a wall)
// GREEN == START CCell (this is where we path find from)
// RED == DEST CCell (this is where we path find to)
// 
// (x_pos,y_pos) corresponds to the upper-left Win32 coordinates of where to 
// start drawing to, dest_hdc is the device context to draw to
void CGrid::drawValues(HDC dest_hdc, int x_pos, int y_pos)
{
	// Create colored brushed
	HBRUSH hbrush_blue = CreateSolidBrush(RGB(15,15,255));
	HBRUSH hbrush_green = CreateSolidBrush(RGB(15,255,15));
	HBRUSH hbrush_red = CreateSolidBrush(RGB(215,30,30));

	RECT rect; // RECT to draw to

	// Fill rect
	rect.left = x_pos;
	rect.top = y_pos;
	rect.right = rect.left + CCell::width;
	rect.bottom = rect.top + CCell::height;

	// Offset the RECT down by 2 -- We do this to achieve a psuedo 3D look
	rect.top += 2;
	rect.bottom -= 2;

	// Loop through the entire CGrid -- Fill the cells as needed
	for(int i = 0; i < ROW_MAX; i++)
	{
		for(int j = 0; j < COL_MAX; j++)
		{
			if(cells[i][j].getVal() == SOLID)
				FillRect(dest_hdc,&rect,hbrush_blue);

			else if(cells[i][j].getVal() == START)
				FillRect(dest_hdc,&rect,hbrush_green);

			else if(cells[i][j].getVal() == DEST)
				FillRect(dest_hdc,&rect,hbrush_red);

			rect.left += CCell::width;
			rect.right += CCell::width;
		}

		rect.left = x_pos;
		rect.right = rect.left + CCell::width;

		rect.top += CCell::height;
		rect.bottom += CCell::height;
	}

	// Delete the HBRUSH's we created
	DeleteObject(hbrush_blue);
	DeleteObject(hbrush_green);
	DeleteObject(hbrush_red);
}

// This draws the path from start to finish -- (x_pos, y_pos) corresponds to the
// upper-left Win32 corner of where to start drawing to -- dest_hdc is the device
// context to draw to
void CGrid::drawPath(HDC dest_hdc, int x_pos, int y_pos)
{
	// If path_nodes are empty OR we haven't found the destination
	// position yet, can't draw the path
	if(path_nodes.empty() || dest_found == false)
		return;

	HBRUSH hbrush_white = CreateSolidBrush(RGB(215,215,215));

	RECT rect; // Rect to draw to
	COORD coord; // Will hold the (x,y) indices of the CGrid of where to draw to

	coord = path_nodes.top();
	path_nodes.pop(); // It will be taken care of so remove it from the stack

	// Fill rect
	rect.left = x_pos + coord.X * CCell::width;
	rect.top = y_pos + coord.Y * CCell::height;
	rect.right = rect.left + CCell::width;
	rect.bottom = rect.top + CCell::height;

	// Shrink the RECT dimensions by 2
	rect.left += 2;
	rect.top += 2;
	rect.right -= 2;
	rect.bottom -= 2;

	FillRect(dest_hdc,&rect,hbrush_white);

	DeleteObject(hbrush_white); // Delete brush, free memory
}

// Returns true if grid square (dest_x, dest_Y) is one square to the north, east, south,
// or west of the grid square specified by (src_x,src_y)
bool CGrid::isAdjacent(int src_x, int src_y, int dest_x, int dest_y)
{
	// If it's not in range, it's not adjacent
	if((dest_x < 0) || (dest_x >= COL_MAX))
		return false;

	// If it's not in range, it's not adjacent
	if((dest_y < 0) || (dest_y >= ROW_MAX))
		return false;

	// If the X values are equal
	if(src_x == dest_x)
	{
		// The squares are adjacent if the source square is above the destination square
		if((src_y - 1) == dest_y)
			return true;

		// Or if the source square is below the destination square
		else if((src_y + 1) == dest_y)
			return true;
	}

	// If the Y values are equal
	else if(src_y == dest_y)
	{
		// The squares are adjacent if the source square is left of the destination square
		if((src_x - 1) == dest_x)
			return true;

		// Or if the source square is right of the destination square
		else if((src_x + 1) == dest_x)
			return true;
	}

	return false; // CCell (dest_x, dest_y) IS NOT adjacent to CCell (src_x, src_y)
}

// Returns true if the grid square (dest_x, dest_y) is "open" -- This means
// the CCell's value DOES NOT equal SOLID and the CCell has not already been visited
bool CGrid::isOpen(int x, int y)
{
	// CCell is not open if it's already been visited
	if(cells[y][x].hasBeenVisited())
		return false;

	// CCell is not open if it's SOLID
	if(cells[y][x].getVal() == SOLID)
		return false;

	return true; // CCell must be open for business %)
}



				
				


