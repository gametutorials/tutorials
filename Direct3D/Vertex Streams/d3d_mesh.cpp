#include "d3d_mesh.h"

////////////////
// Constants //
//////////////

// Maximum number of elements for our mesh 
const int kMaxElements = 10;

// Helper function for setting a D3DVERTEXELEMENT9
inline void SetElement(D3DVERTEXELEMENT9 &element, WORD stream, WORD offset, BYTE type,
					   BYTE method, BYTE usage, BYTE usageIndex)
{
	element.Stream = stream;
	element.Offset = offset;
	element.Type = type;
	element.Method = method;
	element.Usage = usage;
	element.UsageIndex = usageIndex;
}

// Render the mesh
void CD3DMesh::render(bool useDiffuse, bool useTexture)
{
	assert(mIndexBuffer != NULL); // Must have a valid index buffer
	assert(mPosVB != NULL); // Must have at least position data set to render

	HRESULT result; // Used for checking D3D error codes
	int stream = 0; // Holds the current stream number
	
	// Based on "useDiffuse" and "useTextue" create the appropriate vertex declaration
	// for this render call
	if(!setupVertexDeclaration(useDiffuse, useTexture))
		return; // If we can't setup the vertex declaration, we can't draw anything
	
	// First set the vertex declaration
	// This tells D3D what data to expect when you call DrawIndexedPrimitive()
	CD3DObj::mDevice->SetVertexDeclaration(mVertexDeclaration);
	
	// Stream zero will always be position data since it MUST be set
	// By parameter:
	// stream++ -- The stream number of the data.  D3D allows for multiple streams
	//			   of data to be used in per render call which of course makes
	//			   "vertex streams" possible.  "stream" starts at zero, then we
	//			   increment for the next usage.
	// mPosVB -- The IDirect3DVertexBuffer9* to use for this stream
	// 0 -- Offset in bytes, to the beginning of where to start rendering in
	//		the vertex buffer.  We want to begin rendering from the start of the 
	//		the vertex buffer so we pass zero
	// sizeof(CPos) -- The size, in bytes, of an individual element stored 
	//				   in the vertex buffer
	result = CD3DObj::mDevice->SetStreamSource(stream++, mPosVB, 0, sizeof(CPos));
	assert(result == D3D_OK);
	
	// If we have diffuse colors and we are rendering with vertex colors turned on
	if(mColorVB && useDiffuse)
	{
		// We don't know if a texture was used for drawing before us, so we
		// turn off texturing to be on the safe side  
		CD3DObj::mDevice->SetTexture(0, NULL);
		
		// Set the next stream with diffuse colors
		CD3DObj::mDevice->SetStreamSource(stream++, mColorVB, 0, sizeof(DWORD));
	}
	
	// If we have texture coordinates and we are rendering with texturing turned on
	if(mTexCoordVB && useTexture)
	{
		assert(mTexture != NULL);
		
		// Select the mesh's texture
		mTexture->select();
		
		// Set the next stream with texture coordinates
		CD3DObj::mDevice->SetStreamSource(stream, mTexCoordVB, 0, sizeof(CTexCoord));
	}
	
	// All of the CD3DMesh's must have an index buffer, so
	// set the indices for rendering
	result = CD3DObj::mDevice->SetIndices(mIndexBuffer);
	assert(result == D3D_OK);
	
	// Lastly, render the mesh
	result = CD3DObj::mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mVertCount, 0, mTriCount);
	assert(result == D3D_OK);
}

// Loads a texture into the mesh 
bool CD3DMesh::loadTexture(const char *fileName)
{
	assert(fileName != NULL);
	
	// If the texture has not yet been allocated
	if(mTexture == NULL)
	{
		mTexture = new CD3DTexture; // Allocate memory
		
		// Make sure we could allocate the memory
		if(!mTexture)
			return false;
	}
	
	// Return the status of loading the texture
	return mTexture->load(fileName);
}

// Sets the position (x,y,z) data of the mesh
// Returns true on success, false otherwise
bool CD3DMesh::setPosition(const CPos *pos, int count)
{
	// Safety first!  Make sure passed in parameters aren't bogus
	if(!pos || count <= 0)
		return false;
	
	// If the mesh's positions in 3D space are already allocated, freeVertexData() must be
	// called before different positions can be set.
	// **NOTE** freeVertexData() will free ALL vertex data	
	if(mPosVB)
		return false;
	
	// Since every vertex MUST have a 3D position, we will use the
	// count of positions as the overall vertex count
	mVertCount = count;
	
	// Calculate vertex buffer size in bytes
	int vbSizeInBytes = sizeof(CPos) * mVertCount; 

	// Create the D3D vertex buffer for our position data
	// By parameter:
	// vbSizeInBytes -- Size of the vertex buffer in bytes
	// 0 -- A usage flag for special usage of the buffer.  We have no special
	//		usage so we pass zero.
	// 0 -- The FVF code for the vertex buffer.  Since we are not using FVF but
	//		instead vertex declarations, we pass zero.
	// D3DPOOL_MANAGED -- Memory pool to place buffer in.  D3DPOOL_MANAGED says
	//					  "Let D3D take care of memory details for the vertex buffer"
	// &mPosVB -- Pointer to IDirect3DVertexBuffer9 to be filled
	// NULL -- Must be set to NULL
	if(CD3DObj::mDevice->CreateVertexBuffer(vbSizeInBytes, 0, 0, D3DPOOL_MANAGED,
											&mPosVB, NULL) != D3D_OK)
	{
		return false;
	}

	VOID *data = NULL; // Pointer to our data to be locked

	// Lock the position data
	if(mPosVB->Lock(0, 0, (void**)&data, 0) != D3D_OK)
		return false;
	
	memcpy(data, pos, vbSizeInBytes); // Copy over the position data

	// Unlock the position data
	if(mPosVB->Unlock() != D3D_OK) 
		return false;
		
	return true;
}

// Sets the diffuse color (DWORD (A,R,G,B)) data of the mesh
// Returns true on success, false otherwise
bool CD3DMesh::setDiffuseColor(const DWORD *colors, int count)
{
	// Safety first!  Make sure passed in parameters aren't bogus
	if(!colors || count <= 0)
		return false;
	
	// If the vertices diffuse colors are already allocated, freeVertexData() must be
	// called before a new colors can be set.
	// **NOTE** freeVertexData() will free ALL vertex data	
	if(mColorVB)
		return false;
	
	// Since every vertex MUST have a 3D position, we will use the
	// count of positions as the overall vertex count.  Therefore, 
	// for each position, there MUST be a color otherwise we can't set
	// the diffuse color for the mesh
	if(count != mVertCount)
		return false;
	
	// Calculate vertex buffer size in bytes
	int vbSizeInBytes = sizeof(DWORD) * mVertCount; 

	// Create the D3D vertex buffer for our diffuse color data
	// By parameter:
	// vbSizeInBytes -- Size of the vertex buffer in bytes
	// 0 -- A usage flag for special usage of the buffer.  We have no special
	//		usage so we pass zero.
	// 0 -- The FVF code for the vertex buffer.  Since we are not using FVF but
	//		instead vertex declarations, we pass zero.
	// D3DPOOL_MANAGED -- Memory pool to place buffer in.  D3DPOOL_MANAGED says
	//					  "Let D3D take care of memory details for the vertex buffer"
	// &mColorVB -- Pointer to IDirect3DVertexBuffer9 to be filled
	// NULL -- Must be set to NULL
	if(CD3DObj::mDevice->CreateVertexBuffer(vbSizeInBytes, 0, 0, D3DPOOL_MANAGED,
											&mColorVB, NULL) != D3D_OK)
	{
		return false;
	}

	VOID *data = NULL; // Pointer to our data to be locked

	// Lock the diffuse color data
	if(mColorVB->Lock(0, 0, (void**)&data, 0) != D3D_OK)
		return false;
	
	memcpy(data, colors, vbSizeInBytes); // Copy over the diffuse color data

	// Unlock the color data
	if(mColorVB->Unlock() != D3D_OK) 
		return false;
		
	return true;
}

// Sets the texture coorinate (u,v) data of the mesh
// Returns true on success, false otherwise
bool CD3DMesh::setTexCoords(const CTexCoord *tc, int count)
{
	// Safety first!  Make sure passed in parameters aren't bogus
	if(!tc || count <= 0)
		return false;
	
	// If texture coordinates are already allocated, freeVertexData() must be
	// called before a new one can be set.
	// **NOTE** freeVertexData() will free ALL vertex data	
	if(mTexCoordVB)
		return false;
	
	// Since every vertex MUST have a 3D position, we use the count
	// of positions as the overall vertex count.  Therefore, for each
	// position, there MUST be a texture coordinate otherwise we can't set
	// the texture coordinates for the mesh.
	if(count != mVertCount)
		return false;
	
	// Calculate vertex buffer size in bytes
	int vbSizeInBytes = sizeof(CTexCoord) * mVertCount; 

	// Create the D3D vertex buffer for our texture coordinate data
	// By parameter:
	// vbSizeInBytes -- Size of the vertex buffer in bytes
	// 0 -- A usage flag for special usage of the buffer.  We have no special
	//		usage so we pass zero.
	// 0 -- The FVF code for the vertex buffer.  Since we are not using FVF but
	//		instead vertex declarations, we pass zero.
	// D3DPOOL_MANAGED -- Memory pool to place buffer in.  D3DPOOL_MANAGED says
	//					  "Let D3D take care of memory details for the vertex buffer"
	// &mTexCoordVB -- Pointer to IDirect3DVertexBuffer9 to be filled
	// NULL -- Must be set to NULL
	if(CD3DObj::mDevice->CreateVertexBuffer(vbSizeInBytes, 0, 0, D3DPOOL_MANAGED,
											&mTexCoordVB, NULL) != D3D_OK)
	{
		return false;
	}

	VOID *data = NULL; // Pointer to our data to be locked

	// Lock the texture coordinate data
	if(mTexCoordVB->Lock(0, 0, (void**)&data, 0) != D3D_OK)
		return false;
	
	memcpy(data, tc, vbSizeInBytes); // Copy over the texture coordinate data

	// Unlock the texture coordinate data
	if(mTexCoordVB->Unlock() != D3D_OK) 
		return false;
		
	return true;
}

// Set the index buffer
// Return true for success, false otherwise
bool CD3DMesh::setIndexBuffer(const WORD *indices, int indexCount)
{
	// Safety first
	if(!indices || indexCount <= 0)
		return false;

	// If an index buffer is already allocated, freeIndexBuffer() must be
	// called before a new one can be set
	if(mIndexBuffer)
		return false;

	// Calculate and store the number of triangles to be rendered
	mTriCount = indexCount / 3;

	// Calculate index buffer size in bytes
	int ibSizeInBytes = sizeof(WORD) * indexCount;
	
	// Create the index buffer
	if(CD3DObj::mDevice->CreateIndexBuffer(ibSizeInBytes, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
										   &mIndexBuffer, NULL) != D3D_OK)
	{
		return false;
	}
	
	VOID *data = NULL; // Pointer to our data to be locked
	
	// Lock the index buffer data
	if(mIndexBuffer->Lock(0, 0, (void**)&data, 0) != D3D_OK)
		return false;

	memcpy(data, indices, ibSizeInBytes); // Copy over data

	// Unlock the buffer so it is ready for use
	if(mIndexBuffer->Unlock() != D3D_OK) 
		return false;

	return true; // Success!
}

// Free the mesh's vertex data
void CD3DMesh::freeVertexData()
{
	// If we have position data
	if(mPosVB)
	{
		mPosVB->Release(); // Release it
		mPosVB = NULL;
	}
	
	// If we have color data
	if(mColorVB)
	{
		mColorVB->Release(); // Release it
		mColorVB = NULL;
	}
	
	// If we have texture coordinate data
	if(mTexCoordVB)
	{
		mTexCoordVB->Release(); // Be a good warden and release it
		mTexCoordVB = NULL;
	}
	
	// If we have a vertex declaration
	if(mVertexDeclaration)
	{
		mVertexDeclaration->Release(); // You know the drill... Release it!
		mVertexDeclaration = NULL;
	}
}

// Free the mesh's index data
void CD3DMesh::freeIndexBuffer()
{
	if(mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = NULL;
	}
}

// Create the IDirect3DIndexBuffer9* based on what data is set and what data is
// selected to be rendered with the booleans "useDiffuse" and "useTexture"
// Returns true for success, false otherwise 
bool CD3DMesh::setupVertexDeclaration(bool useDiffuse, bool useTexture)
{
	// First, if the declaration is already set, free it up first
	if(mVertexDeclaration)
	{
		mVertexDeclaration->Release();
		mVertexDeclaration = NULL;
	}
	
	/*
		The layout of our mesh is:
		
			Position data (x,y,z) --> REQUIRED!!!
			Color data (DWORD) --> OPTIONAL
			Texture Coordinate data (u,v) --> OPTIONAL
			
		Each piece of data, if it exists, will correspond to a D3DVERTEXELEMENT9 which is 
		responsible for defining what the data is and how it is to be rendered.  We create
		a temporary array for filling out based on what data the mesh currently contains.
	*/
	D3DVERTEXELEMENT9 e[kMaxElements] = {0}; // Array of D3DVERTEXELEMENT9
	int i = 0; // A counter
	
	// The D3DVERTEXELEMENT9 is a struct that defines a piece of vertex data.  Lets 
	// take a look at each member in the struct
	/*
		The struct D3DVERTEXELEMENT9:
			
			Stream -- A WORD that equals the stream number.  This corresponds to what vertex stream,
					  the piece of data will be sent on.  
			Offset -- A WORD that equals an offset, in bytes, from the start of stream to the
					  a specific piece of data in the stream.  Just like when using FVF's, if we
					  wanted we could pack data (for instance position and vertex color data, in this
					  order) into one vertex buffer.  The stream for both position and
					  vertex color data would be the same, but the offset to the position data would
					  be zero and the offset to the vertex color data would be the size of the 
					  position data in bytes.
					  
					  If we have a seperate stream for each piece of vertex data (like we do
					  in this tutorial), offset will always equal zero.
			Type -- A BYTE that defines what type of data it is.  For example:
			
						D3DDECLTYPE_FLOAT1 == one float
						D3DDECLTYPE_FLOAT2 == two floats
						D3DDECLTYPE_D3DCOLOR == A packed DWORD in the form ARGB
						
					The full list of possible data tyes can be found in MSDN by searching
					for D3DDECLTYPE.		
			Method -- A BYTE that defines how to tessellate the data.  Usually you'll
					  want to use D3DDECLMETHOD_DEFAULT, which says to "Treat the data as you
					  normally would".  Check out MSDN and search for D3DDECLMETHOD to read about
					  advanced methods to tessellate geoemtry. 
			BYTE Usage -- A BYTE that defins the intended usage of the data.  Each portion of 
						  vertex data such as, position, normal, color, textured coordinates,
						  has it's own flag to state how the data is to be used.  Check out
						  D3DDECLUSAGE on MSDN for all the different usages.
						  **NOTE** If you are defining a custom type of usage not normally
								   found in the FVF codes, use D3DDECLUSAGE_TEXCOORD
			BYTE UsageIndex -- A BYTE that provides extra usage information and allows for
							   one-to-one mapping of D3DVERTEXELEMENT9 to FVF codes.  In general
							   this value will be zero.
							   
		Below we show how to set variables contained in a D3DVERTEXELEMENT9 in a practical way
	*/
	
	// If we have position data, set a vertex element for position data
	if(mPosVB)
	{
		// Set the element for vertex position
		// 
		// e[i] -- The current D3DVERTEXELEMENT9 element to set.  Every time we set a element we 
		//		   increment 'i' so 'i' will always equal the correct index into the array
		//		   of available D3DVERTEXELEMENT9.
		// i -- The stream that this data will be sent on.  Every time we set a element we 
		//		increment 'i' so 'i' will always equal the current stream
		// 0 -- Offset into the stream to the start of the data.  Since the stream only contains
		//		position data, there is no offset
		// D3DDECLTYPE_FLOAT3 -- Type of data being sent.  Position data constists of a (x,y,z) that
		//						 are each a float.
		// D3DDECLMETHOD_DEFAULT -- Method for tessellating the data.  We want it tessellated just like
		//							it is by default.
		// D3DDECLUSAGE_POSITION -- The use this data is intended for.  We are setting position data
		//							so we set the appropriate flag.
		// 0 -- An additional usage flag.  We have no additional usage so we set this to zero.
		SetElement(e[i], i, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
		++i; // Move to the next element
	}
	
	// If the color vertex buffer is valid, and we want to use it, set a vertex
	// element for vertex color data
	if(mColorVB && useDiffuse)
	{
		// Set the element for vertex color
		// 
		// e[i] -- The current D3DVERTEXELEMENT9 element to set.  Every time we set a element we 
		//		   increment 'i' so 'i' will always equal the correct index into the array
		//		   of available D3DVERTEXELEMENT9.
		// i -- The stream that this data will be sent on.  Every time we set a element we 
		//		increment 'i' so 'i' will always equal the current stream
		// 0 -- Offset into the stream to the start of the data.  Since the stream only contains
		//		vertex color data, there is no offset
		// D3DDECLTYPE_D3DCOLOR -- Type of data being sent.  A D3DCOLOR is made up of 4 bytes (A,R,G,B)
		//						   that are packed into a DWROD.
		// D3DDECLMETHOD_DEFAULT -- Method for tessellating the data.  We want it tessellated just like
		//							it is by default.
		// D3DDECLUSAGE_COLOR -- The use this data is intended for.  We are setting color data
		//						 so we set the appropriate flag.
		// 0 -- An additional usage flag.  We have no additional usage so we set this to zero.
		SetElement(e[i], i, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
		++i; // Move to the next element
	}
	
	// If the texture coordinate vertex buffer is valid and we want to use it, set a vertex
	// element for texture coordinate data
	if(mTexCoordVB && useTexture)
	{
		// Set the element for a set of texture coordinates
		// 
		// e[i] -- The current D3DVERTEXELEMENT9 element to set.  Every time we set a element we 
		//		   increment 'i' so 'i' will always equal the correct index into the array
		//		   of available D3DVERTEXELEMENT9.
		// i -- The stream that this data will be sent on.  Every time we set a element we 
		//		increment 'i' so 'i' will always equal the current stream
		// 0 -- Offset into the stream to the start of the data.  Since the stream only contains
		//		vertex color data, there is no offset
		// D3DDECLTYPE_FLOAT2 -- Type of data being sent.  Texture coordiantes (u,v) consist of 
		//						 two floats, so we set the appropriate flag.
		// D3DDECLMETHOD_DEFAULT -- Method for tessellating the data.  We want it tessellated just like
		//							it is by default.
		// D3DDECLUSAGE_TEXCOORD -- The use this data is intended for.  We are setting texture
		//							coordinate data so we set the appropriate flag.
		// 0 -- An additional usage flag.  We have no additional usage so we set this to zero.
		SetElement(e[i], i, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
		++i; // Move to the next element
	}
	
	/*
		When you are finished setting all the vertex elements for your custom vertex
		declaration, you have to set a final element.  This final element tell D3D "Hey, this
		is where the custom declaration stops!".
		
		**NOTE** D3D provides a macro for setting the last element for a custom vertex 
				 declaration.  It is D3DDECL_END()
	*/
	SetElement(e[i], 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0); // Mark the end of the vertex element
														    // array.  This is similar to a NULL at
														    // the end of a character string.
	
	// Once we have the custom array of vertex elements filled in, we can use them to
	// create our custom vertex declaration.  By parameter:
	// e -- An array of D3DVERTEXELEMENT9 that spell out how the vertex data will
	//		be sent to the graphics card for rasterization.  The final element must
	//		be set to D3DDECL_END()
	// &mVertexDeclaration -- Pointer to a IDirect3DVertexDeclaration9* to be filled with
	//						  the custom vertex declaration
	if(CD3DObj::mDevice->CreateVertexDeclaration(e, &mVertexDeclaration) != D3D_OK)
		return false;
		
	return true; // Success!
}

// Free up all the dynamic data in the mesh
void CD3DMesh::freeMem()
{
	// If there is a texture, free it
	if(mTexture)
	{
		delete mTexture;
		mTexture = NULL;
	}
	
	freeVertexData(); // Free vertex data
	freeIndexBuffer(); // Free index data
}
