#ifndef DEFINES_H
#define DEFINES_H

#include <d3d9.h>
#include "vector.h"

// An RGB color 
// (0,0,0) == Black
// (1,1,1) == White
class CColor
{
	public:
	
		CColor() { /* do nothing */ }
		CColor(float R, float G, float B) : r(R), g(G), b(B) { /* do nothing */ }
		
		// Public data
		float r, g, b;
};

class CTexCoord
{
	public:

		CTexCoord() : u(0.0f), v(0.0f) { /* do nothing */ }
		CTexCoord(float U, float V) : u(U), v(V) { /* do nothing */ }

		CTexCoord operator -(const CTexCoord &tc) const
		{
			return CTexCoord(u - tc.u, v - tc.v);
		}

		float u;
		float v;
};

struct SPosUV
{
	float x, y, z;
	float u, v;

	// Flexible vertex format code for this vertex combination
	static const DWORD kFVF = D3DFVF_XYZ | D3DFVF_TEX1;
};

struct SPosColor
{
	float x, y, z;
	DWORD color;
	
	// Flexible vertex format code for this vertex combination
	static const DWORD kFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct SPosNormColor
{
	float x, y, z;
	float nx, ny, nz;
	DWORD color;

	// Flexible vertex format code for this vertex combination
	static const DWORD kFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE;
};

// Our vertex struct for normal mapping 
struct SVertex
{
	CVector pos;

	CVector tangent;
	CVector binormal;
	CVector normal;

	CTexCoord tc;
};

// General mesh struct
struct SMesh
{
	IDirect3DVertexDeclaration9 *vertexDeclaration;
	IDirect3DVertexBuffer9 *vertexBuffer;
	IDirect3DIndexBuffer9 *indexBuffer;
	
	int vertexCount;
	int indexCount;
};

#endif