#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

// The define for our vertex type
// What is this crazy line "(1 << D3DFVF_TEXCOUNT_SHIFT)" about?
// The D3D defines for having texture coordinates in your 
// flexible vertex format (FVF) are D3DFVF_TEX0 - D3DFVF_TEX8.  However,
// one might logically think that D3DFVF_TEX0 means "One set of texture coordinates"
// and D3D_TEX1 means "Two set of texture coordinates" similar to the
// zero based counting used to index into an array.  But this thinking is
// not correct.  The D3D defines are not necessarily in a sequential order.  So to
// set the number of UVs (texture coordinates) in a FVF (for our case, we have
// one set of UVs) you shift the number of UVs the FVF contains by the D3D define
// D3DFVF_TEXCOUNT_SHIFT.  The resulting number will be the correct flag
// to specify how many sets of texture coordinates are in the FVF.
#define SVertexType D3DFVF_XYZ | (1 << D3DFVF_TEXCOUNT_SHIFT)

// Our vertex struct  
struct SVertex
{
	float x, y, z; // Our position in space (world coordinates)
	float u, v; // Our texture coordinates
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
