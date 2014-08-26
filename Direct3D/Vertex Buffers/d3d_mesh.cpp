#include "d3d_mesh.h"	

// Render the mesh
void CD3DMesh::render() const
{
	HRESULT result;
	
	// Set the vertex data
	// By paramater:
	// 0 -- The stream number of the data.  D3D allows for multiple streams
	//		of data to be used in a particular render.  The streams use
	//		zero based counting and since we only have (which is typically the case)
	//		we pass zero
	// mVertexBuffer -- The IDirect3DVertexBuffer9* to use for rendering
	// 0 -- Offset in bytes, to the beginning of where to start rendering in
	//		the vertex buffer.  We want to begin rendering from the start of the 
	//		the vertex buffer so we pass zero
	// sizeof(SVertex) -- The size, in bytes, of an individual vertex stored 
	//					  in the vertex buffer
	result = CD3DObj::mDevice->SetStreamSource(0, mVertexBuffer, 0, sizeof(SVertex));
	assert(result == D3D_OK);

	// Specify vertex type
	// SVertexType -- Is the #define which defines our vertex in "vertex_types.h"
	CD3DObj::mDevice->SetFVF(SVertexType);

	// Draw the geometry
	// By paramater:
	// D3DPT_TRIANGLELIST -- The primitive to use when rendering the vertex buffer
	//						 Our mesh always assumes triangles
	// 0 -- The index of the first vertex to use when rendering.  We want
	//		to begin rendering with the first vertex, so we pass zero
	// mTriCount -- The number of triangles to render
	result = CD3DObj::mDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,mTriCount);
	assert(result == D3D_OK);
}

// Set the vertex data
bool CD3DMesh::setVertexData(void *verts, int vertCount)
{
	// Safety first
	if(!verts || vertCount <= 0)
		return false;
		
	// If we're setting new data, make sure we free up anything
	// already allocated
	freeMem();
	
	// Calculate and store the number of triangles to be rendered
	mTriCount = vertCount / 3;
	
	// Calculate vertex buffer size in bytes
	int vbSizeInBytes = sizeof(SVertex) * vertCount; 

	// Create vertex buffer -- This the function to call to create a vertex buffer
	// Lets break down each parameter of the function:
	// vbSizeInBytes -- The total number of bytes in the vertex buffer
	// 0 -- This a D3DUSAGE flag.  An example of usage would be if we knew we going
	//		to use this buffer for point sprites (a special type of D3D primitive) we
	//		could tag it as such by using the D3DUSAGE_POINTS flag.  However we have
	//		no special usage planned so we pass 0
	// SVertexType -- The flexible vertex flag (FVF) which defines the vertex data
	// D3DPOOL_MANAGED -- The type of memory allocation for this vertex buffer.  By
	//					  passing the D3DPOOL_MANAGED flag, we are letting D3D handle the
	//					  memory management, so D3D will be responsible for copying our
	//					  vertex buffer to the graphics card when it's needed for rendering
	//					  An additional benefit to using this flag, is that if we call
	//					  the IDirect3DDevice9 method Reset() for toggling between windowed
	//					  mode and full screen we will not have to recreate the vertex buffer
	// &mVertexBuffer -- The address of the IDirect3DVertexBuffer9 to be created
	// NULL -- A lovely Microsoft reserved parameter.  It must equal NULL.				    
	if(CD3DObj::mDevice->CreateVertexBuffer(vbSizeInBytes, 0, SVertexType, D3DPOOL_MANAGED,
											&mVertexBuffer, NULL) != D3D_OK)
	{
		return false;
	}
	
	// Now that we have a created vertex buffer, we need to fill it with 
	// the vertex data.  

	void *v = NULL; // This is a pointer we will use to lock the
				   // the vertex data associated with our "mVertexBuffer"  

	// Lock the vertex data
	// By parameter:
	// 0 -- Offset in bytes to lock in the vertex buffer.  By passing zero
	//		we say we want to begin locking at the start of the buffer
	// 0 -- Number of bytes to lock in the vertex buffer.  By passing zero
	//		we say we want to lock the entire buffer
	// &v -- Address of the pointer which will point to the memory buffer of
	//		 the D3D vertex buffer's data upon returning from the function.
	//		 So "v" will point to the IDirect3DVertexBuffer9's vertex data
	//		 that we can then fill in 
	// 0 --  This a D3DLOCK flag, that if we wanted, we could specify special
	//		 locking functionality.  We don't have any special way we want
	//		 to lock the data, so we pass zero
	if(mVertexBuffer->Lock(0, 0, &v, 0) != D3D_OK)
		return false;
	
	// Now that we have a pointer to our vertex buffer, we
	// copy over the data
	memcpy(v, verts, vbSizeInBytes);

	// Once we're finished copying over the data
	// we can unlock our buffer and have it ready to be rendered
	if(mVertexBuffer->Unlock() != D3D_OK) 
		return false;
	
	return true; // Setting the vertex data was successful
}

// Free memory
void CD3DMesh::freeMem()
{
	if(mVertexBuffer) // Free the vertex buffer if allocated
		mVertexBuffer->Release();
		
	zeroVars();
}

// Zero out the variable members of the class
void CD3DMesh::zeroVars()
{
	mVertexBuffer = NULL;
	mTriCount = 0;
}
