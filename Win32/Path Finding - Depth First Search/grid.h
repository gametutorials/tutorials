#ifndef CGrid_H
#define CGrid_H

#include <stack>
#include <assert.h>
#include "cell.h"
#include "winobj.h"

using namespace std;

#define ROW_MAX 10 // Number of rows in our CGrid
#define COL_MAX 15 // Number of columns in our CGrid

// This is our CGrid class -- It handles all the drawing of the grid AND the actually path 
// finding -- You'll notice we use a double array of CCell's to make the CGrid.  Whenever you
// deal with double arrays, it's important to keep how you index straight.  At the bottom of
// this file you'll see the "double array layout" and the "Win32 coordinate layout" of a 
// 3x3 CGrid -- This should help clear up in your mind the row/column flip-flopping that goes
// on when dealing with a 2D array.
//
// It is important to note that our CGrid is based on the Win32 coordinate system.  So 
// when we pass in an (x,y) that's the coordinate system it should be in -- We'll do
// the conversion to 2D array coordinates (ie flip-flop them) internally
//
class CGrid
{
	public:

		// Default Constructor
		CGrid() { init(); }

		// Initializes the grid to a "default state" 
		void init();

		// Return width or height in pixels of CGrid
		int getWidth() { return CCell::width * COL_MAX; }
		int getHeight() { return CCell::height * ROW_MAX; }

		// Sets the grid cell value at (x,y) to "value" 
		void setCellVal(int x, int y, int value);											   

		int getCellVal(int x, int y); // Retrieves the value of CCell at (x,y)

		// Fills x and y with the START pos in the grid -- Returns true 
		// on success, false on failure (leaves x and y unchanged on failure)
		bool getStartPos(int &x, int &y);

		// Fills x and y with the DEST pos in the grid -- Returns true 
		// on success, false on failure (leaves x and y unchanged on failure)
		bool getDestPos(int &x, int &y);

		void draw(HDC dest_hdc, int x_pos, int y_pos); // Draws the grid starting at the
													  // upper-left Win32 coordinates
													 // (x_pos, y_pos) to the HDC passed in

		void depthFirstSearch(int x, int y); // This is the recursive function that 
											// carries out the "depth-first search"

		// Returns true or false if we've found the destination or not
		bool destFound() { return dest_found; }

		// Returns true if we're done drawing the "white square" traveling from
		// the start cell to the destination cell, false otherwise
		bool doneDrawing() { return path_nodes.empty() && dest_found; } // Slightly tricky
																	   // syntax here but
																	  // it's a good mental
																	 // exercise to figure
																	// out what this is doing
																   // if it's not obvious %)
		

	private:

		CCell cells[ROW_MAX][COL_MAX]; // The array that makes up the grid

		bool dest_found; // This gets set to true when we path find to our 
						// destination

		// We will use this to store the (x,y) indexes (2D array index)
		// into our CGrid that creates the path from the start location to the 
		// end location
		stack<COORD> path_nodes;

		// This goes through and draws the "contents" of each grid square
		// Every grid square is either: open, solid, the start pos or the end pos
		void drawValues(HDC dest_hdc, int x_pos, int y_pos);

		// Returns true if the grid square (dest_x, dest_y) is adjacent (one square
		// north, east, west, or south of) the grid square specified by (src_x,src_y)
		bool isAdjacent(int src_x, int src_y, int dest_x, int dest_y);

		// Returns true if the grid square (x, y) is open (ie the square has
		// not been visited, and is not SOLID), otherwise returns false
		bool isOpen(int x, int y);

		// Draws the path from the start position to the destination position
		void drawPath(HDC dest_hdc, int x_pos, int y_pos);
};

#endif


/*
		Use this as a guide to the constant flip-flopping (row/column, column/row)
		that goes on when dealing with 2D array indexes compared to Win32 coordinates

		// Straight double array indices
		[0][0]	[0][1]	[0][2]	
		[1][0]	[1][1]	[1][2]	
		[2][0]	[2][1]	[2][2]

		// Win32 style coordinates in (x,y) form
		(0,0)	(1,0)	(2,0)	
		(0,1)	(1,1)	(2,1)	
		(0,2)	(1,2)	(2,2)

*/
