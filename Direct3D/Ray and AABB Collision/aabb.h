#ifndef AABB_H
#define AABB_H

#include "ray.h"
#include "d3d_obj.h"

// Below is an ASCII diagram of an AABB.  The numbering of the corners is significant as it is
// used by the getCorner() function.
                                
//            6 _____________________________ 7
//             /|                           /|
//            / |                          / |
//           /  |                         /  |
//          /   |                        /   |
//         /    |                       /    |
//        /     |                      /     |
//       /      |                     /      |
//      /       |                    /       |
//     /        |                   /        |
//  2 /         |                3 /         |
//   /----------------------------/          |
//   |          |                 |          |
//   |          |                 |          |      
//   |          |                 |          | 
//   |          |-----------------|----------|      
//   |         / 4                |         /  5    
//   |        /                   |        /        
//   |       /                    |       /         
//   |      /                     |      /          
//   |     /                      |     /           
//   |    /                       |    /            
//   |   /                        |   /             
//   |  /                         |  /              
//   | /                          | /               
//   |/                           |/                
//   ------------------------------
//  0                              1

// Coordinate System:
//
//   +Y   +Z
//    |   /
//    |  /
//    | /
//    + ------ +X

/*	The acronym AABB stand for Axis Aligned Bounding Box.  As the full name suggests, an AABB is 
	a box with each side being perfectly aligned to the X, Y, and Z axis.  Of course which side aligns
	to which axis depends on the world coordinate system you choose.  The world coordinate system
	for this tutorial is the default Direct3D coordinate system, which is displayed in the ASCII
	drawing above.

	Typically to define a box, you need 8 points to represent the 8 corners in a box.  Now imagine 
	creating an AABB so that it is centered around (0,0,0) of the world.  Since each side, by definition,
	must align to the X, Y, and Z axis, we only need to store two points to completely define our 
	AABB.  This is because, using those two points, we can calculate the remaining six.  The two 
	points we store are called the "min" and "max" of the AABB.  The min is defined to be the
	smallest value on the X, Y, and Z axis.  The max is defined to be the largest value
	on the X, Y, and Z axis.  Given our world coordinate system and labeling of the corners, corner
	zero will always be our min, and corner seven will always be our max.
*/

class CAxisAlgnBB
{
	public:
			
		CAxisAlgnBB() { /* do nothing */ }
		CAxisAlgnBB(const CPos &min, const CPos &max);
	
		void render(int argb); // Draws the AABB
		bool intersect(const CRay &ray);  // Returns true if they intersect, false otherwise
	
		void set(const CPos &min, const CPos &max); // Set the AABB

		// Data Access ***

			CPos getCorner(int which);

			const CPos& getMin() { return mMin; }
			const CPos& getMax() { return mMax; }

		// *** End Data Access	

	private:

		CPos mMin;  // Min corner of box
		CPos mMax;  // Max corner of box
};

#endif