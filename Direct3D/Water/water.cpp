#include <stdio.h>
#include "water.h"

const CPos kWaterCen(0.0f, 0.0f, 0.0f); // Water is always centered at (0,0,0)

// Helper function for determining if a float is less than or equal to
// another float
inline bool ToleranceLE(float a, float b, float tolerance = 0.001f)
{
	return ((a - tolerance) <= b);
}

// Constructor
CWater::CWater()
{
	memset(mWaveTable, 0, sizeof(mWaveTable));
	
	mVerts = NULL;
	mIndices = NULL;
	
	mVertCount = 0;
	mIndexCount = 0;
	mTesselation = 0;
	
	// "texture" gets initialized by it's constructor 
}

// Init the water object, returns true on success, false otherwise
bool CWater::init()
{
	// Fill the wave table
	float angInc = 360.0f / (float)kWaveTableSize;
	float angle = 0.0f;
	
	// Fill in the table with values of the sine function
	for(int i = 0; i < kWaveTableSize; ++i)
	{
		mWaveTable[i] = sin(DEG2RAD(angle));
		angle += angInc;
	}

	// Create the tessellated quad
	if(!createGeometery(kWaterWid, kWaterHgt, kWaterTesselation))
		return false;
	
	// Load the water texture
	return mTexture.load("water.jpg"); 
}

// Process the water every frame
void CWater::process()
{
	static int startIndex = 0;
	
	int waveIndex = startIndex;
	int vertIndex = 0;
	
	// Loop through all rows of triangles
	for(int i = 0; i <= mTesselation; ++i)
	{
		// Loop through all the columns of triangles
		for(int j = 0; j <= mTesselation; ++j, ++vertIndex)
		{
			// Perturb each vertex
			mVerts[vertIndex].y += mWaveTable[waveIndex] * kWaterForce;
			
			if(++waveIndex == kWaveTableSize)
				waveIndex = 0;
		}
		
	}
	
	// Keep cycling thorough the mWaveTable
	if(++startIndex == kWaveTableSize)
		startIndex = 0;
}

// Render the water object as a triangle strip
void CWater::render()
{
	bool b = g3D->setAlphaBlendMode(true); // Turn on alpha blending

	mTexture.select(); // Select the water texture
	g3D->render(mVerts, mVertCount, mIndices, mIndexCount); // Render the tessellated quad
	
	g3D->setAlphaBlendMode(false); // Turn off alpha blending
}

// Creates a tesselated quad of (width x height) dimensions
// Number of triangles in quad is (tessAmt * tessAmt * 2)
bool CWater::createGeometery(float width, float height, int tessAmt)
{
	if(width < 0.0f || height < 0.0f || tessAmt <= 0)
		return false;

	// Set tesselation amount
	mTesselation = tessAmt;
		
	// Calculate vertex count.  We know that we have (tesselation * tesselation * 2) triangles
	// with 3 vertices per triangle.  So each row of vertices will contain "number of rows + 1"
	// vertices and each column of vertices well contain "number of columns + 1" vertices.  This
	// makes the total number of vertices equal the following equation
	mVertCount = (mTesselation + 1) * (mTesselation + 1);
	
	// Calculate index count.  We know that we'll have (tesselation * tesselation * 2) triangles
	// and we have 3 vertices per triangle, thus the below equation
	mIndexCount = mTesselation * mTesselation * 6;
	
	// Allocate space for vertices and indices
	mVerts = new SVertex[mVertCount];
	mIndices = new WORD[mIndexCount];
	
	if(mVerts == NULL || mIndices == NULL)
		return false;	
	
	// Get the sub divided width and height of each triangle
	float subDivideWid = width * (1.0f / (float)mTesselation);
	float subDivideHgt = height * (1.0f / (float)mTesselation);
	
	// Calculate the starting (x,y) and ending (x,y) of the quad 
	float startX = kWaterCen.x - (width / 2.0f);
	float startY = kWaterCen.z - (height / 2.0f);
	float endX = kWaterCen.x + (width / 2.0f);
	float endY = kWaterCen.z + (height / 2.0f);
	
	float uCoord = 0.0f; // Holds the current trinagle's U coordinate
	float vCoord = 1.0f; // Holds the current triangle's V coordinate
	float uvInc = 1.0f / (float)mTesselation; // Calculate the amount to add to each UV coordinate
	
	int i = 0; // Index for all of for...loops
	
	// Create all the vertices
	for(float y = startY; ToleranceLE(y, endY); y += subDivideHgt)
	{
		for(float x = startX; ToleranceLE(x, endX); x += subDivideWid, ++i)
		{
			// Set the value for the current vert
			mVerts[i].x = x;
			mVerts[i].y = kWaterCen.y;
			mVerts[i].z = y;
			mVerts[i].u = uCoord;
			mVerts[i].v = vCoord;
			mVerts[i].color = D3DCOLOR_ARGB(128, 255, 255, 255);
			
			uCoord += uvInc;			
		}
		
		// Set UV for next row of vertices
		uCoord = 0.0f;
		vCoord -= uvInc;
	}
	
	// Make sure we calculatd the vertex count correctly
	assert(mVertCount == i);
	
	int indexValue = 0;
	int nextRow = 0;
	
	// Look at this sample 2 x 2 quad to understand the pattern used to fill in the indices
	/*
		6----7----8		Index count =  24
		|\ 	 |\   |		Tesselation = 2	
		| \  | \  | 	
		|  \ |  \ |		Pattern is:	0, 3, 1
		|   \|   \|					1, 3, 4
		3----4----5					1, 4, 2
		|\ 	 |\   |					2, 4, 5
		| \  | \  | 				3, 6, 4
		|  \ |  \ |					4, 6, 7
		|   \|   \|					4, 7, 5,
		0----1----2					5, 7, 8
	*/
	
	// Create all the indices
	for(i = 0; i < mIndexCount; i += 6)
	{
		mIndices[i] = indexValue;
		mIndices[i + 1] = indexValue + (mTesselation + 1);
		mIndices[i + 2] = mIndices[i] + 1;
		
		indexValue++;
		
		mIndices[i + 3] = indexValue;
		mIndices[i + 4] = indexValue + mTesselation;
		mIndices[i + 5] = mIndices[i + 4] + 1;
		
		if(++nextRow == mTesselation)
		{
			indexValue++;
			nextRow = 0;
		}
	}
	
	return true;
}

// Free the memory
CWater::~CWater()
{
	if(mVerts)
	{
		delete[] mVerts;
		mVerts = NULL;
	}
	
	if(mIndices)
	{
		delete[] mIndices;
		mIndices = NULL;
	}
}
		
		
