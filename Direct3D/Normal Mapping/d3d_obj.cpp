#include "d3d_obj.h"

// Declare our static variables
IDirect3D9* CD3DObj::mInterface = NULL;
IDirect3DDevice9* CD3DObj::mDevice = NULL;
ID3DXEffect* CD3DObj::mEffect = NULL;
HRESULT CD3DObj::mResult = 0;

const char kEffectFileName[] = "Shader.fx";

// Initializes our D3D object -- Returns true on success, false otherwise
bool CD3DObj::init(HWND hwnd)
{
	// Create the D3D object, which is needed to create the D3DDevice.
	mInterface = Direct3DCreate9(D3D_SDK_VERSION);

	// Error Check
	if(mInterface == NULL)
		return false;

	// This is the structure that defines how our 3D device (the thing that allows us
	// to render) is going to be created.
	D3DPRESENT_PARAMETERS params = {0}; // Start by zeroing out the parameters
	
	params.Windowed = TRUE; // The application is windowed
	params.SwapEffect = D3DSWAPEFFECT_DISCARD; // No double buffering so we have no buffer to swap							  
	params.BackBufferFormat = D3DFMT_UNKNOWN; // User the desktop's format for the frame buffer
	params.EnableAutoDepthStencil = true; // We want a z-buffer
	params.AutoDepthStencilFormat = D3DFMT_D16; // The data format for the z-buffer is 16-bit

	// First try and create a hardware device with hardware vertex processing
	mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
									   D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &mDevice);
	
	// If we can't try software vertex processing instead
	if(mResult != D3D_OK)
	{
		mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
										   D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &mDevice);
		if(mResult != D3D_OK)
			return false; // Couldn't create a D3D 9.0 device
	}
	
	ID3DXBuffer *errorBuffer = NULL; // A buffer for an error string if our effect file fails
									// to load
									
	// Load the effect file
	// By parameter:
	// mDevice -- The IDirect3DDevice9 used to load the effect file
	// kEffectFileName -- The name of the effect file
	// NULL -- An optional character array of pre-processed macros to be used
	//		   by the effect file, we have none so we pass NULL
	// NULL -- An optional ID3DXInclude interface for handling #include inside of
	//		   the effect file.  We do not use #include in the effect file, so 
	//		   we pass NULL
	// 0 -- Compiler option flags for the shader.  Passing 0 indicates no special
	//		compiler flags
	// NULL -- An optional ID3DXEffectPool object for sharing data between multiple
	//		   effect files.  We only have one effect file, so we pass NULL
	// &mEffect -- Pointer to an ID3DXEffect to be created
	// &errorBuffer -- Pointer to a ID3DXBuffer to be filled with an error string
	//				   in the event an error occurs
	mResult = D3DXCreateEffectFromFile(mDevice, kEffectFileName,	NULL, NULL,	0, 
									   NULL, &mEffect, &errorBuffer);
	
	// If an error occurred								   
	if(mResult != D3D_OK)
	{
		// Get the error string
		const char *str = NULL;
		
		if(errorBuffer)
			str = (const char*)errorBuffer->GetBufferPointer();
		else
			str = "Error loading effect file";
		
		// Print it to the screen
		MessageBox(hwnd, str, "Shader Compilation Error", MB_OK | MB_ICONERROR);
			return false;
	}
	
	// Turn off lighting
	if(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE) != D3D_OK)
		return false;
		
	// Turn on Z-buffering
	if(mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) != D3D_OK)
		return false;
	
	return true; // We got loaded
}

bool CD3DObj::initForDebug(HWND hwnd)
{
	// Create the D3D object, which is needed to create the D3DDevice.
	mInterface = Direct3DCreate9(D3D_SDK_VERSION);

	// Error Check
	if(mInterface == NULL)
		return false;

	// This is the structure that defines how our 3D device (the thing that allows us
	// to render) is going to be created.
	D3DPRESENT_PARAMETERS params = {0}; // Start by zeroing out the parameters

	params.Windowed = TRUE; // The application is windowed
	params.SwapEffect = D3DSWAPEFFECT_DISCARD; // No double buffering so we have no buffer to swap							  
	params.BackBufferFormat = D3DFMT_UNKNOWN; // User the desktop's format for the frame buffer
	params.EnableAutoDepthStencil = true; // We want a z-buffer
	params.AutoDepthStencilFormat = D3DFMT_D16; // The data format for the z-buffer is 16-bit

	// Here we create a device that is capable of debugging
	mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwnd,
									   D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &mDevice);

	if(mResult != D3D_OK)
		return false; // Couldn't create device

	ID3DXBuffer *errorBuffer = NULL; // A buffer for an error string if our effect file fails to load

	// Load the effect file so that it is capable of debugging
	// By parameter:
	// mDevice -- The IDirect3DDevice9 used to load the effect file
	// kEffectFileName -- The name of the effect file
	// NULL -- An optional character array of pre-processed macros to be used
	//		   by the effect file, we have none so we pass NULL
	// NULL -- An optional ID3DXInclude interface for handling #include inside of
	//		   the effect file.  We do not use #include in the effect file, so 
	//		   we pass NULL
	// D3DXSHADER_DEBUG -- Compiler option flags for the shader.  This flag says to 
	//					   build in file name, line numbers, and debug symbol information when
	//					   compiling the shader.
	// NULL -- An optional ID3DXEffectPool object for sharing data between multiple
	//		   effect files.  We only have one effect file, so we pass NULL
	// &mEffect -- Pointer to an ID3DXEffect to be created
	// &errorBuffer -- Pointer to a ID3DXBuffer to be filled with an error string
	//				   in the event an error occurs
	mResult = D3DXCreateEffectFromFile(mDevice, kEffectFileName, NULL, NULL, D3DXSHADER_DEBUG, 
									   NULL, &mEffect, &errorBuffer);

	// If an error occurred								   
	if(mResult != D3D_OK)
	{
		// Get the error string
		const char *str = NULL;

		if(errorBuffer)
			str = (const char*)errorBuffer->GetBufferPointer();
		else
			str = "Error loading effect file";

		// Print it to the screen
		MessageBox(hwnd, str, "Shader Compilation Error", MB_OK | MB_ICONERROR);
			return false;
	}
	
	// Turn off lighting
	if(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE) != D3D_OK)
		return false;
		
	// Turn on Z-buffering
	if(mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) != D3D_OK)
		return false;

	return true; // We got loaded
}

// Begins the scene
void CD3DObj::begin()
{
	// This begins our scene.
	mResult = mDevice->BeginScene();
	assert(mResult == D3D_OK);
}

// End the scene and draw it
void CD3DObj::end()
{
	// This ends the scene
	mResult = mDevice->EndScene();
	assert(mResult == D3D_OK);

	// Present the finished scene to the screen
	mResult = mDevice->Present(NULL, NULL, NULL, NULL);
	assert(mResult == D3D_OK);
}

// Renders a D3D Mesh
bool CD3DObj::render(ID3DXMesh *mesh)
{
	assert(mesh != NULL);

	IDirect3DVertexBuffer9 *vertexBuffer = NULL;
	IDirect3DIndexBuffer9 *indexBuffer = NULL;

	int numVerts = 0;
	int numTris = 0;

	mesh->GetVertexBuffer(&vertexBuffer);
	mesh->GetIndexBuffer(&indexBuffer);
	
	numVerts = mesh->GetNumVertices();
	numTris = mesh->GetNumFaces();

	mDevice->SetIndices(indexBuffer);
	mDevice->SetStreamSource(0, vertexBuffer, 0, mesh->GetNumBytesPerVertex());

	// Set the flexible vertex format
	mDevice->SetFVF(mesh->GetFVF());

	// Render the mesh
	mResult = mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numTris);
		return (mResult == D3D_OK);
}

// Renders a SMesh
bool CD3DObj::render(const char *technique, const SMesh *mesh)
{
	assert(mEffect != NULL);

	// Error check
	if(!mesh || !technique) 
		return false;

	unsigned int numPasses = 0; // Used to store the number of render passes
	int numTris = mesh->indexCount / 3;
	
	// Set the technique to be used
	mResult = mEffect->SetTechnique(technique);
	assert(mResult == D3D_OK);
	
	// Set the vertex declaration
	mResult = mDevice->SetVertexDeclaration(mesh->vertexDeclaration);
	assert(mResult == D3D_OK);

	// Begin the current technique
	// By parameter:
	// &numPasses -- Address to a unsigned int to be filled with the number of passes in the technique
	// 0 -- Flag that defines how the effect should save/restore render states.  By passing
	//		zero we say "Save and restore ALL render states"
	mEffect->Begin(&numPasses, 0);

	for(unsigned int i = 0; i < numPasses; ++i)
	{
		// Begin the 'ith' pass on the selected technique
		mEffect->BeginPass(i);

		mDevice->SetIndices(mesh->indexBuffer);
		mDevice->SetStreamSource(0, mesh->vertexBuffer, 0, sizeof(SVertex));

		// Render the mesh
		mResult = mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh->vertexCount, 0, numTris);

		// End the 'ith' pass
		mEffect->EndPass();

		// Error check.  Make sure that drawing the primitives succeeded
		if(mResult != D3D_OK)
			return false;
	}

	// End the current technique
	mEffect->End();
		return true;
}

bool CD3DObj::renderLine(const CPos &start, const CPos &end, DWORD color)
{
	// Set the FVF 
	mDevice->SetFVF(SPosColor::kFVF);
	
	SPosColor verts[2] = { start.x, start.y, start.z, color,
						   end.x, end.y, end.z, color };

	// Render the line segments
	mResult = mDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, verts, sizeof(SPosColor));
	return (mResult == D3D_OK); // Returns status of DrawPrimitiveUP()
}

// Sets the projection matrix which dictates how our 3D scene is projected onto our 2D monitor
void CD3DObj::setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip)
{
	assert(mEffect != NULL);
	
	// Create the projection matrix
	D3DXMATRIXA16 matrix;
	D3DXMatrixPerspectiveFovLH(&matrix, fov, aspectRatio, nearClip, farClip);
	
	// Set the projection matrix for fixed function pipeline
	mResult = mDevice->SetTransform(D3DTS_PROJECTION, &matrix);
	assert(mResult == D3D_OK);
	
	// Set the projection matrix in shader
	mResult = mEffect->SetMatrix("gProjMat", &matrix);
	assert(mResult == D3D_OK);
}

// Sets up the view of the scene based on passed in eye and target
void CD3DObj::setViewMatrix(const CPos &eye, const CPos &lookAt)
{
	assert(mEffect != NULL);

	D3DXMATRIXA16 matrix;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // World's up vector

	// Create the view matrix
	// **NOTE** We can cast to (D3DXVECTOR3*) because our CPos has it's data members (x,y,z)
	// declared the same way a D3DXVECTOR3 does
	D3DXMatrixLookAtLH(&matrix, (D3DXVECTOR3*)(&eye), (D3DXVECTOR3*)(&lookAt), &up);
	
	// Set the view matrix for fixed function pipeline
	mResult = mDevice->SetTransform(D3DTS_VIEW, &matrix);
	assert(mResult == D3D_OK);
	
	// Set the view matrix for shader
	mResult = mEffect->SetMatrix("gViewMat", &matrix);
	assert(mResult == D3D_OK);
}

// Sets the world matrix to the matrix passed in
void CD3DObj::setWorldMatrix(const D3DXMATRIX *matrix)
{
	assert(mEffect != NULL);
	
	// Set the world matrix for fixed function pipeline
	mResult = mDevice->SetTransform(D3DTS_WORLD, matrix);
	assert(mResult == D3D_OK);
	
	// Set the world matrix for shader
	mResult = mEffect->SetMatrix("gWorldMat", matrix);
	assert(mResult == D3D_OK);
}

// Clears the viewport to a specified ARGB color
bool CD3DObj::clear(int color, float zDepth)
{
	// Clear the color and z-depth
	mResult = mDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, zDepth, 0);
		return (mResult == D3D_OK);
}

// Free up all the memory
void CD3DObj::deinit()
{
	if(mDevice != NULL)
		mDevice->Release();

	if(mInterface != NULL)
		mInterface->Release();
		
	if(mEffect != NULL)
		mEffect->Release();

	// Zero out our D3D interface, device, and effect
	mDevice = NULL;
	mInterface = NULL;
	mEffect = NULL;
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object