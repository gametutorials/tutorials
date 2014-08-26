#ifndef D3D_MESH_H
#define D3D_MESH_H

#include "d3d_obj.h"
#include "d3d_texture.h"

////////////////
// Constants //
//////////////


/*
	Below is the definition for our mesh class.  It use the concept of streams instead of 
	a packed vertex.  Lets illustrate that:
	
		////////////////////
		// Packed Vertex //
		//////////////////
		
		SVeretx
		{
			float x, y, z;					
			DWORD color;					
		}									
		
		SVertex verts[4];
		
		**NOTE** The vertex information is packed in a struct and you then have an
				 array of those structs 
		
		******************************************************************************
		******************************************************************************
		******************************************************************************
		
		/////////////////////
		// Vertex Streams //
		///////////////////
		
		struct SPos
		{
			float x, y, z;
		};
		
		SPos pos[4];
		DWORD color[4];	
		
		**NOTE** Instead of the vertex information being packed in a struct, each
				 piece is in it's own separate array (stream).  By having everything
				 separate, we can easily add or subtract data.  Also, if are hardware
				 allows, we can take advantage of parallel processing which will give
				 us a performance boost.	
*/

class CD3DMesh
{
	public:
	
		// Constructor -- Zero out the member variables
		CD3DMesh()
		{
			zeroVars();
		}

		// Render the mesh.  If "useDiffuse" is true, the diffuse color will be used when rendering
		// if set.  If "useTexture" is true, the texture will be applied if set.
		void render(bool useDiffuse, bool useTexture);
		
		// Loads the texture "fileName" into the mesh
		// Returns true for success, false otherwise
		bool loadTexture(const char *fileName);

		// Sets the meshes 3D positions.  This must be set for the 
		// mesh's render() to work.  Returns true on success, false otherwise 
		bool setPosition(const CPos *pos, int count);
		
		// Sets the mesh's diffuse color (also called vertex color).  If setting
		// diffuse colors for the mesh, you MUST set the same number of colors
		// as there are (x,y,z) positions in the mesh.  Returns true for success
		// false otherwise.
		bool setDiffuseColor(const DWORD *colors, int count);
		
		// Sets the mesh's texture coordinates.  If setting texture coordinates for the mesh, 
		// you MUST set the same number of texure coordiantes as there are (x,y,z) positions
		// in the mesh.  Returns true for success false otherwise.
		bool setTexCoords(const CTexCoord *tc, int count);
		
		// This is what sets our mesh's internal index buffer
		// We return true on success, false otherwise
		bool setIndexBuffer(const WORD *indices, int indexCount);
		
		void freeVertexData(); // Free all of the vertex data
		void freeIndexBuffer(); // Free the index buffer
		
		// Free up the mesh's data
		~CD3DMesh()
		{
			freeMem();
		}			 
		
	private:
		
		// Even when using streams in D3D land, you need to use the Direct3D native vertex
		// buffer format.  However, instead of having all you data in one vertex buffer, each
		// piece of data gets it's own unique vertex buffer
		IDirect3DVertexBuffer9 *mPosVB; // D3D vertex buffer for position (x,y,z)
		IDirect3DVertexBuffer9 *mColorVB; // D3D vertex buffer for color (DWORD in (A,R,G,B) format)
		IDirect3DVertexBuffer9 *mTexCoordVB; // D3D vertex buffer for texture coordinates (u,v)
		
		IDirect3DIndexBuffer9 *mIndexBuffer; // The D3D index buffer interface
		
		// Now we are NOT using FVF flags to tell how the vertices are laid out.  So how
		// does D3D know?  Simple, you have to fill out a vertex declaration that declares
		// how the vertex data is laid out.  But now you are not bound by any of the rules
		// used of FVF.	
		IDirect3DVertexDeclaration9 *mVertexDeclaration; // Declares format of vertex data 
		
		CD3DTexture *mTexture; // Our D3D texture class for holding the texture
		
		int mVertCount; // Number of vertices in the vertex buffer
		int mTriCount; // Number of triangles to draw
		
		// A mesh doesn't know what data will be set in it.  This function creates the
		// IDirect3DVertexDeclaration9 based on the data set in the mesh. 				
		bool setupVertexDeclaration(bool useDiffuse, bool useTexture);
		
		// Completely frees up the mesh
		void freeMem();
			
		// Zeros out CD3DMesh's variables 
		void zeroVars()
		{
			mPosVB = mColorVB = mTexCoordVB = NULL;
			mIndexBuffer = NULL;
			mVertexDeclaration = NULL;
			mTexture = NULL;
			
			mVertCount = mTriCount = 0;
		}
};

#endif
