#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

/*

This file is all about defining our ONE vertex type for this tutorial.  We know we need
vertices to make up all the objects that we'll want in our 3D world.  This includes
everything from characters, to the set, the particles, to the HUD.  However, it would
be a real drag if all the vertices had to be of the same type.  I mean does everything
that goes into a character need to go into a HUD power bar?  Of course not.  Different
models will have different vertex needs and this is where the power of FVF shines.

Another advantage to using FVF is that by limiting the data in certain vertices you
decrease memory usage and minimize the processing bandwidth required to render models.  And
whenever you can reduce memory and gain speed, you do it!

Okay so now we are staring to understand that FVF (Flexible Vertex Format for all that forgot)
is important to use, but what exactly is it and how do we use it?

Well D3D has a predefined way all vertices must be laid out.  This is shown at the bottom
of this file.  If you peek and look at the "chart" you'll see that you can, if you wanted,
include a butt-load of data with each vertex.  Having all of these things present would
certainly slow down the app.  So by making D3DFVF codes we define our own vertices.

To define our own vertex is not too difficult.  First we need to make a struct that
contains what we want our vertices to have.  We MUST layout the data in the struct 
in the EXACT SAME ORDER it is expected to be laid out (as specified at the bottom of this file).
Now we can have components missing from our vertex definition but we must match up order wise
with the way D3D expects it from top to bottom.  The second part is making a D3DFVF "flag" 
that says, "Hey our vertices include this data".  D3D has a list of flags for each possible
component (for instance the diffuse color) that can be stored in a vertex.  We'll create our 
flag for our vertices by ORing together each component flag that we use to create our
vertex.  Don't worry if it sounds a little confusing, it is confusing %).  But take a
long look at the code below and hopefully that will clear things up.
*/

// Here is our D3DFVF define (flag).  Notice we combine two separate flags to make
// our new flag.  Here's what each individual flag means:
// D3DFVF_XYZRHW -- The vertex format includes the position of a transformed vertex.
//					**NOTE** A transformed vertex is one who's position is already in screen space
//							 that means you can think of the (x,y) in normal Win32 POINT 
//							 coordinates
// D3DFVF_DIFFUSE -- The vertex format includes a diffuse color component 
#define SVertexType D3DFVF_XYZRHW | D3DFVF_DIFFUSE

// Here's our vertex struct
struct SVertex
{
	float x, y, z, w; // Our position in space.  Since this is a 3D application one would
					 // expect an (x,y,z) for position so what gives with the 'w'?  Well 
					// generally we do store our position in 3D space, but this tutorial
				   // stores the positions for the triangle it draws in screen space (the
				  // same coordinate system the window uses).  This way we can very easily draw
				 // something to the screen.  The 'w' servers as a flag to say that we are in
				// screen space, not 3D space and is set to 1.0f
	DWORD color; // Our diffuse color
};

// FVF Vertex Order
// This is the grand master order that all D3D Vertices must adhere to or else %)

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
