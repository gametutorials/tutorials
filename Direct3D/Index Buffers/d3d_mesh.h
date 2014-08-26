#ifndef D3D_MESH_H
#define D3D_MESH_H

#include "d3d_obj.h"

/*
	This is our mesh class.  A mesh is a collection of triangle data and other 
	properties unique to the triangle data.  Typically a model, lets say a human
	character in a game, is made up of several meshes.  For instance, our human could
	be comprised of a legs mesh, torso mesh and head mesh.

	The following mesh class is very simple.  It contains a vertex buffer and an
	index buffer which defines the triangle data in the vertex buffer.  This just
	scratches the surface at the complexity of a modern game mesh object.
*/

class CD3DMesh
{
	public:
	
		// Constructor -- Zero out the member variables
		CD3DMesh()
		{
			zeroVars();
		}

		// Render the mesh.  We make the function a "const" function because
		// we promise to not change any of the mesh's data while rendering
		void render() const;

		// This is what sets our mesh's internal vertex buffer
		// We return true on success, false otherwise 
		bool setVertexData(void *verts, int vertCount);
		
		// This is what sets our mesh's internal index buffer
		// We return true on success, false otherwise
		bool setIndexData(WORD *indices, int indexCount);
		
		// Free up the mesh's data
		~CD3DMesh()
		{
			freeMem();
		}			 
		
	private:
	
		IDirect3DVertexBuffer9 *mVertexBuffer; // The D3D vertex buffer interface
		IDirect3DIndexBuffer9 *mIndexBuffer; // The D3D index buffer interface
		
		int mVertCount; // Number of vertices in the vertex buffer
		int mTriCount; // Number of triangles to draw
			
		void freeMem(); // Free all memory in the CD3DMesh
	
		// **NOTE** We separate the freeing of the vertex data and index data into two
		//			different functions because we want it possible to set the vertex data
		//			once and change the index data multiple times, and vice versa
		void freeVertexData(); // Free all of the vertex data
		void freeIndexData(); // Free all the index data
		
		void zeroVars(); // Zeros out all data inside CD3DMesh
		
};

#endif
