#include "d3d_obj.h"

// Declare our static variables
IDirect3D9* CD3DObj::mInterface = NULL;
IDirect3DDevice9* CD3DObj::mDevice = NULL;
HRESULT CD3DObj::mResult = 0;

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
	
	// There's a bunch of parameters we could set up, however for this tutorial we
	// only care about a few.
	params.Windowed = TRUE; // This parameter says if the application runs in a window (TRUE)
							// or is full screen (FALSE)
	params.BackBufferCount = 1; // The number of back buffers for the application
	params.SwapEffect = D3DSWAPEFFECT_FLIP; // The swap effect flag handles how the D3D application
										   // interacts with the back buffers.  By specifying the
										  // D3DSWAPEFFECT_FLIP flag what will happen is rendering
										 // will occur to our current back buffer, which we 
										// choose to only have one.  Once rendering is completed, the
									   // buffer is flipped with the current frame buffer and becomes
									  // the frame buffer, while the current frame buffer becomes
									 // the back buffer.  Flipping is really fast, because it avoids
									// copying the back buffer to the front buffer.								  
	params.BackBufferFormat = D3DFMT_UNKNOWN; // Here we set what we want our D3DFORMAT to be.
											 // An example format would be D3DFMT_R8G8B8 which
											// is a 24-bit RGB format.  However we choose 
											// D3DFMT_UNKNOWN so that when the device is created
											// the format that will be selected is the current
											// desktop display format.
	params.EnableAutoDepthStencil = true; // This says "Create a Z-buffer/Stencil buffer for us"
										 // **NOTE** If we want an application that has a z-buffer
										// we're gonna want to do this
	params.AutoDepthStencilFormat = D3DFMT_D16; // Here we select what type of Z-buffer/Stencil 
												// buffer we want.  D3DFMT_16 says, "We want a 
												// 16-bit Z-buffer".  This should be supported by the
												// greatest number of video cards.

	// All right we're done setting up our device parameters.  Time to make the
	// device.  So by parameter:
	// D3DADAPTER_DEFAULT -- Specifies which adapter to use.  D3DADAPTER_DEFAULT means always
	//						 use the primary adapter.
	// D3DDEVTYPE_HAL -- This is the D3DDEVTYPE.  It specifies what the device type will
	//       			 be.  D3DDEVTYPE_HAL says the device will use hardware rasterization.
	// hwnd -- This is the HWND to the window that the focus of the device belongs to.  Basically
	//		   this answers the question "Which window owns this device?"
	// D3DCREATE_HARDWARE_VERTEXPROCESSING -- This is a D3DCREATE flag.  It controls the behavior
	//										  of the device.  This specific flag says, "Do the
	//										  vertex processing in hardware."
	// &params -- This is the address of the devices "presentation parameters".  It dictates how
	//			  the device will present itself to the screen.  So such things as back buffers,
	//			  refresh rate, and stencil format can be specified in this struct.
	// &mDevice -- This is the address of a pointer to a IDirect3DDevice9.  This will get 
	//				  filled in with the created device.
	mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										hwnd,
										D3DCREATE_HARDWARE_VERTEXPROCESSING,
										&params,
										&mDevice);

	// It's possible we'll get an error because not all vid-cards can handle vertex processing
	// So in the event we do get an error we'll try to make the device again.  We will
	// only change D3DCREATE_HARDWARE_VERTEXPROCESSING to D3DCREATE_SOFTWARE_VERTEXPROCESSING
	// which says, "Do the vertex processing in software" 
	if(mResult != D3D_OK)
	{
		mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
											D3DDEVTYPE_HAL,
											hwnd,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											&params,
											&mDevice);
		if(mResult != D3D_OK)
			return false; // Couldn't create a D3D 9.0 device
	}
	
	 // Turn off back face culling so our geometry is always visible
	 mResult = mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	 assert(mResult == D3D_OK);
	
	 // Turn off D3D lighting, since we are providing our own vertex colors
	 mResult = mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	 assert(mResult == D3D_OK);
	    
	 // Turn on Z-buffering
	 mResult = mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	 assert(mResult == D3D_OK);
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

// This renders a list of SVertex type vertices that is "numVerts" long
bool CD3DObj::render(SVertex *vertList, int numVerts)
{
	// Set the flexible vertex format
	mDevice->SetFVF(SVertexType);

	// Render the list of vertices as triangles
	mResult = mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, numVerts / 3, vertList, sizeof(SVertex));
		return (mResult == D3D_OK);
}

bool CD3DObj::renderLine(SVertex *vertList, int numVerts)
{
	assert(numVerts % 2 == 0); // We're rendering line segments so there should 
							  // be an even number of vertices
							  
	// Set the FVF
	// The FVF is a flag which defines what data is stored in the vertex
	mDevice->SetFVF(SVertexType);

	// Render the line segments
	mResult = mDevice->DrawPrimitiveUP(D3DPT_LINELIST, numVerts / 2, vertList, sizeof(SVertex));
		return (mResult == D3D_OK);
}

// Sets up the view of the scene based on passed in eye and target
void CD3DObj::setViewMatrix(const CPos &eye, const CPos &lookAt)
{
	D3DXMATRIXA16 matrix;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // World's up vector

	// Create and set the view matrix
	// **NOTE** We can cast to (D3DXVECTOR3*) because our CVector has it's data members (x,y,z)
	// declared the same way a D3DXVECTOR3 does
	D3DXMatrixLookAtLH(&matrix, (D3DXVECTOR3*)(&eye), (D3DXVECTOR3*)(&lookAt), &up);
    mDevice->SetTransform(D3DTS_VIEW, &matrix); // Set our view of the world
}

// Sets the projection matrix, which dictates how our 3D scene is projected onto our 2D monitor
// using the passed in field of view, aspect ratio, near clip plane, and far clip plane
void CD3DObj::setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip)
{
	D3DXMATRIXA16 matrix;

	// Create and set projection matrix
    D3DXMatrixPerspectiveFovLH(&matrix, fov, aspectRatio, nearClip, farClip);
    mDevice->SetTransform(D3DTS_PROJECTION, &matrix);
}

// Sets the world matrix to the matrix passed in
void CD3DObj::setWorldMatrix(const D3DXMATRIX *matrix)
{
	mDevice->SetTransform(D3DTS_WORLD, matrix);
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

	// Zero out our D3D interface and device 
	mDevice = NULL;
	mInterface = NULL;
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object
