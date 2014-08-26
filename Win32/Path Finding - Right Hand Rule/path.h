#ifndef PATH_H
#define PATH_H

#include "bitmap.h"

// Map width and height
#define MAP_WIDTH 20
#define MAP_HEIGHT 20

// Here we enumerate our indexes into our array of CBitmapS (declared in path.cpp) for
// displaying on the screen -- PATH = 0, WALL = 1 and so on...
enum TILE_INDEX {PATH,WALL,EXIT,PLAYER,MAX_TILES};
enum DIRECTION {NORTH, WEST, SOUTH, EAST}; // Same thing for the directions

// This is a "double char array" that holds ALL the names of the bitmaps were going to load
// Notice "Tile4.bmp" will match up with the "PATH" TILE_INDEX and so on...
const char TILE_NAMES[4][256] =
{
	{"Tile4.bmp"},
	{"Tile2.bmp"},
	{"Tile1.bmp"},
	{"Tile3.bmp"},
};

// Here we declare an "int array" that will define our "maze" 
// Notice how '1' matches up with TILE_INDEX "WALL" and '0' matches up with
// TILE_INDEX "PATH"
const int MAP[MAP_WIDTH * MAP_HEIGHT] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0
	1,0,1,0,0,0,0,0,1,0,0,0,1,1,1,1,1,1,1,1, // 1
	1,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,1, // 2
	1,0,1,0,1,0,1,1,0,1,1,0,1,1,1,0,1,0,1,1, // 3
	1,0,1,0,1,0,1,1,0,1,1,0,1,0,1,0,1,0,1,1, // 4
	1,0,1,0,1,0,1,1,0,1,1,0,1,0,1,0,1,0,1,1, // 5
	1,0,1,0,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,1, // 6
	1,0,1,0,1,1,1,0,0,0,1,0,1,0,0,0,1,1,0,1, // 7
	1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,0,1, // 8
	1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,1,0,1, // 9
	1,0,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,0,1, // 10
	1,0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,1,0,0,1, // 11
	1,0,1,1,0,0,0,1,1,0,1,0,1,0,1,0,1,0,1,1, // 12
	1,0,0,0,0,1,0,1,1,0,1,0,1,0,1,0,1,0,1,1, // 13
	1,0,1,1,0,1,0,0,1,0,1,0,1,1,1,0,1,0,0,1, // 14
	1,0,1,1,0,1,1,0,1,0,1,0,1,1,1,0,1,0,1,1, // 15
	1,0,1,1,1,1,1,0,1,0,1,0,0,0,1,0,1,0,1,1, // 16
	1,0,0,1,0,0,0,0,1,0,1,1,1,0,1,0,1,0,1,1, // 17
	1,1,1,1,0,1,1,1,1,0,1,0,0,0,1,0,1,0,0,1, // 18
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 19

};

// This little struct holds all the information we need to know for
// our path finding excursion
struct PFIND_POS
{
	int end_xPos; // Ending x-position in MAP coordinates
	int end_yPos; // Ending y-position in MAP coordinates

	int xPos; // Current "entity" x-position in MAP coordinates
	int yPos; // Current "entity" y-position in MAP coordinates

	DIRECTION cur_dir; // Current direction entity is traveling in
};

// Initializes everything for the "path finding"
bool PFIND_Init();

bool LoadTiles(); // Loads the tiles
void InitPositions(); // Initializes a starting and ending pos for the path

// Displays the current state of the map
void DrawMap(CBitmap &disp_bmp);
void DrawTile(HDC dest_dc, int mapX, int mapY, int index); // Draws the specified tile to the
														 // "dest_dc" at (mapX,mapY)

bool CanTurnRight(); // Returns true if "entity" can turn right, false otherwise
bool CanMoveForward(); // Returns true if "entity" can move forward, false otherwise

void TurnRight(); // Turns entity right
void TurnLeft(); // Turns entity left
void MoveForward(); // Moves one square forward in current direction

void Move(); // Moves entity

bool DestBeenReached(); // Returns true if destination has been reached, false otherwise

#endif
