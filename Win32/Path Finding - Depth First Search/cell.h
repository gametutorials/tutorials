#ifndef CELL_H
#define CELL_H

// Here's the defines for the different values a CCell can have
#define OPEN 0 // OPEN means the cell is "open" (ie nothing is in it)
#define SOLID 1 // SOLID means the cell is "solid" (ie this cell acts like a wall)
#define START 2 // START marks the cell where we start path finding from
#define DEST 3 // DEST marks the cell we path find to

// The following class defines a cell -- A cell is one square in our grid.  You'll notice we
// have NO .cpp file associated with this .h file.  Are we out of our mind!?!  No not at all.  
// It's totally valid to put all the implementation in the header file.  There's of course 
// reasons why you don't always do this (debugging difficulties for one), but I won't digress.
class CCell
{
	public:

		 // Default Constructor
		CCell():value(OPEN) { /* do nothing */ }

		// *** Data Access Functions

			int getWidth() { return width; } // Return width (shared by all CCell's)
			int getHeight() { return height; } // Return height (shared by all CCell's)
	
			bool hasBeenVisited() { return visited; }

			int getVal() { return value; }

		// End of Data Access Functions ***

		// Set the width of all CCell's in pixels
		void setWidth(int wid) { width = wid; }

		// Set the height of all CCell's in pixels
		void setHeight(int hgt) { height = hgt; }

		// Set "visited" to the flag passed in
		void setVisited(bool flag) { visited = flag; }

		// Set "value" to the cell value passed in
		void setVal(int val) { value = val; }

		/*  Quick recap on static member variables:

			int main()
			{
				CCell xxx;  
				CCell yyy;
				CCell zzz;

				// xxx, yyy, and zzz have the SAME width and height

				yyy.setWidth(55);

				// The cool thing is xxx, yyy, and zzz STILL have the SAME width and height
				// because static member variables are shared by all instances of that variable
				// type, whenever you change width or height, on which ever CCell variable, they
				// change for all CCell variables.

				// Also you can get/set the width/height of all CCell's by using the scope
				// resolution operator (::) like this:

				CCell::width = 64;
					return 0;

				// Now xxx, yyy, and zzz have the width of 64
			}

			Okay hopefully that refreshes your memory %).
		*/

		static int width; // Width of all CCell's in pixels (shared by all CCell variables)
		static int height; // Height of all CCell's in pixels (shared by all CCell variables)

	private:

		bool visited; // This boolean flag says whether a CCell has been
					  // visited or not (used when doing the pathing)

		int value; // Holds the value of the CCell (ie OPEN, SOLID, etc)
};


#endif
