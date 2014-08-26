#ifndef _OCTREE_H
#define _OCTREE_H

#include "Main.h"
#include <vector>
using namespace std;


// The maximum amount of triangles per node
extern int g_MaxTriangles;

// The maximum amount of subdivisions allow (Levels of subdivision)
extern int g_MaxSubdivisions;

// The amount of nodes created in the octree
extern int g_EndNodeCount;

// Turn lighting on initially
extern bool g_bLighting;

// This holds if we want lines or triangles
extern bool g_bRenderMode;


// These values (0 - 7) store the index ID's for the octree node array (m_pOctreeNodes)
enum eOctreeNodes
{
	TOP_LEFT_FRONT,			// 0
	TOP_LEFT_BACK,			// 1
	TOP_RIGHT_BACK,			// etc...
	TOP_RIGHT_FRONT,
	BOTTOM_LEFT_FRONT,
	BOTTOM_LEFT_BACK,
	BOTTOM_RIGHT_BACK,
	BOTTOM_RIGHT_FRONT
};

// This is our debug lines class to view the octree visually
class CDebug 
{

public:

	// This adds a line to our list of debug lines
	void AddDebugLine(CVector3 vPoint1, CVector3 vPoint2);

	// This adds a rectangle with a given center, width, height and depth to our list
	void AddDebugRectangle(CVector3 vCenter, float width, float height, float depth);

	// This renders all of the lines
	void RenderDebugLines();		

	// This clears all of the debug lines
	void Clear();					

private:

	// This is the vector list of all of our lines
	vector<CVector3> m_vLines;		
};


// This is our octree class
class COctree
{

public:

	// The constructor and deconstructor
	COctree();
	~COctree();

	// This returns the center of this node
	CVector3 GetCenter() {	 return m_vCenter;	}

	// This returns the triangle count stored in this node
	int GetTriangleCount()  {   return m_TriangleCount;	}

	// This returns the widht of this node (since it's a cube the height and depth are the same)
	float GetWidth() {	 return m_Width;	}

		// This returns if this node is subdivided or not
	bool IsSubDivided()  {   return m_bSubDivided;	}

	// This sets the initial width, height and depth for the whole scene
	void GetSceneDimensions(CVector3 *pVertices, int numberOfVerts);

	// This takes in the previous nodes center, width and which node ID that will be subdivided
	CVector3 GetNewNodeCenter(CVector3 vCenter, float width, int nodeID);

	// This subdivides a node depending on the triangle and node width
	void CreateNode(CVector3 *pVertices, int numberOfVerts, CVector3 vCenter, float width);

	// This cleans up the new subdivided node creation process, so our code isn't HUGE!
	void CreateNewNode(CVector3 *pVertices,	vector<bool> pList, int numberOfVerts,
				  	   CVector3 vCenter,	float width,        int triangleCount, int nodeID);

	// Once we are finished subdividing we need to assign the vertices to the end node.
	void AssignVerticesToNode(CVector3 *pVertices, int numberOfVerts);

	// This goes through each of the nodes and then draws the end nodes vertices.
	// This function should be called by starting with the root node.
	void DrawOctree(COctree *pNode);

	// This free's the data allocated in the octree and restores the variables
	void DestroyOctree();

private:

	// This initializes the data members
	void InitOctree();

	// This tells us if we have divided this node into more sub nodes
	bool m_bSubDivided;

	// This is the size of the cube for this current node
	float m_Width;

	// This holds the amount of triangles stored in this node
	int m_TriangleCount;

	// This is the center (X, Y, Z) point in this node
	CVector3 m_vCenter;

	// This stores the triangles that should be drawn with this node
	CVector3 *m_pVertices;

	// These are the eight nodes branching down from this current node
	COctree *m_pOctreeNodes[8];	
};


// This returns the cross product between 2 vectors
CVector3 Cross(CVector3 vVector1, CVector3 vVector2);

// This returns the magnitude of a vector
float Magnitude(CVector3 vNormal);

// This returns a normalized vector
CVector3 Normalize(CVector3 vVector);


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This header file contains the class prototypes for our CDebug and COctree classes.
// We also have an enum that contains ID's for each node of a subdivided list of 8.
// We could have said 0 - 7 but enum's work great to steer away form hard coded constants.
// Also, this makes it easier to read.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
