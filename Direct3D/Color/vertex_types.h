#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

/*
	Below we define our vertex type will use for this tutorial.  At the bottom of this file
	is a refresher chart of the "master layout" a D3D vertex must adhere to (admit it, you
	forgot already :)
*/

// Here is our D3DFVF define (flag).  Our vertex type contains a transformed (x,y,z,w) position
// (that means the coordinates are already in screen space, just like 2D) and a color
#define SVertexType D3DFVF_XYZRHW | D3DFVF_DIFFUSE

// Our vertex struct.  
struct SVertex
{
	float x, y, z, w; // Our position in space.  Since this is a 3D application one would
					 // expect an (x,y,z) for position so what gives with the 'w'?  Well 
					// generally we do store our position in 3D space, but this tutorial
				   // stores the positions for the triangle it draws in screen space (the
				  // same coordinate system the window uses).  This way we can very easily draw
				 // something to the screen.  The 'w' servers as a flag to say that we are in
				// screen space, not 3D space and is set to 1.0f
	DWORD color; // Our diffuse color (also referred to as the vertex color)
};

// FVF Vertex Order
// This is the grand master order that all D3D Vertices must adhere to

/*
	float x, y, z;	// Position (untransformed or transformed)
	float w; // The reciprocal homogeneous component

	float blend1, blend2, blend3; // Blending weight data

	float vnX, vnY, vnZ; // The vertex normal for untransformed vertices only

	float ptSize; // The vertex point size

	DWORD diffuseColor; // The diffuse color in ARGB
	DWORD specularColor; // The specular color in ARGB

	float u1, v1; // Texture coordinates (Set 1)
	float u2, v2; // Texture coordinates (Set 2)
	float u3, v3; // Texture coordinates (Set 3)
	float u4, v4; // Texture coordinates (Set 4)
	float u5, v5; // Texture coordinates (Set 5)
	float u6, v6; // Texture coordinates (Set 6)
	float u7, v7; // Texture coordinates (Set 7)
	float u8, v8; // Texture coordinates (Set 8)
						 
*/

#endif
