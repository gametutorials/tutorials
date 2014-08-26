#ifndef _TERRAIN_H
#define _TERRAIN_H

#define MAP_SIZE		1024				// This is the size of our .raw height map
#define STEP_SIZE		16					// This is width and height of each triangle grid

// This returns the height (0 to 255) from a heightmap given an X and Y
int Height(BYTE *pHeightMap, int X, int Y);

// This loads a .raw file of a certain size from the file
void LoadRawFile(LPSTR strName, int nSize, BYTE *pHeightMap);

// This turns heightmap data into primitives and draws them to the screen
void RenderHeightMap(BYTE *pHeightMap);


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Nothing new was added to this file.
// 
//
// ©2000-2005 GameTutorials