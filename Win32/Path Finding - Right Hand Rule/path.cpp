#include "path.h"

// Globals
CBitmap tiles[MAX_TILES]; // Our array of bitmaps for displaying
PFIND_POS Pos; // Our struct for holding all "positions" relevant on the MAP

// Inits "Pos" and loads the tiles
bool PFIND_Init()
{	
	InitPositions();

	// Set current direction to NORTH
	Pos.cur_dir = NORTH;

	return LoadTiles();
}

// Loads all the bitmaps that will be used as "tiles"
bool LoadTiles()
{
	for(int i = 0; i < MAX_TILES; i++)
	{
		if(!tiles[i].loadBMP((char*)TILE_NAMES[i]))
			return false;
	}

	return true;

}

// Init the ending and starting positions for our "maze"
void InitPositions()
{
	while(1)
	{
		// Generate a RANDOM ending position
		Pos.end_xPos = rand()%MAP_WIDTH;
		Pos.end_yPos = rand()%MAP_HEIGHT;

		// If this ending position lies on the "maze path" (ie the white tiles)
		// We've picked an ending position that will work, thus break out of the loop
		if(MAP[Pos.end_xPos + Pos.end_yPos * MAP_WIDTH] == PATH)
			break;
	}

	while(1)
	{
		// Generate a RANDOM starting position
		Pos.xPos = rand()%MAP_WIDTH;
		Pos.yPos = rand()%MAP_HEIGHT;

		// If this starting position lies on the "maze path" (ie the white tiles)
		// We've picked a starting position that will work, thus break out of the loop
		if(MAP[Pos.xPos + Pos.yPos * MAP_WIDTH] == PATH)
			break;
	}

}

// This draws the MAP (the maze) to the screen
void DrawMap(CBitmap &disp_bmp)
{
	// Loop thru the entire map blitting each "tile"
	for(int y = 0; y < MAP_HEIGHT; y++)
	{
		for(int x = 0; x < MAP_WIDTH; x++)
			DrawTile(disp_bmp.getDC(),x,y,MAP[x + y * MAP_WIDTH]);

	}

	// Blit ending square
	DrawTile(disp_bmp.getDC(),Pos.end_xPos,Pos.end_yPos,EXIT);

	// Blit current position square 
	DrawTile(disp_bmp.getDC(),Pos.xPos,Pos.yPos,PLAYER);

}

// Draws a "tile" to the screen
void DrawTile(HDC dest_dc, int mapX, int mapY, int index)
{
	// Get width and height of tile
	int tileWid = tiles[index].getWidth();
	int tileHgt = tiles[index].getHeight();

	RECT dest_rect;

	// Here we fill "dest_rect" with the location ON SCREEN of where we want to draw
	dest_rect.left = mapX * tileWid;
	dest_rect.right = dest_rect.left + tileWid;
	dest_rect.top = mapY * tileHgt;
	dest_rect.bottom = dest_rect.top + tileHgt;

	// Blit the "index" tile to the window and the specified RECT
	tiles[index].blit(dest_dc,dest_rect);
}

// Returns true if "entity" can turn right, false otherwise
bool CanTurnRight()
{
	// A quick breakdown of what "turning right means"
	// If I'm headed NORTH, turning right would turn me EAST
	// If I'm headed WEST, turning right would turn me NORTH
	// If I'm headed SOUTH, turning right would turn me WEST
	// If I'm headed EAST, turning right would turn me SOUTH

	// There's a quick explanation of treating a ONE DIMENSIONAL array as 
	// a TWO DIMENSIONAL array at the bottom -- This will hopefully help if you are unsure
	// why we are using the various "offsets" into MAP for checking the various directions

	switch(Pos.cur_dir)
	{
		
		case NORTH:

			// Notice how NO "if statement" is needed -- This will evaluate
			// to either TRUE or FALSE
			return (MAP[(Pos.xPos + 1) + Pos.yPos * MAP_WIDTH] == PATH);

		case WEST:

			return (MAP[Pos.xPos + (Pos.yPos - 1) * MAP_WIDTH] == PATH);
				
		case SOUTH:

			return (MAP[(Pos.xPos - 1) + Pos.yPos * MAP_WIDTH] == PATH);
				
		case EAST:

			return (MAP[Pos.xPos + (Pos.yPos + 1) * MAP_WIDTH] == PATH);

		default:

			return false; // Only valid directions are NORTH, WEST, SOUTH, EAST

	}

}

// Returns true if the entity can move ONE square in the "current direction"
// returns false otherwise
bool CanMoveForward()
{
	// There's a quick explanation of treating a ONE DIMENSIONAL array as 
	// a TWO DIMENSIONAL array at the bottom -- This will hopefully help if you are unsure
	// why we are using the various "offsets" into MAP for checking the various directions

	switch(Pos.cur_dir)
	{
		
		case NORTH:

			// Notice how NO "if statement" is needed -- This will evaluate
			// to either TRUE or FALSE
			return (MAP[Pos.xPos + (Pos.yPos - 1) * MAP_WIDTH] == PATH);

		case WEST:

			return (MAP[(Pos.xPos - 1) + Pos.yPos * MAP_WIDTH] == PATH);
				
		case SOUTH:

			return (MAP[Pos.xPos + (Pos.yPos + 1) * MAP_WIDTH] == PATH);
				
		case EAST:

			return (MAP[(Pos.xPos + 1) + Pos.yPos * MAP_WIDTH] == PATH);

		default:

			return false; // Only valid directions are NORTH, WEST, SOUTH, EAST
	}

} // end of CanTurnRight()

// Turn the "entity" right
void TurnRight()
{
	switch(Pos.cur_dir)
	{
		
		case NORTH:

			Pos.cur_dir = EAST;
				break;

		case WEST:

			Pos.cur_dir = NORTH;
				break;				
			
		case SOUTH:

			Pos.cur_dir = WEST;
				break;

		case EAST:

			Pos.cur_dir = SOUTH;
				break;
	}

}

// Turn the "entity" left
void TurnLeft()
{
	switch(Pos.cur_dir)
	{
		
		case NORTH:

			Pos.cur_dir = WEST;
				break;

		case WEST:

			Pos.cur_dir = SOUTH;
				break;				
			
		case SOUTH:

			Pos.cur_dir = EAST;
				break;

		case EAST:

			Pos.cur_dir = NORTH;
				break;
	}

}

// Move the "entity" ONE space forward in the current direction they are facing
void MoveForward()
{
	switch(Pos.cur_dir)
	{
			
		case NORTH:

			Pos.yPos--;
				break;

		case WEST:

			Pos.xPos--;
				break;

		case SOUTH:

			Pos.yPos++;
				break;

		case EAST:

			Pos.xPos++;
				break;

	}
}

// This "moves" the entity
void Move()
{
	// If the entity can turn right (ie there is a valid open space to their right)
	if(CanTurnRight())
	{
		TurnRight(); // Turn right
		MoveForward(); // And move forward
	}
	else if(CanMoveForward()) // Otherwise if you can move forward in the current direction
		MoveForward();		  // you are traveling in, do that
	else
		TurnLeft(); // Else, turn left
}

// Returns true if "entity" is ON TOP of the destination position, false otherwise
bool DestBeenReached()
{
	if((Pos.xPos == Pos.end_xPos) && (Pos.yPos == Pos.end_yPos))
		return true;

	return false;
}


/* A quick reminder of how to index a one dimensional array as a two dimensional array

	If this is our two dimensional array:

	2111
	1111
	1117  // with '2' being represented by the coordinates (0,0)

	And we have this stored in a one dimensional array (which lets call Array), meaning it 
	would look like this:

	int Array = {	2,  //0 -- With these number being the INDEX into the array
					1,  //1
					1,  //2
					1,  //3
					1,  //4
					1,  //5
					1,  //6
					1,  //7
					1,  //8
					1,  //9
					1,  //10
					7,  //11
				};

	If you look at the 2D array you can see that the WIDTH of it is '4' (because 
	2111 is four numbers long and all the rows below that are also four numbers long)

	So our equation to get to '7' is:  

	7's "x position" which is 3 (because it's three from the left when you start counting at 0)
	PLUS 7's "y position" which is 2 (because it's two from the top when start counting at 0)
	MULTIPLIED by the WIDTH of the 2D array (which we said was 4)

	int temp = Array[3 + (2 * 4)]

	// Which equals...

	int temp = Array[11] // which Array[11] = '7'
	
*/
	
