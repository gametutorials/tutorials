//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader 4									 //
//																		 //
//		$Description:	This shows how to check collision with BSP files //
//																		 //
//***********************************************************************//

#include "Main.h"
#include "Camera.h"
#include "Quake3Bsp.h"
#include "Frustum.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// * Knowledge Assumption *
//
// Okay, this is going to be a bit of explanation that will involve patience for
// those who are new to collision detection.  I recommend viewing our simple
// tutorials that deal with basic collision detection.  So I don't have to explain
// every little thing about collision, I will just assume that you have a basic
// knowledge of how to do basic collision detection tests like a ray and plane, etc...
// 
// Okay, are you excited to actually get this collision detection under your belt?!?!  
// Well, if not, I am excited enough for the both of us just teaching it :)  Here is how
// it works:
//
// 1)  We take our start and end position of the camera, which makes a ray (line or vector).
//     We then use that movement vector and test it against the brushes, which are
//     just a bunch of planes that make up a convex volume, like a box or platform.
//	   
// 2)  With our movement vector (startPos - endPos) we test that against all the planes
//     of a brush.  There is brushSides which store the plane information like the normal
//     and the distance the plane is from the origin.  This is necessary to have for doing
//     the plane equation (Ax + By + Cz + D = 0).  If you don't understand this, please read
//     our other tutorials on collision to get the basics of how collision works.  If you
//     don't get the plane equation, you won't get this tutorial.  
//
//     * Plane Equation Quick Review *
//     
//     Since I know many of you reading this will be to excited to get collision working 
//     you will ignore it, so I'll tell you about the plane equation in a sentence or 2 (or 6) :)  
//     The plane equation is a way to test a point against a plane to see how far the point 
//     is from the plane.  If the point used to test is a point ON the plane, then the result 
//     will be 0, which means that the point has no distance from the plane since it's on it.
//     What we do is not use a point on the plane, but we want to check the points of our
//     movement vector against the brush planes.  The ABC of the plane equation is the normal
//     of the plane (A = NormalX, B = NormalY, C = NormalZ).  the xyz is the point we are
//     going to test.  The D is the distance the plane is from the origin.  If the result of
//     the equation is positive, that means the point we checked against the normal of the plane
//     is in front of the plane, whereas if it's negative it will of course be behind the plane.
//     If both points of our movement vector are in front of all the planes (positive), then we
//     most likely didn't collide into the hull of the object.  This of course isn't always
//     true since we could have clipped a corner of the brush.  We do a special algorithm to
//     check this.  It's actually quite brilliant, ... too bad I didn't invent it :)
//
// 3)  If our end point isn't inside of the brush (behind all of the planes), then we don't
//     just stop there, we need to check to see if we either hit a corner of the brush or
//     our speed was so great that we happened to clear straight through the whole brush in
//     one frame.  We do this by creating a startRatio and an endRatio.  We set the startRatio
//     at -1.0 and put the endRatio at 1.0 to represent our start and end position.  We then
//     go through each of the brush sides and check find the intersection point (or distance
//     ratio) for each plane.  BUT, we only get the intersection ratio for the startRatio
//     if the startRatio (startPosition) is in front of the plane.  Otherwise we check the
//     other planes with the endRatio (endPosition).  When we get the ratio of the collision
//     point on the plane for the start and end ratio, we set the start and end ratio with
//     the new ratio (collision intersection ratio on the plane).  If the startRatio is greater
//     than the endRatio, there is no collision, otherwise we collide with the brush!
//    
//     No matter how hard you explain this, it probably won't make sense unless you see a
//     picture (I'm at least a visual person).  Frank Puig Placeres ( fpuig2003@yahoo.com )
//     drew a great example of how this collision works.  Check out this picture:
//
//     BrushCollision.htm (The picture that describes how we check brush collision)
//
// 4)  If we didn't have a collision, we just return with no change, but if we did have a
//     collision, we need to move the camera (or player) collision back in front of the
//     brush planes that it collided with.  After we know there is a collision, we don't
//     need the endRatio data, but we need to store the last startRatio.  We store that in
//     a variable called the traceRatio (m_traceRatio).  You will find that when doing
//     collision and working with BSP info, the word "Trace" will be used often.  It isn't
//     intuitive to me as much as other words, but I chose to use it so that you will be
//     familiar with it when you see it everywhere else.
//
// 5)  Now that we have the last startRatio and stored it in the m_traceRatio, we can move
//     our camera (or player) to the correct position so that it isn't inside the brush.
//     we do that with this equation:
//
//	   vNewPosition = vStart + (vEnd - vStart) * m_traceRatio;
//
//     Now if you are like me and don't always undersatnd math when you first see it,
//     let me explain why this works.  The vStart is our start position of the camera
//     before we tried to move.  We want to start from our start position and add the
//     ratio that is from our start position to the collision point on the brush's plane.
//     Since the ratio isn't a vector, it's just a distance, we need to take that distance
//     and make it into a vector, which will be added to vStart (our initial position).
//     To turn the distance into a vector and give it a direction, we multiply the
//     distance from the start position to the collision point (m_traceRatio) by the
//     vector of our direction that we were headed, which equates to our end point
//     minus our initial position (vEnd - vStart).  This gives us a vector to add to
//     our initial position (vStart) that has a distance and a direction to the collision
//     point on the plane (intersection point).
//
// 6)  Finally, we set our camera position to the new position (vNewPosition) so that
//     we aren't allow to go through the walls and world data.  This is how it works!
//
// So these are the basic six steps of what we are doing in this tutorial to test collision.
// Of course, in the next tutorials we will get a bit more complicated when we deal with
// sliding, gravity and climbing stairs, but for now this is important to initially understand
// in order to progress and code the rest of the physics for your project.  We actually
// will be working with a kEpsilon value to compensate for floating point errors, which
// is 0.03125 (This is what Quake uses).  This is because 0.0 doesn't always mean a clean
// 0.0 because of floating point problems.  It could mean 0.0000001898908 and we wouldn't
// ever really know.  Unless we don't specifically set a float to 0, it's dangerous and
// uncertain to assume that it's ever really 0.
//
// Well, let's stop talking about the code and actually get to the guts of BSP Collision!
// (Be sure to check out BrushCollision.htm before looking at the code so you understand
//  what we are actually trying to do).
//
//

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our global frustum class, which is used to cull BSP leafs
extern CFrustum g_Frustum;

// This will store how many faces are drawn and are seen by the camera
extern int g_VisibleFaces;

// This tells us if we want to render the lightmaps
extern bool g_bLightmaps;

// This holds the gamma value that was stored in the config file
extern float g_Gamma;

// This tells us if we want to render the textures
extern bool g_bTextures;


//////////////////////////// CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our object's constructor to initial all it's data members
/////
//////////////////////////// CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuake3BSP::CQuake3BSP()
{
	// Here we simply initialize our member variables to 0
	m_numOfVerts		= 0;	
	m_numOfFaces		= 0;
	m_numOfIndices		= 0;	
	m_numOfTextures		= 0;
	m_numOfLightmaps	= 0;
	m_numOfNodes		= 0;
	m_numOfLeafs		= 0;
	m_numOfLeafFaces	= 0;
	m_numOfPlanes		= 0;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// We added some new variables so we need to initialize them like good girls/boys
	m_numOfBrushes		= 0;
	m_numOfBrushSides	= 0;
	m_numOfLeafBrushes	= 0;

	m_traceRatio		= 0;
	m_traceType			= 0;
	m_traceRadius		= 0;

	// This stores if we collided or not
	bool m_bCollided = false;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	
	// Initialize all the dynamic BSP data pointers to NULL
	m_pVerts		 = NULL;	
	m_pFaces		 = NULL;	
	m_pIndices		 = NULL;	
	m_pNodes		 = NULL;
	m_pLeafs		 = NULL;
	m_pPlanes		 = NULL;
	m_pLeafFaces	 = NULL;

	memset(&m_clusters, 0, sizeof(tBSPVisData));
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Here we initialize our dynamic arrays of data for the brushe information of the BSP
	m_pBrushes       = NULL;
	m_pBrushSides	 = NULL;
	m_pTextures      = NULL;
	m_pLeafBrushes	 = NULL;
	
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

}


//////////////////////////// CHANGE GAMMA \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This manually changes the gamma of an image
/////
//////////////////////////// CHANGE GAMMA \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::ChangeGamma(byte *pImage, int size, float factor)
{
	// Go through every pixel in the lightmap
	for(int i = 0; i < size / 3; i++, pImage += 3) 
	{
		float scale = 1.0f, temp = 0.0f;
		float r = 0, g = 0, b = 0;

		// extract the current RGB values
		r = (float)pImage[0];
		g = (float)pImage[1];
		b = (float)pImage[2];

		// Multiply the factor by the RGB values, while keeping it to a 255 ratio
		r = r * factor / 255.0f;
		g = g * factor / 255.0f;
		b = b * factor / 255.0f;
		
		// Check if the the values went past the highest value
		if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
		if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
		if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

		// Get the scale for this pixel and multiply it by our pixel values
		scale*=255.0f;		
		r*=scale;	g*=scale;	b*=scale;

		// Assign the new gamma'nized RGB values to our image
		pImage[0] = (byte)r;
		pImage[1] = (byte)g;
		pImage[2] = (byte)b;
	}
}


////////////////////////////// CREATE LIGHTMAP TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a texture map from the light map image bits
/////
////////////////////////////// CREATE LIGHTMAP TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::CreateLightmapTexture(UINT &texture, byte *pImageBits, int width, int height)
{
	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &texture);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// Change the lightmap gamma values by our desired gamma
	ChangeGamma(pImageBits, width*height*3, g_Gamma);

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, pImageBits);

	//Assign the mip map levels		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}


//////////////////////////// FIND TEXTURE EXTENSION \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This attaches the image extension to the texture name, if found
/////
//////////////////////////// FIND TEXTURE EXTENSION \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::FindTextureExtension(char *strFileName)
{
	char strJPGPath[MAX_PATH] = {0};
	char strTGAPath[MAX_PATH]    = {0}; 
	FILE *fp = NULL;

	// Get the current path we are in
	GetCurrentDirectory(MAX_PATH, strJPGPath);

	// Add on a '\' and the file name to the end of the current path.
	// We create 2 seperate strings to test each image extension.
	strcat(strJPGPath, "\\");
	strcat(strJPGPath, strFileName);
	strcpy(strTGAPath, strJPGPath);
	
	// Add the extensions on to the file name and path
	strcat(strJPGPath, ".jpg");
	strcat(strTGAPath, ".tga");

	// Check if there is a jpeg file with the texture name
	if((fp = fopen(strJPGPath, "rb")) != NULL)
	{
		// If so, then let's add ".jpg" onto the file name and return
		strcat(strFileName, ".jpg");
		return;
	}

	// Check if there is a targa file with the texture name
	if((fp = fopen(strTGAPath, "rb")) != NULL)
	{
		// If so, then let's add a ".tga" onto the file name and return
		strcat(strFileName, ".tga");
		return;
	}
}


//////////////////////////// LOAD BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This loads in all of the .bsp data for the level
/////
//////////////////////////// LOAD BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CQuake3BSP::LoadBSP(const char *strFileName)
{
	FILE *fp = NULL;
	int i = 0;
	
	// Check if the .bsp file could be opened
	if((fp = fopen(strFileName, "rb")) == NULL)
	{
		// Display an error message and quit if the file can't be found.
		MessageBox(g_hWnd, "Could not find BSP file!", "Error", MB_OK);
		return false;
	}

	// Initialize the header and lump structures
	tBSPHeader header = {0};
	tBSPLump lumps[kMaxLumps] = {0};

	// Read in the header and lump data
	fread(&header, 1, sizeof(tBSPHeader), fp);
	fread(&lumps, kMaxLumps, sizeof(tBSPLump), fp);

	// Now we know all the information about our file.  We can
	// then allocate the needed memory for our member variables.

	// Allocate the vertex memory
	m_numOfVerts = lumps[kVertices].length / sizeof(tBSPVertex);
	m_pVerts     = new tBSPVertex [m_numOfVerts];

	// Allocate the face memory
	m_numOfFaces = lumps[kFaces].length / sizeof(tBSPFace);
	m_pFaces     = new tBSPFace [m_numOfFaces];

	// Allocate the index memory
	m_numOfIndices = lumps[kIndices].length / sizeof(int);
	m_pIndices     = new int [m_numOfIndices];


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// We used to just get the texture data, then delete it, but we need the texture
	// info to tell us what type of brush that we collide with (solid, water, sprite, etc...)
	// Now we store m_pTextures in our BSP Class to get that information.

	// Allocate memory to read in the texture information.
	m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
	m_pTextures = new tBSPTexture [m_numOfTextures];

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Allocate memory to read in the lightmap data.  
	m_numOfLightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap *pLightmaps = new tBSPLightmap [m_numOfLightmaps ];

	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[kVertices].offset, SEEK_SET);

	// Go through all of the vertices that need to be read and swap axis's
	for(i = 0; i < m_numOfVerts; i++)
	{
		// Read in the current vertex
		fread(&m_pVerts[i], 1, sizeof(tBSPVertex), fp);
		
		// Swap the y and z values, and negate the new z so Y is up.
		float temp = m_pVerts[i].vPosition.y;
		m_pVerts[i].vPosition.y = m_pVerts[i].vPosition.z;
		m_pVerts[i].vPosition.z = -temp;
	}	

	// Seek to the position in the file that stores the index information
	fseek(fp, lumps[kIndices].offset, SEEK_SET);

	// Read in all the index information
	fread(m_pIndices, m_numOfIndices, sizeof(int), fp);

	// Seek to the position in the file that stores the face information
	fseek(fp, lumps[kFaces].offset, SEEK_SET);

	// Read in all the face information
	fread(m_pFaces, m_numOfFaces, sizeof(tBSPFace), fp);

	// Seek to the position in the file that stores the texture information
	fseek(fp, lumps[kTextures].offset, SEEK_SET);
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This isn't new code, but we renamed pTextures to m_pTextures.  This is now
	// added to our BSP Class, which means we don't delete it immediately afterwards
	// like in the previous tutorials.

	// Read in all the texture information
	fread(m_pTextures, m_numOfTextures, sizeof(tBSPTexture), fp);

	// Go through all of the textures
	for(i = 0; i < m_numOfTextures; i++)
	{
		// Find the extension if any and append it to the file name
		FindTextureExtension(m_pTextures[i].strName);
		
		// Create a texture from the image
		CreateTexture(m_textures[i], m_pTextures[i].strName);
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Seek to the position in the file that stores the lightmap information
	fseek(fp, lumps[kLightmaps].offset, SEEK_SET);

	// Go through all of the lightmaps and read them in
	for(i = 0; i < m_numOfLightmaps ; i++)
	{
		// Read in the RGB data for each lightmap
		fread(&pLightmaps[i], 1, sizeof(tBSPLightmap), fp);

		// Create a texture map for each lightmap that is read in.  The lightmaps
		// are always 128 by 128.
		CreateLightmapTexture(m_lightmaps[i], 
							 (unsigned char *)pLightmaps[i].imageBits, 128, 128);
	}

	// Delete the image bits because we are already done with them
	delete [] pLightmaps;

	// Store the number of nodes and allocate the memory to hold them
	m_numOfNodes = lumps[kNodes].length / sizeof(tBSPNode);
	m_pNodes     = new tBSPNode [m_numOfNodes];

	// Seek to the position in the file that hold the nodes and store them in m_pNodes
	fseek(fp, lumps[kNodes].offset, SEEK_SET);
	fread(m_pNodes, m_numOfNodes, sizeof(tBSPNode), fp);

	// Store the number of leafs and allocate the memory to hold them
	m_numOfLeafs = lumps[kLeafs].length / sizeof(tBSPLeaf);
	m_pLeafs     = new tBSPLeaf [m_numOfLeafs];

	// Seek to the position in the file that holds the leafs and store them in m_pLeafs
	fseek(fp, lumps[kLeafs].offset, SEEK_SET);
	fread(m_pLeafs, m_numOfLeafs, sizeof(tBSPLeaf), fp);

	// Now we need to go through and convert all the leaf bounding boxes
	// to the normal OpenGL Y up axis.
	for(i = 0; i < m_numOfLeafs; i++)
	{
		// Swap the min y and z values, then negate the new Z
		int temp = m_pLeafs[i].min.y;
		m_pLeafs[i].min.y = m_pLeafs[i].min.z;
		m_pLeafs[i].min.z = -temp;

		// Swap the max y and z values, then negate the new Z
		temp = m_pLeafs[i].max.y;
		m_pLeafs[i].max.y = m_pLeafs[i].max.z;
		m_pLeafs[i].max.z = -temp;
	}

	// Store the number of leaf faces and allocate the memory for them
	m_numOfLeafFaces = lumps[kLeafFaces].length / sizeof(int);
	m_pLeafFaces     = new int [m_numOfLeafFaces];

	// Seek to the leaf faces lump, then read it's data
	fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
	fread(m_pLeafFaces, m_numOfLeafFaces, sizeof(int), fp);

	// Store the number of planes, then allocate memory to hold them
	m_numOfPlanes = lumps[kPlanes].length / sizeof(tBSPPlane);
	m_pPlanes     = new tBSPPlane [m_numOfPlanes];

	// Seek to the planes lump in the file, then read them into m_pPlanes
	fseek(fp, lumps[kPlanes].offset, SEEK_SET);
	fread(m_pPlanes, m_numOfPlanes, sizeof(tBSPPlane), fp);

	// Go through every plane and convert it's normal to the Y-axis being up
	for(i = 0; i < m_numOfPlanes; i++)
	{
		float temp = m_pPlanes[i].vNormal.y;
		m_pPlanes[i].vNormal.y = m_pPlanes[i].vNormal.z;
		m_pPlanes[i].vNormal.z = -temp;
	}

	// Seek to the position in the file that holds the visibility lump
	fseek(fp, lumps[kVisData].offset, SEEK_SET);

	// Check if there is any visibility information first
	if(lumps[kVisData].length) 
	{
		// Read in the number of vectors and each vector's size
		fread(&(m_clusters.numOfClusters),	 1, sizeof(int), fp);
		fread(&(m_clusters.bytesPerCluster), 1, sizeof(int), fp);

		// Allocate the memory for the cluster bitsets
		int size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
		m_clusters.pBitsets = new byte [size];

		// Read in the all the visibility bitsets for each cluster
		fread(m_clusters.pBitsets, 1, sizeof(byte) * size, fp);
	}
	// Otherwise, we don't have any visibility data (prevents a crash)
	else
		m_clusters.pBitsets = NULL;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Like we do for other data, we read get the size of brushes and allocate memory
	m_numOfBrushes = lumps[kBrushes].length / sizeof(int);
	m_pBrushes     = new tBSPBrush [m_numOfBrushes];
	
	// Here we read in the brush information from the BSP file
	fseek(fp, lumps[kBrushes].offset, SEEK_SET);
	fread(m_pBrushes, m_numOfBrushes, sizeof(tBSPBrush), fp);

	// Get the size of brush sides, then allocate memory for it
	m_numOfBrushSides = lumps[kBrushSides].length / sizeof(int);
	m_pBrushSides     = new tBSPBrushSide [m_numOfBrushSides];

	// Read in the brush sides data
	fseek(fp, lumps[kBrushSides].offset, SEEK_SET);
	fread(m_pBrushSides, m_numOfBrushSides, sizeof(tBSPBrushSide), fp);

	// Read in the number of leaf brushes and allocate memory for it
	m_numOfLeafBrushes = lumps[kLeafBrushes].length / sizeof(int);
	m_pLeafBrushes     = new int [m_numOfLeafBrushes];

	// Finally, read in the leaf brushes for traversing the bsp tree with brushes
	fseek(fp, lumps[kLeafBrushes].offset, SEEK_SET);
	fread(m_pLeafBrushes, m_numOfLeafBrushes, sizeof(int), fp);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Close the file
	fclose(fp);

	// Here we allocate enough bits to store all the faces for our bitset
	m_FacesDrawn.Resize(m_numOfFaces);

	// Return a success
	return true;
}


//////////////////////////// FIND LEAF \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the leaf our camera is in
/////
//////////////////////////// FIND LEAF \\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CQuake3BSP::FindLeaf(const CVector3 &vPos)
{
	int i = 0;
	float distance = 0.0f;
	
	// Continue looping until we find a negative index
	while(i >= 0)
	{
		// Get the current node, then find the slitter plane from that
		// node's plane index.  Notice that we use a constant reference
		// to store the plane and node so we get some optimization.
		const tBSPNode&  node = m_pNodes[i];
		const tBSPPlane& plane = m_pPlanes[node.plane];

		// Use the Plane Equation (Ax + by + Cz + D = 0) to find if the
		// camera is in front of or behind the current splitter plane.
        distance =	plane.vNormal.x * vPos.x + 
					plane.vNormal.y * vPos.y + 
					plane.vNormal.z * vPos.z - plane.d;

		// If the camera is in front of the plane
        if(distance >= 0)	
		{
			// Assign the current node to the node in front of itself
            i = node.front;
        }
		// Else if the camera is behind the plane
        else		
		{
			// Assign the current node to the node behind itself
            i = node.back;
        }
    }

	// Return the leaf index (same thing as saying:  return -(i + 1)).
    return ~i;  // Binary operation
}


//////////////////////////// IS CLUSTER VISIBLE \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tells us if the "current" cluster can see the "test" cluster
/////
//////////////////////////// IS CLUSTER VISIBLE \\\\\\\\\\\\\\\\\\\\\\\\\\\*

inline int CQuake3BSP::IsClusterVisible(int current, int test)
{
	// Make sure we have valid memory and that the current cluster is > 0.
	// If we don't have any memory or a negative cluster, return a visibility (1).
	if(!m_clusters.pBitsets || current < 0) return 1;

	// Use binary math to get the 8 bit visibility set for the current cluster
	byte visSet = m_clusters.pBitsets[(current*m_clusters.bytesPerCluster) + (test / 8)];
	
	// Now that we have our vector (bitset), do some bit shifting to find if
	// the "test" cluster is visible from the "current" cluster, according to the bitset.
	int result = visSet & (1 << ((test) & 7));

	// Return the result ( either 1 (visible) or 0 (not visible) )
	return ( result );
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is where all the juice of our tutorial starts.  Put on your thinking caps
// because it's going to maybe be a bit to wrap yourself around at first.
// You will notice that we use the dot product quite a bit in this tutorial.
// This is because the plane equation actually is a dot product.  Ax + By + Cz
// is essentially a dot product.  We just minus the distance from the origin and
// we got the plane equation.  To cut down on code, we use the Dot() function
// when calculating the plane equation to determine the points against the brush planes.
//
//

/////////////////////////////////// DOT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This computers the dot product of 2 vectors
/////
/////////////////////////////////// DOT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Dot(CVector3 vVector1, CVector3 vVector2) 
{
	// The dot product is this equation: V1.V2 = (V1.x * V2.x  +  V1.y * V2.y  +  V1.z * V2.z)
	// In math terms, it looks like this:  V1.V2 = ||V1|| ||V2|| cos(theta)

	//    (V1.x * V2.x        +        V1.y * V2.y        +        V1.z * V2.z)
	return ( (vVector1.x * vVector2.x) + (vVector1.y * vVector2.y) + (vVector1.z * vVector2.z) );
}


/////////////////////////////////// TRACE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a start and end position (general) to test against the BSP brushes
/////
/////////////////////////////////// TRACE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 CQuake3BSP::Trace(CVector3 vStart, CVector3 vEnd)
{
	// Initially we set our trace ratio to 1.0f, which means that we don't have
	// a collision or intersection point, so we can move freely.
	m_traceRatio = 1.0f;
	
	// We start out with the first node (0), setting our start and end ratio to 0 and 1.
	// We will recursively go through all of the nodes to see which brushes we should check.
	CheckNode(0, 0.0f, 1.0f, vStart, vEnd);

	// If the traceRatio is STILL 1.0f, then we never collided and just return our end position
	if(m_traceRatio == 1.0f)
	{
		return vEnd;
	}
	else	// Else COLLISION!!!!
	{
		// If we get here then it's assumed that we collided and need to move the position
		// the correct distance from the starting position a position around the intersection
		// point.  This is done by the cool equation below (described in detail at top of page).

		// Set our new position to a position that is right up to the brush we collided with
		CVector3 vNewPosition = vStart + ((vEnd - vStart) * m_traceRatio);

		// Return the new position to be used by our camera (or player)
		return vNewPosition;
	}
}


/////////////////////////////////// TRACE RAY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes a start and end position (ray) to test against the BSP brushes
/////
/////////////////////////////////// TRACE RAY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 CQuake3BSP::TraceRay(CVector3 vStart, CVector3 vEnd)
{
	// We don't use this function, but we set it up to allow us to just check a
	// ray with the BSP tree brushes.  We do so by setting the trace type to TYPE_RAY.
	m_traceType = TYPE_RAY;

	// Run the normal Trace() function with our start and end 
	// position and return a new position
	return Trace(vStart, vEnd);
}


/////////////////////////////////// TRACE SPHERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This tests a sphere around our movement vector against the BSP brushes for collision
/////
/////////////////////////////////// TRACE SPHERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 CQuake3BSP::TraceSphere(CVector3 vStart, CVector3 vEnd, float radius)
{
	// In this tutorial we are doing sphere collision, so this is the function
	// that we will be doing to initiate our collision checks.

	// Here we initialize the type of trace (SPHERE) and initialize other data
	m_traceType = TYPE_SPHERE;
	m_bCollided = false;
	m_traceRadius = radius;

	// Get the new position that we will return to the camera or player
	CVector3 vNewPosition = Trace(vStart, vEnd);

	// Return the new position to be changed for the camera or player
	return vNewPosition;
}


/////////////////////////////////// CHECK NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This traverses the BSP to find the brushes closest to our position
/////
/////////////////////////////////// CHECK NODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::CheckNode(int nodeIndex, float startRatio, float endRatio, CVector3 vStart, CVector3 vEnd)
{
	// Remember, the nodeIndices are stored as negative numbers when we get to a leaf, so we 
	// check if the current node is a leaf, which holds brushes.  If the nodeIndex is negative,
	// the next index is a leaf (note the: nodeIndex + 1)
	if(nodeIndex < 0)
	{
		// If this node in the BSP is a leaf, we need to negate and add 1 to offset
		// the real node index into the m_pLeafs[] array.  You could also do [~nodeIndex].
		tBSPLeaf *pLeaf = &m_pLeafs[-(nodeIndex + 1)];

		// We have a leaf, so let's go through all of the brushes for that leaf
		for(int i = 0; i < pLeaf->numOfLeafBrushes; i++)
		{
			// Get the current brush that we going to check
			tBSPBrush *pBrush = &m_pBrushes[m_pLeafBrushes[pLeaf->leafBrush + i]];

			// This is kind of an important line.  First, we check if there is actually
			// and brush sides (which store indices to the normal and plane data for the brush).
			// If not, then go to the next one.  Otherwise, we also check to see if the brush
			// is something that we want to collide with.  For instance, there are brushes for
			// water, lava, bushes, misc. sprites, etc...  We don't want to collide with water
			// and other things like bushes, so we check the texture type to see if it's a solid.
			// If the textureType can be binary-anded (&) and still be 1, then it's solid,
			// otherwise it's something that can be walked through.  That's how Quake chose to
			// do it.

			// Check if we have brush sides and the current brush is solid and collidable
			if((pBrush->numOfBrushSides > 0) && (m_pTextures[pBrush->textureID].textureType & 1))
			{
				// Now we delve into the dark depths of the real calculations for collision.
				// We can now check the movement vector against our brush planes.
				CheckBrush(pBrush, vStart, vEnd);
			}
		}

		// Since we found the brushes, we can go back up and stop recursing at this level
		return;
	}

	// If we haven't found a leaf in the node, then we need to keep doing some dirty work
	// until we find the leafs which store the brush information for collision detection.

	// Grad the next node to work with and grab this node's plane data
	tBSPNode *pNode = &m_pNodes[nodeIndex];
	tBSPPlane *pPlane = &m_pPlanes[pNode->plane];
	
	// Now we do some quick tests to see which side we fall on of the node in the BSP

	// Here we use the plane equation to find out where our initial start position is
	// according the the node that we are checking.  We then grab the same info for the end pos.
	float startDistance = Dot(vStart, pPlane->vNormal) - pPlane->d;
	float endDistance = Dot(vEnd, pPlane->vNormal) - pPlane->d;
	float offset = 0.0f;

	// If we are doing any type of collision detection besides a ray, we need to change
	// the offset for which we are testing collision against the brushes.  If we are testing
	// a sphere against the brushes, we need to add the sphere's offset when we do the plane
	// equation for testing our movement vector (start and end position).  * More Info * For
	// more info on sphere collision, check out our tutorials on this subject.

	// If we are doing sphere collision, include an offset for our collision tests below
	if(m_traceType == TYPE_SPHERE)
		offset = m_traceRadius;

	// Below we just do a basic traversal down the BSP tree.  If the points are in
	// front of the current splitter plane, then only check the nodes in front of
	// that splitter plane.  Otherwise, if both are behind, check the nodes that are
	// behind the current splitter plane.  The next case is that the movement vector
	// is on both sides of the splitter plane, which makes it a bit more tricky because we now
	// need to check both the front and the back and split up the movement vector for both sides.

	// Here we check to see if the start and end point are both in front of the current node.
	// If so, we want to check all of the nodes in front of this current splitter plane.
	if(startDistance >= offset && endDistance >= offset)
	{
		// Traverse the BSP tree on all the nodes in front of this current splitter plane
		CheckNode(pNode->front, startDistance, endDistance, vStart, vEnd);
	}
	// If both points are behind the current splitter plane, traverse down the back nodes
	else if(startDistance < -offset && endDistance < -offset)
	{
		// Traverse the BSP tree on all the nodes in back of this current splitter plane
		CheckNode(pNode->back, startDistance, endDistance, vStart, vEnd);
	}	
	else
	{
		// If we get here, then our ray needs to be split in half to check the nodes
		// on both sides of the current splitter plane.  Thus we create 2 ratios.
		float Ratio1 = 1.0f, Ratio2 = 0.0f, middleRatio = 0.0f;
		CVector3 vMiddle;	// This stores the middle point for our split ray

		// Start of the side as the front side to check
		int side = pNode->front;

		// Here we check to see if the start point is in back of the plane (negative)
		if(startDistance < endDistance)
		{
			// Since the start position is in back, let's check the back nodes
			side = pNode->back;

			// Here we create 2 ratios that hold a distance from the start to the
			// extent closest to the start (take into account a sphere and epsilon).
			// We use epsilon like Quake does to compensate for float errors.  The second
			// ratio holds a distance from the other size of the extents on the other side
			// of the plane.  This essential splits the ray for both sides of the splitter plane.
			float inverseDistance = 1.0f / (startDistance - endDistance);
			Ratio1 = (startDistance - offset - kEpsilon) * inverseDistance;
			Ratio2 = (startDistance + offset + kEpsilon) * inverseDistance;
		}
		// Check if the starting point is greater than the end point (positive)
		else if(startDistance > endDistance)
		{
			// This means that we are going to recurse down the front nodes first.
			// We do the same thing as above and get 2 ratios for split ray.
			// Ratio 1 and 2 are switched in contrast to the last if statement.
			// This is because the start is starting in the front of the splitter plane.
			float inverseDistance = 1.0f / (startDistance - endDistance);
			Ratio1 = (startDistance + offset + kEpsilon) * inverseDistance;
			Ratio2 = (startDistance - offset - kEpsilon) * inverseDistance;
		}

		// Make sure that we have valid numbers and not some weird float problems.
		// This ensures that we have a value from 0 to 1 as a good ratio should be :)
		if (Ratio1 < 0.0f) Ratio1 = 0.0f;
        else if (Ratio1 > 1.0f) Ratio1 = 1.0f;

        if (Ratio2 < 0.0f) Ratio2 = 0.0f;
        else if (Ratio2 > 1.0f) Ratio2 = 1.0f;

		// Just like we do in the Trace() function, we find the desired middle
		// point on the ray, but instead of a point we get a middleRatio percentage.
		// This isn't the true middle point since we are using offset's and the epsilon value.
		// We also grab the middle point to go with the ratio.
		middleRatio = startRatio + ((endRatio - startRatio) * Ratio1);
		vMiddle = vStart + ((vEnd - vStart) * Ratio1);

		// Now we recurse on the current side with only the first half of the ray
		CheckNode(side, startRatio, middleRatio, vStart, vMiddle);

		// Now we need to make a middle point and ratio for the other side of the node
		middleRatio = startRatio + ((endRatio - startRatio) * Ratio2);
		vMiddle = vStart + ((vEnd - vStart) * Ratio2);

		// Depending on which side should go last, traverse the bsp with the
		// other side of the split ray (movement vector).
		if(side == pNode->back)
			CheckNode(pNode->front, middleRatio, endRatio, vMiddle, vEnd);
		else
			CheckNode(pNode->back, middleRatio, endRatio, vMiddle, vEnd);
	}
}


/////////////////////////////////// CHECK BRUSH \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This checks our movement vector against all the planes of the brush
/////
/////////////////////////////////// CHECK BRUSH \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::CheckBrush(tBSPBrush *pBrush, CVector3 vStart, CVector3 vEnd)
{
	float startRatio = -1.0f;		// Like in BrushCollision.htm, start a ratio at -1
    float endRatio = 1.0f;			// Set the end ratio to 1
    bool startsOut = false;			// This tells us if we starting outside the brush

	// This function actually does the collision detection between our movement
	// vector and the brushes in the world data.  We will go through all of the
	// brush sides and check our start and end ratio against the planes to see if
	// they pass each other.  We start the startRatio at -1 and the endRatio at
	// 1, but as we set the ratios to their intersection points (ratios), then
	// they slowly move toward each other.  If they pass each other, then there
	// is definitely not a collision.

	// Go through all of the brush sides and check collision against each plane
	for(int i = 0; i < pBrush->numOfBrushSides; i++)
	{
		// Here we grab the current brush side and plane in this brush
		tBSPBrushSide *pBrushSide = &m_pBrushSides[pBrush->brushSide + i];
		tBSPPlane *pPlane = &m_pPlanes[pBrushSide->plane];

		// Let's store a variable for the offset (like for sphere collision)
		float offset = 0.0f;

		// If we are testing sphere collision we need to add the sphere radius
		if(m_traceType == TYPE_SPHERE)
			offset = m_traceRadius;

		// Test the start and end points against the current plane of the brush side.
		// Notice that we add an offset to the distance from the origin, which makes
		// our sphere collision work.
		float startDistance = Dot(vStart, pPlane->vNormal) - (pPlane->d + offset);
		float endDistance = Dot(vEnd, pPlane->vNormal) - (pPlane->d + offset);

		// Make sure we start outside of the brush's volume
		if(startDistance > 0)	startsOut = true;

		// Stop checking since both the start and end position are in front of the plane
		if(startDistance > 0 && endDistance > 0)
			return;

		// Continue on to the next brush side if both points are behind or on the plane
		if(startDistance <= 0 && endDistance <= 0)
			continue;

		// If the distance of the start point is greater than the end point, we have a collision!
		if(startDistance > endDistance)
		{
			// This gets a ratio from our starting point to the approximate collision spot
			float Ratio1 = (startDistance - kEpsilon) / (startDistance - endDistance);

			// If this is the first time coming here, then this will always be true,
			// since startRatio starts at -1.0f.  We want to find the closest collision,
			// so we still continue to check all of the brushes before quitting.
			if(Ratio1 > startRatio)
			{
				// Set the startRatio (currently the closest collision distance from start)
				startRatio = Ratio1;
				m_bCollided = true;		// Let us know we collided!
			}
		}
		else
		{
			// Get the ratio of the current brush side for the endRatio
			float Ratio = (startDistance + kEpsilon) / (startDistance - endDistance);

			// If the ratio is less than the current endRatio, assign a new endRatio.
			// This will usually always be true when starting out.
			if(Ratio < endRatio)
				endRatio = Ratio;
		}
	}

	// If we didn't start outside of the brush we don't want to count this collision - return;
	if(startsOut == false)
	{
		return;
	}
	
	// If our startRatio is less than the endRatio there was a collision!!!
	if(startRatio < endRatio)
	{
		// Make sure the startRatio moved from the start and check if the collision
		// ratio we just got is less than the current ratio stored in m_traceRatio.
		// We want the closest collision to our original starting position.
		if(startRatio > -1 && startRatio < m_traceRatio)
		{
			// If the startRatio is less than 0, just set it to 0
			if(startRatio < 0)
				startRatio = 0;

			// Store the new ratio in our member variable for later
			m_traceRatio = startRatio;
		}
	}
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


//////////////////////////// RENDER FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This renders a face, determined by the passed in index
/////
//////////////////////////// RENDER FACE \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::RenderFace(int faceIndex)
{
// Here we grab the face from the index passed in
	tBSPFace *pFace = &m_pFaces[faceIndex];

	// Assign our array of face vertices for our vertex arrays and enable vertex arrays
	glVertexPointer(3, GL_FLOAT, sizeof(tBSPVertex), &(m_pVerts[pFace->startVertIndex].vPosition));
	glEnableClientState(GL_VERTEX_ARRAY);

	// If we want to render the textures
	if(g_bTextures)
	{
		// Set the current pass as the first texture (For multi-texturing)
		glActiveTextureARB(GL_TEXTURE0_ARB);

		// Give OpenGL the texture coordinates for the first texture, and enable that texture
		glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(tBSPVertex), 
									   &(m_pVerts[pFace->startVertIndex].vTextureCoord));

		// Set our vertex array client states for allowing texture coordinates
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Turn on texture arrays for the first pass
		glClientActiveTextureARB(GL_TEXTURE0_ARB);

		// Turn on texture mapping and bind the face's texture map
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,  m_textures[pFace->textureID]);
	}

	if(g_bLightmaps)
	{
		// Set the current pass as the second lightmap texture_
		glActiveTextureARB(GL_TEXTURE1_ARB);

		// Turn on texture arrays for the second lightmap pass
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		// Next, we need to specify the UV coordinates for our lightmaps.  This is done
		// by switching to the second texture and giving OpenGL our lightmap array.
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(tBSPVertex), 
									   &(m_pVerts[pFace->startVertIndex].vLightmapCoord));

		// Turn on texture mapping and bind the face's lightmap over the texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,  m_lightmaps[pFace->lightmapID]);
	}

	// Render our current face to the screen with vertex arrays
	glDrawElements(GL_TRIANGLES, pFace->numOfIndices, GL_UNSIGNED_INT, &(m_pIndices[pFace->startIndex]) );
}


//////////////////////////// RENDER LEVEL \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Goes through all of the faces and draws them if the type is FACE_POLYGON
/////
//////////////////////////// RENDER LEVEL \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::RenderLevel(const CVector3 &vPos)
{
	// Reset our bitset so all the slots are zero.
	m_FacesDrawn.ClearAll();

	// Grab the leaf index that our camera is in
	int leafIndex = FindLeaf(vPos);

	// Grab the cluster that is assigned to the leaf
	int cluster = m_pLeafs[leafIndex].cluster;

	// Initialize our counter variables (start at the last leaf and work down)
	int i = m_numOfLeafs;
	g_VisibleFaces = 0;

	// Go through all the leafs and check their visibility
	while(i--)
	{
		// Get the current leaf that is to be tested for visibility from our camera's leaf
		tBSPLeaf *pLeaf = &(m_pLeafs[i]);

		// If the current leaf can't be seen from our cluster, go to the next leaf
		if(!IsClusterVisible(cluster, pLeaf->cluster)) 
			continue;

		// If the current leaf is not in the camera's frustum, go to the next leaf
		if(!g_Frustum.BoxInFrustum((float)pLeaf->min.x, (float)pLeaf->min.y, (float)pLeaf->min.z,
		  	 				       (float)pLeaf->max.x, (float)pLeaf->max.y, (float)pLeaf->max.z))
			continue;
		
		// If we get here, the leaf we are testing must be visible in our camera's view.
		// Get the number of faces that this leaf is in charge of.
		int faceCount = pLeaf->numOfLeafFaces;

		// Loop through and render all of the faces in this leaf
		while(faceCount--)
		{
			// Grab the current face index from our leaf faces array
			int faceIndex = m_pLeafFaces[pLeaf->leafface + faceCount];

			// Before drawing this face, make sure it's a normal polygon
			if(m_pFaces[faceIndex].type != FACE_POLYGON) continue;

			// Since many faces are duplicated in other leafs, we need to
			// make sure this face already hasn't been drawn.
			if(!m_FacesDrawn.On(faceIndex)) 
			{
				// Increase the rendered face count to display for fun
				g_VisibleFaces++;

				// Set this face as drawn and render it
				m_FacesDrawn.Set(faceIndex);
				RenderFace(faceIndex);
			}
		}			
	}
}


//////////////////////////// DESTROY \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This cleans up our object and frees allocated memory
/////
//////////////////////////// DESTROY \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuake3BSP::Destroy()
{
	// If we still have valid memory for our vertices, free them
	if(m_pVerts) 
	{
		delete [] m_pVerts;		m_pVerts = NULL;
	}

	// If we still have valid memory for our faces, free them
	if(m_pFaces)	
	{
		delete [] m_pFaces;		m_pFaces = NULL;
	}
	
	// If we still have valid memory for our indices, free them
	if(m_pIndices)	
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
	}

	// If we still have valid memory for our nodes, free them
	if(m_pNodes)	
	{
		delete [] m_pNodes;		m_pNodes = NULL;
	}

	// If we still have valid memory for our leafs, free them
	if(m_pLeafs)	
	{
		delete [] m_pLeafs;		m_pLeafs = NULL;
	}

	// If we still have valid memory for our leaf faces, free them
	if(m_pLeafFaces)	
	{
		delete [] m_pLeafFaces;	m_pLeafFaces = NULL;
	}

	// If we still have valid memory for our planes, free them
	if(m_pPlanes)	
	{
		delete [] m_pPlanes;	m_pPlanes = NULL;
	}

	// If we still have valid memory for our clusters, free them
	if(m_clusters.pBitsets)	
	{
		delete [] m_clusters.pBitsets;		m_clusters.pBitsets = NULL;
	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// If we still have valid memory for our brushes, free them
	if(m_pBrushes)	
	{
		delete [] m_pBrushes;		m_pBrushes = NULL;
	}
	
	// If we still have valid memory for our brush sides, free them
	if(m_pBrushSides)	
	{
		delete [] m_pBrushSides;	m_pBrushSides = NULL;
	}

	// If we still have valid memory for our leaf brushes, free them
	if(m_pLeafBrushes)	
	{
		delete [] m_pLeafBrushes;	m_pLeafBrushes = NULL;
	}

	// If we still have valid memory for our BSP texture info, free it
	if(m_pTextures)	
	{
		delete [] m_pTextures;		m_pTextures = NULL;
	}	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Free all the textures
	glDeleteTextures(m_numOfTextures, m_textures);

	// Delete the lightmap textures
	glDeleteTextures(m_numOfLightmaps, m_lightmaps);
}


//////////////////////////// ~CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is our deconstructor that is called when the object is destroyed
/////
//////////////////////////// ~CQUAKE3BSP \\\\\\\\\\\\\\\\\\\\\\\\\\\*

CQuake3BSP::~CQuake3BSP()
{
	// Call our destroy function
	Destroy();
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We did it!  Hopefully this wasn't too overwhelming for you.  Now that we got
// the base done, the rest of the tutorials in this series should be a piece of
// cake.  I'd even be so bold as to say carrot cake!
//
// Be sure to notice that we changed the pTextures to m_pTextures in our load
// function.  This is so we can save the texture information in our class for
// knowing what kind of a brush we are colliding with.  We also added the code
// to free that information in our DeInit() function.  You'll also notice new
// lines of code in our constructor (init function) to initialize the new variables.
// 
// The hardest part about this tutorial in my mind is wrapping your head around
// the splitting part in CheckNode().  We need to split up our movement vector (ray)
// if the start and end point lie on a different side of a plane.
//
// In this tutorial all we do is just run into the wall and stop.  The next tutorial
// will focus on sliding, as well as AABB collision checks.
//
// For more information on collision with Quake3 you can check out:
//
// http://www.nathanostgard.com/tutorials/quake3/collision/
//
// I want to thanks Nathan Ostgard, Frank Puig Placeres (fpuig2003@yahoo.com) and
// Gabriel Gambetta (Gabriel@MysteryStudio.com) for their input and guidance on
// the BSP collision.  You can contact them to answer any further questions you might have.
//
// Good luck on your projects!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
