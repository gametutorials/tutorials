#ifndef WATER_H
#define WATER_H

#include "d3d_obj.h"
#include "d3d_texture.h"

/////////////
// Macros //
///////////

#define DEG2RAD(x) (x * (D3DX_PI / 180.0f)) // Converts degrees to radians
#define RAD2DEG(x) (x * (180.0f / D3DX_PI)) // Converts radians to degrees

#define F2I(x) ( (int)((x) + 0.5f) ) // Converts a float to int with proper rounding

////////////////
// Constants //
//////////////

const int kWaterTesselation = 32; // Number of rows X cols of tessellated quads in water
const int kWaveTableSize = 36; // Number of entries in the wave table

const float kWaterForce = 1.0f / 25.0f; // Controls the "choppiness" of the water (Range is 0.0f - 1.0f)
								       // 0.0f == Water is still 
								      // 1.0f == Water is VERY choppy (so choppy it looks very bad)
const float kWaterWid = 30.0f; // Width of water (along X-axis)
const float kWaterHgt = 30.0f; // Height of water (along Z-axis)

class CWater
{
	public:
	
		CWater(); // Constructor
		
		bool init(); // Initialize the water object
		void process(); // Process the water object (apply sin() wave to Y value of verts)
		void render(); // Render the water
		
		~CWater(); // Deconstructor

	private:
	
		bool createGeometery(float width, float height, int tesselation);
	
		CD3DTexture mTexture;
		SVertex *mVerts;
		WORD *mIndices;
		
		int mVertCount;
		int mIndexCount;
		int mTesselation;
		
		float mWaveTable[kWaveTableSize];		
};

#endif
