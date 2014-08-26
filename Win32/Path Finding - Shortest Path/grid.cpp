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
			grid_array[i][j].setCellVal(OPEN); // Cells should all start as "open"
			grid_array[i][j].setVisited(false); // No cells visited
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

	grid_array[y][x].setCellVal(val);
}

// Gets the value at grid box location (x,y)
int CGrid::getCellVal(int x, int y)
{
	// Debug error checking
	assert((x >= 0) && (x < COL_MAX));
	assert((y >= 0) && (y < ROW_MAX));

	return grid_array[y][x].getCellVal();
}

// Get the position we're path finding from (start location)
bool CGrid::getStartPos(int &x, int &y)
{
	// Find the START position in the CGrid if it's been set
	for(int i = 0; i < ROW_MAX; i++)
	{
		for(int j = 0; j < COL_MAX; j++)
		{
			if(grid_array[i][j].getCellVal() == START)
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
			if(grid_array[i][j].getCellVal() == DEST)
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
// If the path finding has been completed, drawGrid() also draws the path
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

// Here's where all the path finding goodness resides.  The pathFind() method will
// determine and fill "path_nodes" with the a shortest path from the "start cell" to
// the "destination cell" -- So lets go over how we are going to implement it:
/*
	We begin at whatever our start position is -- From there (in order) we will 
	check the north, west, east, and south "cells".  Additionally each time we are checking
	these surrounding cells we are building paths from the starting cell that includes
	these cells.  So assuming that ALL four cells where open, and NONE of these cells
	were the destination, we'd have four distinct paths that lead out from our start cell.

	So we've starting building four distinct paths from our start cell... What now?  Well
	for each path we start to build we'll add it to a local variable "master_list".  This 
	is a queue of stacks.  If you are unfamiliar with what a queue or stack is, this is where
	you stop reading this tutorial and brush up on those concepts, otherwise chances are good
	that you will become hopelessly lost.

	The "master_list", as aforementioned, is a queue of stacks -- We're using STL so we 
	don't have to worry about the implementation of the queue or the stack, all we have to do
	is use it.  Each stack in our queue will be a stack of "cells" that lead away from our
	start cell.  The first time through the loop (assuming all the north, west, east and south
	cells are open) we'd have FOUR distinct stacks of cells leading out of the start cell.  It
	would look something like this:
						
	QUEUE[0] = { STACK["North Cell"] ["Start Cell"] }		// Top of queue
	QUEUE[1] = { STACK["West Cell"] ["Start Cell"] }
	QUEUE[2] = { STACK["East Cell"] ["Start Cell"] }
	QUEUE[3] = { STACK["South Cell"] ["Start Cell"] }		// Bottom of queue

	The next time through the loop we'll pop off the TOP stack of our queue (the stack
	containing the start cell and the "north adjacent cell") -- Then we'd check to 
	see if we can move to ANY of the adjacent cells to the "north adjacent cell"
	Lets assume we can move to the north, west, and east cell (we obviously can't move
	to the south cell because that's the start cell and we've all ready visited it)
	Then what we'd have is 3 NEW paths that are added to the master list.  It would then
	look something like this:

	// N == the cell NORTH of the starting cell

	QUEUE[0] = { STACK["West Cell"] ["Start Cell"] }			// Top of queue
	QUEUE[1] = { STACK["East Cell"] ["Start Cell"] }
	QUEUE[2] = { STACK["South Cell"] ["Start Cell"] }
	QUEUE[3] = { STACK["North of N"] [N] ["Start Cell"] }
	QUEUE[4] = { STACK["West of N"] [N] ["Start Cell"] }
	QUEUE[5] = { STACK["East of N"] [N] ["Start Cell"] }		// Bottom of queue

	Basically the pattern continues in this fashion until the destination is reached
	or the "master_list" becomes empty signifying there is NO path from the start cell
	to the destination cell.
	
	This is all really wordy and high level.  So if your eyes glazed over when you were
	reading it, don't worry so did mine :) -- Hopefully it will make some more sense
	when you read the implementation
*/
bool CGrid::pathFind()
{
	/* 
	   So what's up with the COORD?  Our grid is made up of a double array of CCell's.
	   We'll use the COORD (which contains an x and y) for storing the row/column 
	   indices into our double array so we know what cell we are on.  There is an explanation
	   of row/column vs (x,y) at the bottom of grid.h in case you find yourself asking the 
	   question "What is he doing???"
	*/


	queue< stack<COORD> > master_list; // Our master list of stacks of cells that 
									  // are all valid different paths from the start cell

	stack<COORD> single_path; // This will hold ONE valid path from the start cell

	int startX, startY;

	getStartPos(startX,startY); // Get our starting position 
	grid_array[startY][startX].setVisited(true); // Set the starting position to visited

	COORD cell = {startX,startY}; // Create a COORD representing our starting postion

	single_path.push(cell); // Our first path obviously begins with the starting position
	master_list.push(single_path); // This will also be the first stack of potential 
								  // paths on our master list

	// We loop while the "master_list" is not empty -- If it becomes empty then NO 
	// path exists from the start cell to the destination cell
	while(master_list.empty() == false)
	{
		single_path = master_list.front(); // Get the TOP stack of path cells
		master_list.pop(); // ALWAYS remove the TOP of queue -- Any potential paths
						   // will be re-added at the back of the queue once we've
						   // went through checking for open adjacent cells

		cell = single_path.top(); // Get the current cell we're checking 
								 // for our current stack of path cells

		// Loop through all possible adjacent CCell's
		for(int dest_y = cell.Y - 1; dest_y <= cell.Y + 1; dest_y++)
		{
			for(int dest_x = cell.X - 1; dest_x <= cell.X + 1; dest_x++)
			{
				// We are only concerned with north, west, east, and south adjacent cells,
				// so skip all cells we don't care about
				if(!isAdjacent(cell.X,cell.Y,dest_x,dest_y))
					continue; 
				
				// If the CCell is open (ie it hasn't been visited and it's not a wall)
				if(isOpen(dest_x,dest_y))
				{
					COORD temp_cell = {dest_x, dest_y};

					// If the cell is the destination cell -- We're done!!!!!!!!!
					if(grid_array[dest_y][dest_x].getCellVal() == DEST)
					{	
						dest_found = true; // We've found the destination
						
						// Add the destination to the stack of "path_nodes"
						path_nodes.push(temp_cell);

						// Then, while the "single_path" (which is the SHORTEST PATH)
						// isn't empty, add all the CCell's (the COORD holding the row/column
						// indexes into our double array of CCell's) in "single_path" to the 
						// "path_nodes" -- This works out slick, so are path_nodes will
						// be in the correct order (ie start pos at top, end pos at bottom)
						while(single_path.empty() == false)
						{
							path_nodes.push(single_path.top());
							single_path.pop();
						}
							
						return true; // We have found the promised land!
					}

					// If we get here, then the current "open" cell is not the
					// destination cell

					// So... First set the now open cell to visited 
					grid_array[dest_y][dest_x].setVisited(true);

					// Then add it to our current path
					single_path.push(temp_cell);

					// Then add the new current path to the master list
					master_list.push(single_path);

					// Last but not least, remove the "cell" we just added to our current
					// path.  Remember we want to create all the new potential paths
					// leading away from our current "end cell" in our "current path"
					// So we pop off the cell we just added so next time through the loop
					// we'll check our previous state again
					single_path.pop();

				} // end of if(isOpen(dest_x,dest_y))
			
			} // end of for(int dest_x = cell.X - 1; dest_x <= cell.X + 1; dest_x++)

		} // end of for(int dest_y = cell.Y - 1; dest_y <= cell.Y + 1; dest_y++)

	} // end of while(master_list.empty() == false)

	return false; // No path existed

} // end of void CGrid::pathFind()

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////// Private Methods

// This draws the CGrid with each CCell's representative color value
// BLUE == SOLID CCell (you can think of this as a wall)
// GREEN == START CCell (this is where we path find from)
// RED == DEST CCell (this is where we path find to)
// 
// dest_hdc is the device context to draw to, (x_pos,y_pos) corresponds to the upper-left
// Win32 coordinates of where to start drawing to
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
			if(grid_array[i][j].getCellVal() == SOLID)
				FillRect(dest_hdc,&rect,hbrush_blue);

			else if(grid_array[i][j].getCellVal() == START)
				FillRect(dest_hdc,&rect,hbrush_green);

			else if(grid_array[i][j].getCellVal() == DEST)
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

// This draws the path from start to finish -- dest_hdc is the device
// context to draw to -- (x_pos, y_pos) corresponds to the
// upper-left Win32 corner of where to start drawing to
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
	if(grid_array[y][x].hasBeenVisited())
		return false;

	// CCell is not open if it's SOLID
	if(grid_array[y][x].getCellVal() == SOLID)
		return false;

	return true; // CCell must be open for business %)
}


	






				
				


