#ifndef D3D_MESH_H
#define D3D_MESH_H

#include "d3d_obj.h"

/*
	This is our mesh class.  A mesh is a collection of triangle data and other 
	properties unique to the triangle data.  Typically a model, lets say a human
	character in a game, is made up of several meshes.  For instance, our human could
	be comprised of a legs mesh, torso mesh and head mesh.
	
	The following mesh class is very simple and just scratches the surface at the
	complexity of a modern game mesh object.
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
		
		// Deconstructor -- Free the mesh
		~CD3DMesh()
		{
			freeMem();
		}			 
		
	private:
	
		IDirect3DVertexBuffer9 *mVertexBuffer; // The D3D vertex buffer interface
		int mTriCount; // Number of triangles to draw
			
		void freeMem(); // Free all memory in the CD3DMesh
		void zeroVars(); // Zeros out all data inside CD3DMesh
		
};

#endif
