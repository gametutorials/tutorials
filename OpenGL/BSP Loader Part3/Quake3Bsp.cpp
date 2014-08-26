//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader 3									 //
//																		 //
//		$Description:	Loads bsp tree data and culls portals and leafs	 //
//																		 //
//***********************************************************************//

#include "Main.h"
#include "Camera.h"
#include "Quake3Bsp.h"
#include "Frustum.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// In this version of our tutorial we added the ability to only draw what's in the
// camera's frustum.  This is done by PVSs (Potential Visibility Sets) and frustum
// culling.  We needed to read in the BSP tree data from the file to help us out
// with this.  The rendering loop now goes through every leaf, checks if it's
// visible from the current cluster, then checks if it's inside of the frustum.
// Both of these checks need to happen, not just the frustum culling, or else
// face data behind walls will get drawn when it isn't needed.  It's better to
// make the extra checks that it is to draw more polygons, from a high level that
// is...
//
//
// Be sure to look at the HTML file format that was included with this tutorial.
// It has the whole file format and added information not discussed in this tutorial.
//
//


// This is our global frustum class, which is used to cull BSP leafs
extern CFrustum g_Frustum;

// This will store how many faces are drawn and are seen by the camera
extern int g_VisibleFaces;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	m_numOfVerts     = 0;	
	m_numOfFaces     = 0;
	m_numOfIndices   = 0;
	m_numOfTextures  = 0;
	m_numOfLightmaps = 0;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	m_numOfNodes	 = 0;
	m_numOfLeafs	 = 0;
	m_numOfLeafFaces = 0;
	m_numOfPlanes	 = 0;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Initialize all the dynamic BSP data pointers to NULL
	m_pVerts		 = NULL;	
	m_pFaces		 = NULL;
	m_pIndices       = NULL;
	

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	m_pNodes		 = NULL;
	m_pLeafs		 = NULL;
	m_pPlanes		 = NULL;
	m_pLeafFaces	 = NULL;
	memset(&m_clusters, 0, sizeof(tBSPVisData));

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

	// Allocate memory to read in the texture information.
	m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
	tBSPTexture *pTextures = new tBSPTexture [m_numOfTextures];

	// Allocate memory to read in the lightmap data.  
	m_numOfLightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap *pLightmaps = new tBSPLightmap [m_numOfLightmaps ];

	// Seek to the position in the file that stores the vertex information
	fseek(fp, lumps[kVertices].offset, SEEK_SET);

	// Go through all of the vertices that need to be read and swap axises
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
	
	// Read in all the texture information
	fread(pTextures, m_numOfTextures, sizeof(tBSPTexture), fp);

	// Go through all of the textures
	for(i = 0; i < m_numOfTextures; i++)
	{
		// Find the extension if any and append it to the file name
		FindTextureExtension(pTextures[i].strName);
		
		// Create a texture from the image
		CreateTexture(m_textures[i], pTextures[i].strName);
	}

	// We can now free all the texture information since we already loaded them
	delete [] pTextures;

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// In this function we read from a bunch of new lumps.  These include
	// the BSP nodes, the leafs, the leaf faces, BSP splitter planes and
	// visibility data (clusters).

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


	// Close the file
	fclose(fp);

	// Here we allocate enough bits to store all the faces for our bitset
	m_FacesDrawn.Resize(m_numOfFaces);

	// Return a success
	return true;
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

//////////////////////////// FIND LEAF \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the leaf our camera is in
/////
//////////////////////////// FIND LEAF \\\\\\\\\\\\\\\\\\\\\\\\\\\*

int CQuake3BSP::FindLeaf(const CVector3 &vPos)
{
	int i = 0;
	float distance = 0.0f;

	// This function takes in our camera position, then goes and walks
	// through the BSP nodes, starting at the root node, finding the leaf node
	// that our camera resides in.  This is done by checking to see if
	// the camera is in front or back of each node's splitting plane.
	// If the camera is in front of the camera, then we want to check
	// the node in front of the node just tested.  If the camera is behind
	// the current node, we check that nodes back node.  Eventually, this
	// will find where the camera is according to the BSP tree.  Once a
	// node index (i) is found to be a negative number, that tells us that
	// that index is a leaf node, not another BSP node.  We can either calculate
	// the leaf node index from -(i + 1) or ~1.  This is because the starting
	// leaf index is 0, and you can't have a negative 0.  It's important
	// for us to know which leaf our camera is in so that we know which cluster
	// we are in.  That way we can test if other clusters are seen from our cluster.
	
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
	// This function is used to test the "current" cluster against
	// the "test" cluster.  If the "test" cluster is seen from the
	// "current" cluster, we can then draw it's associated faces, assuming
	// they aren't frustum culled of course.  Each cluster has their own
	// bitset containing a bit for each other cluster.  For instance, if there
	// is 10 clusters in the whole level (a tiny level), then each cluster
	// would have a bitset of 10 bits that store a 1 (visible) or a 0 (not visible) 
	// for each other cluster.  Bitsets are used because it's faster and saves
	// memory, instead of creating a huge array of booleans.  It seems that
	// people tend to call the bitsets "vectors", so keep that in mind too.

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// In this new revision of RenderLevel(), we do things a bit differently.
	// Instead of looping through all the faces, we now want to loop through
	// all of the leafs.  Each leaf stores a list of faces assign to it.
	// We call FindLeaf() to find the current leaf index that our camera is
	// in.  This leaf will then give us the cluster that the camera is in.  The
	// cluster is then used to test visibility between our current cluster
	// and other leaf clusters.  If another leaf's cluster is visible from our 
	// current cluster, the leaf's bounding box is checked against our frustum.  
	// Assuming the bounding box is inside of our frustum, we draw all the faces
	// stored in that leaf.  

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
// We added the code to load and walk the BSP tree data, along with the PVS
// checks for every cluster.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//
