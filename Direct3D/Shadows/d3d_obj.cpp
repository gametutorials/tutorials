#include "d3d_obj.h"

// Declare our static variables
IDirect3D9* CD3DObj::mInterface = NULL;
IDirect3DDevice9* CD3DObj::mDevice = NULL;
ID3DXEffect* CD3DObj::mEffect = NULL;
HRESULT CD3DObj::mResult = 0;
bool CD3DObj::mFullScreen = false; // Start in windowed mode
HWND CD3DObj::mHWND = NULL;
float CD3DObj::mFOV = 0.0f;
float CD3DObj::mNearClip = 0.0f;
float CD3DObj::mFarClip = 0.0f;
float CD3DObj::mAspectRatio = 0.0f;

////////////////
// Constants //
//////////////

const char kEffectFileName[] = "Shader.fx"; // Name of .fx file

// Initializes our D3D object -- Returns true on success, false otherwise
bool CD3DObj::init(HWND hwnd)
{
	// Set the window handle
	mHWND = hwnd;

	// Create the D3D object, which is needed to create the D3DDevice.
	mInterface = Direct3DCreate9(D3D_SDK_VERSION);

	// Error Check
	if(mInterface == NULL)
		return false;

	if(!createDevice(false))
		return false;
	
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
	mResult = D3DXCreateEffectFromFile(mDevice, kEffectFileName, NULL, NULL, 0, 
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
	
	return setDefaultStates(); // Return the status of setting the default render states
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
	params.BackBufferCount = 1; // Number of render surfaces
	params.SwapEffect = D3DSWAPEFFECT_FLIP; // We want to treat the back buffers as a circular
											// queue and rotate through them							  
	params.BackBufferFormat = D3DFMT_UNKNOWN; // User the desktop's format for the frame buffer
	params.EnableAutoDepthStencil = true; // We want a z-buffer and a stencil buffer
	params.AutoDepthStencilFormat = D3DFMT_D24S8; // 24-bit Z-buffer, 8-bit stencil buffer

	if(mResult != D3D_OK)
		return false; // Couldn't create device

	ID3DXBuffer *errorBuffer = NULL; // A buffer for an error string if our effect file fails to load
	
	// The compiler flags for the shader.  Here is what each flag means:
	// D3DXSHADER_DEBUG -- This flag says to build in file name, line numbers, and debug symbol
	//					   information when compiling the shader
	// D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT -- Turns off optimizations for pixel shaders and turns on
	//										 debugging for pixel shaders
	// D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT -- Turns off optimizations for vertex shaders and turns on
	//										 debugging for vertex shaders
	DWORD compilerOpts = D3DXSHADER_DEBUG | D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT |
						 D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;

	// Load the effect file so that it is capable of debugging
	// By parameter:
	// mDevice -- The IDirect3DDevice9 used to load the effect file
	// kEffectFileName -- The name of the effect file
	// NULL -- An optional character array of pre-processed macros to be used
	//		   by the effect file, we have none so we pass NULL
	// NULL -- An optional ID3DXInclude interface for handling #include inside of
	//		   the effect file.  We do not use #include in the effect file, so 
	//		   we pass NULL
	// compilerOpts -- Compiler option flags for the shader
	// NULL -- An optional ID3DXEffectPool object for sharing data between multiple
	//		   effect files.  We only have one effect file, so we pass NULL
	// &mEffect -- Pointer to an ID3DXEffect to be created
	// &errorBuffer -- Pointer to a ID3DXBuffer to be filled with an error string
	//				   in the event an error occurs
	mResult = D3DXCreateEffectFromFile(mDevice, kEffectFileName, NULL, NULL, compilerOpts, 
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

	return setDefaultStates(); // Return the status of setting the default render states
}

// Puts the renderer in a default state
bool CD3DObj::setDefaultStates()
{
	// Turn on Z-buffering
	if(mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE) != D3D_OK)
		return false;
	
	// Turn off vertex lighting	
	if(mDevice->SetRenderState(D3DRS_LIGHTING, FALSE) != D3D_OK)
		return false;
		
	return true;
}

bool CD3DObj::toggleFullScreen()
{
	assert(mDevice != NULL);

	static RECT winRect = {0};
	D3DPRESENT_PARAMETERS params = {0}; // Presentation parameters to be filled

	// Toggle full screen flag
	mFullScreen = !mFullScreen; 

	// Get presentation parameters for the new current full screen mode
	setPresentationParams(params);

	// If we're changing to full screen mode, save off the window's RECT so
	// we can put the window back in position when switching back to windowed mode
	if(mFullScreen == true)
		GetWindowRect(mHWND, &winRect);
	
	// Notify the effect interface that a device reset is about to happen	
	if(mEffect->OnLostDevice() != D3D_OK)
	{
		// Put flag back
		mFullScreen = !mFullScreen;
			return false;
	}
	
	// Reset the device to the new parameters
	if(mDevice->Reset(&params) != D3D_OK)
	{
		// Put flag back
		mFullScreen = !mFullScreen;
			return false;
	}
	
	// Update the effect file so it knows Reset() has been called on the device
	mEffect->OnResetDevice();

	// If we've changed to windowed mode
	if(mFullScreen == false)
	{
		// Move and resize window
		SetWindowPos(mHWND, HWND_NOTOPMOST, winRect.left, winRect.top, winRect.right - winRect.left,
					 winRect.bottom - winRect.top, SWP_SHOWWINDOW);
	}

	// We must reset the render states
	setDefaultStates();
	setProjMatrix(mFOV, mNearClip, mFarClip);
		return true;
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

// Renders a D3D Mesh with a shader
bool CD3DObj::render(const char *technique, ID3DXMesh *mesh)
{
	assert(mEffect != NULL);

	// Error check
	if(!mesh || !technique) 
		return false;

	IDirect3DVertexBuffer9 *vertexBuffer = NULL;
	IDirect3DIndexBuffer9 *indexBuffer = NULL;

	unsigned int numPasses = 0; // Used to store the number of render passes
	int numVerts = 0;
	int numTris = 0;

	mesh->GetVertexBuffer(&vertexBuffer);
	mesh->GetIndexBuffer(&indexBuffer);

	numVerts = mesh->GetNumVertices();
	numTris = mesh->GetNumFaces();

	// Set the technique to be used
	mEffect->SetTechnique(technique);

	// Begin the current technique
	mEffect->Begin(&numPasses, 0);

	for(unsigned int i = 0; i < numPasses; ++i)
	{
		// Begin the 'ith' pass on the selected technique
		mEffect->BeginPass(i);

		mDevice->SetIndices(indexBuffer);
		mDevice->SetStreamSource(0, vertexBuffer, 0, sizeof(SVertex));

		// Set the flexible vertex format
		mDevice->SetFVF(SVertexType);

		// Render the mesh
		mResult = mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numTris);

		// End the 'ith' pass
		mEffect->EndPass();

		// Error check -- Make sure that drawing the primitives succeeded
		if(mResult != D3D_OK)
			return false;
	}

	// End the current technique
	mEffect->End();
		return true;
}

// Renders a D3D Mesh
bool CD3DObj::render(ID3DXMesh *mesh)
{
	// Error check
	if(!mesh) 
		return false;
	
	// Pointers to our vertex buffer and index buffer
	IDirect3DVertexBuffer9 *vertexBuffer = NULL;
	IDirect3DIndexBuffer9 *indexBuffer = NULL;

	mesh->GetVertexBuffer(&vertexBuffer); // Get the vertex buffer for the mesh
	mesh->GetIndexBuffer(&indexBuffer); // Get the index buffer for the mesh

	int numVerts = mesh->GetNumVertices(); // Get the number of vertices in the mesh
	int numTris = mesh->GetNumFaces(); // Get the number of triangles in the mesh

	// Set the index buffer
	mDevice->SetIndices(indexBuffer);
	
	// Set the vertex buffer
	mDevice->SetStreamSource(0, vertexBuffer, 0, sizeof(SVertex));

	// Set the flexible vertex format
	mDevice->SetFVF(SVertexType);

	// Render the mesh
	mResult = mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numTris);
		return (mResult == D3D_OK);
}

// Sets the projection matrix which dictates how our 3D scene is projected onto our 2D monitor
void CD3DObj::setProjMatrix(float fov, float nearClip, float farClip)
{
	assert(mEffect != NULL);
	
	// Create the projection matrix
	D3DXMATRIXA16 matrix;
	
	mFOV = fov;
	mNearClip = nearClip;
	mFarClip = farClip;
	
	// Create projection matrix
	D3DXMatrixPerspectiveFovLH(&matrix, mFOV, mAspectRatio, mNearClip, mFarClip);
	
	// Set the projection matrix
	mResult = mDevice->SetTransform(D3DTS_PROJECTION, &matrix);
	assert(mResult == D3D_OK);
	
	// Set the projection matrix in the shader
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
	
	// Set the view matrix
	mResult = mDevice->SetTransform(D3DTS_VIEW, &matrix);
	assert(mResult == D3D_OK);
	
	// Set the view matrix in the shader
	mResult = mEffect->SetMatrix("gViewMat", &matrix);
	assert(mResult == D3D_OK);
}

// Sets the world matrix to the matrix passed in
void CD3DObj::setWorldMatrix(const D3DXMATRIX *matrix)
{
	assert(mEffect != NULL);
	
	// Set the world matrix
	mResult = mDevice->SetTransform(D3DTS_WORLD, matrix);
	assert(mResult == D3D_OK);
	
	// Set the world matrix in the shader
	mResult = mEffect->SetMatrix("gWorldMat", matrix);
	assert(mResult == D3D_OK);
}

// Clears the viewport to a specified ARGB color
bool CD3DObj::clear(int color, float zDepth, DWORD stencil)
{
	// Set the flag for everything we want to clear
	int flag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

	// Clear the color, z-buffer, and stencil buffer
	mResult = mDevice->Clear(0, NULL, flag, color, zDepth, stencil);
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

// *** Private Methods *** //

bool CD3DObj::createDevice(bool canDebug)
{
	assert(mInterface != NULL);
	assert(mDevice == NULL); // Should be destroyed before reallocated

	D3DPRESENT_PARAMETERS params = {0}; // Presentation parameters to be filled

	// Fill the presentation parameters
	setPresentationParams(params);
	
	if(canDebug)
	{
		// Create a device that is capable of debugging
		mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, mHWND,
									       D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &mDevice);
	}
	else // Make a normal hardware accelerated device
	{
		mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mHWND,
										   D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &mDevice);
										   
		if(mResult != D3D_OK) // If hardware vertex processing isn't supported...
		{
			// Try software vertex processing
			mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mHWND,
											   D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &mDevice);
		}
	}
	
	return (mResult == D3D_OK); // Return result of device creation
}

// Fill the D3DPRESENT_PARAMETERS with the correct parameters based 
// on windowed or full screen mode
void CD3DObj::setPresentationParams(D3DPRESENT_PARAMETERS &params)
{
	// Zero out the params struct 
	ZeroMemory(&params, sizeof(D3DPRESENT_PARAMETERS));

	// Set all parameters that can be shared between windowed and full screen mode
	params.hDeviceWindow = mHWND; // Handle to window
	params.BackBufferCount = 1; // Number of back buffers
	params.SwapEffect = D3DSWAPEFFECT_FLIP; // We want to treat the back buffers as a circular
											// queue and rotate through them
	params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Draw everything immediately, don't
																// wait for V-sync
	params.EnableAutoDepthStencil = TRUE; // We want a Z-buffer and stencil buffer
	params.AutoDepthStencilFormat = D3DFMT_D24S8; // 24-bit Z-buffer, 8-bit stencil buffer

	if(mFullScreen)
	{
		D3DDISPLAYMODE dispMode = {0};

		// Get the current configuration of the primary monitor
		mResult = mInterface->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);
		assert(mResult == D3D_OK);

		// Fill in the rest of the parameters with the primary display device's parameters
		params.Windowed = FALSE;
		params.BackBufferWidth = dispMode.Width;
		params.BackBufferHeight = dispMode.Height;
		params.FullScreen_RefreshRateInHz = dispMode.RefreshRate;
		params.BackBufferFormat = dispMode.Format; // Use the current color depth of the monitor	
	}
	else
	{
		// Get client rect
		RECT rect = {0};
		GetClientRect(mHWND, &rect);

		params.Windowed = true; // We want a window
		params.BackBufferWidth = rect.right; // Backbuffer's width equals client rect's width
		params.BackBufferHeight = rect.bottom; // Backbuffer's height equals client rect's height
		params.BackBufferFormat = D3DFMT_UNKNOWN; // Use whatever the current color depth of the
												 // monitor is for the back buffer
	}
	
	// Calculate the aspect ratio
	mAspectRatio = (float)params.BackBufferWidth / (float)params.BackBufferHeight;
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object