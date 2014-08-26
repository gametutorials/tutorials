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

	// Set the index data
	// By parameter:
	// mIndexBuffer -- The IDirect3DIndexBuffer9* to use for rendering
	result = CD3DObj::mDevice->SetIndices(mIndexBuffer);
	assert(result == D3D_OK);

	// Specify vertex type
	// SVertexType -- Is the #define which defines our vertex in "vertex_types.h"
	CD3DObj::mDevice->SetFVF(SVertexType);

	// Last but not least, draw the geometry
	// By parameter:
	// D3DPT_TRIANGLELIST -- The primitive to use when rendering the vertex buffer
	//						 Our mesh always assumes triangles
	// 0 -- This is the offset from the beginning of the vertex buffer to the first
	//		vertex to be used.  So a zero in the index buffer correspondings to this number
	//		in the vertex buffer.  Since we want to use the entire vertex buffer, we pass zero
	// 0 -- This is the minimum vertex buffer index allowed.  Typically you are always going
	//		to set this to zero
	// mVertCount -- The number of vertices in the selected IDirect3DVertexBuffer9
	// 0 -- The starting index to use in the rendering.  Since we use the entire index buffer
	//		we pass zero
	// mTriCount -- The number of primitives (in our case triangles) to draw
	result = CD3DObj::mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,mVertCount,0,mTriCount);
	assert(result == D3D_OK);
}

// Set the vertex data
bool CD3DMesh::setVertexData(void *verts, int vertCount)
{
	// Safety first
	if(!verts || vertCount <= 0)
		return false;
		
	// If we already have allocated vertex data, free it first
	freeVertexData();
	
	// Store the number of vertices in the vertex buffer
	mVertCount = vertCount;
	
	// Calculate vertex buffer size in bytes
	int vbSizeInBytes = sizeof(SVertex) * mVertCount; 

	// Create vertex buffer	
	if(CD3DObj::mDevice->CreateVertexBuffer(vbSizeInBytes, 0, SVertexType, D3DPOOL_MANAGED,
											&mVertexBuffer, NULL) != D3D_OK)
	{
		return false;
	}

	VOID *v = NULL; // Pointer to our verts to be locked

	// Lock the verts
	if(mVertexBuffer->Lock(0, 0, (void**)&v, 0) != D3D_OK)
		return false;
	
	memcpy(v, verts, vbSizeInBytes); // Copy over data

	// Unlock the vertex buffer
	if(mVertexBuffer->Unlock() != D3D_OK) 
		return false;
	
	return true;
}

// Set the index data
bool CD3DMesh::setIndexData(WORD *indices, int indexCount)
{
	// Safety first
	if(!indices || indexCount <= 0)
		return false;

	// First free up any existing index data
	freeIndexData();

	// Calculate and store the number of triangles to be rendered
	mTriCount = indexCount / 3;

	// Calculate index buffer size in bytes
	int ibSizeInBytes = sizeof(WORD) * indexCount;
	
	// Now we can create index buffer
	// By paramter:
	// ibSizeInBytes -- The size of the index buffer in bytes
	// 0 -- This a D3DUSAGE flag.  An example of usage would be if we knew we going
	//		to use this index buffer for point sprites (a special type of D3D primitive)
	//		we could tag it as such by using the D3DUSAGE_POINTS flag.  However we have
	//		no special usage planned so we pass 0
	// D3DFMT_INDEX16 -- This is the data format of the index buffer.  We are using
	//					 the MS data type WORD for our index buffer.  A WORD is typedefed
	//					 to be an unsigned short.  A short is 16-bit in length, so we
	//					 pass the D3DFMT_INDEX16 to say our index buffer data format is
	//					 16-bits per index
	// D3DPOOL_MANAGED -- The type of memory allocation for this index buffer.  By
	//					  passing the D3DPOOL_MANAGED flag, we are letting D3D handle the
	//					  memory management, so D3D will be responsible for copying our
	//					  index buffer to the graphics card when it's needed for rendering
	//					  An additional benefit to using this flag, is that if we call
	//					  the IDirect3DDevice9 method Reset() for toggling between windowed
	//					  mode and full screen we will not have to recreate the index buffer
	// &mIndexBuffer -- Address of a IDirect3DIndexBuffer9* to be filled with the created
	//					index buffer
	// NULL -- This is a MS reserved parameter.  Always pass NULL.
	if(CD3DObj::mDevice->CreateIndexBuffer(ibSizeInBytes, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
										   &mIndexBuffer, NULL) != D3D_OK)
	{
		return false;
	}
	
	// Now that we have an allocated index buffer, we need to fill it with 
	// the index data.  

	VOID *i = NULL; // This is a pointer we will use to lock the
					// the index data associated with our "mIndexBuffer" 

	// Lock the index data
	// By parameter:
	// 0 -- Offset in bytes to lock in the index buffer.  By passing zero
	//		we say we want to begin locking at the start of the buffer
	// 0 -- Number of bytes to lock in the index buffer.  By passing zero
	//		we say we want to lock the entire buffer
	// &v -- Address of the pointer which will point to the memory buffer of
	//		 the D3D index buffer's data upon returning from the function.
	//		 So "i" will point to the IDirect3DIndexBuffer9's index data
	//		 that we can then fill in 
	// 0 --  This a D3DLOCK flag, that if we wanted, we could specify special
	//		 locking functionality.  We don't have any special way we want
	//		 to lock the data, so we pass zero
	if(mIndexBuffer->Lock(0, 0, (void**)&i, 0) != D3D_OK)
		return false;

	// Now that we have a pointer to our index buffer, we
	// copy over the data
	memcpy(i, indices, ibSizeInBytes); // Copy over data

	// Once we're finished copying over the data
	// we can unlock our buffer and have it ready to be rendered
	if(mIndexBuffer->Unlock() != D3D_OK) 
		return false;

	return true;
}

// Free all the memory in CD3DMesh
void CD3DMesh::freeMem()
{
	freeVertexData();
	freeIndexData();
	zeroVars();
}

// Free the mesh's vertex data
void CD3DMesh::freeVertexData()
{
	if(mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = NULL;
	}
}

// Free the mesh's index data
void CD3DMesh::freeIndexData()
{
	if(mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = NULL;
	}
}

// Zero out all the member variables of the CD3DMesh
void CD3DMesh::zeroVars()
{
	mVertexBuffer = NULL;
	mIndexBuffer = NULL;
	
	mVertCount = 0;
	mTriCount = 0;
}
