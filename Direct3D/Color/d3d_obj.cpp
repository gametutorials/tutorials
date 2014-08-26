#include "d3d_obj.h"

// Declare our static variables
// So why have "mInterface" and "mDevice" be public?  Well we are going to want to
// access those variables by other classes (say for instance if we add a D3D texture class),
// so we keep them public so when we build on this framework, we can access them easily
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
										  // D3DSWAPEFFECT_FLIP flag what will happen is drawing
										 // will occur to our current back buffer, which we 
										// choose to only have one.  Once rendering is completed, the
									   // buffer is flipped with the current frame buffer and becomes
									  // the frame buffer, while the current frame buffer becomes
									 // the back buffer.  Flipping is really fast, because it avoids
									// copying the back buffer to the front buffer  						  
    params.BackBufferFormat = D3DFMT_UNKNOWN; // Here we set what we want our D3DFORMAT to be.
											 // An example format would be D3DFMT_R8G8B8 which
											// is a 24-bit RGB format.  However we choose 
										   // D3DFMT_UNKNOWN so that when the device is created
										  // the format that will be selected is the current
										 // desktop display format.

	
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

	// This presents (draws) the next back buffer (we only have one in our case) to the screen
	// By parameter:
	// NULL -- Must be NULL unless a swap chain was created with D3DSWAPEFFECT_COPY
	//		   **NOTE** We used D3DSWAPEFFECT_DISCARD
	// NULL -- Must be NULL unless a swap chain was created with D3DSWAPEFFECT_COPY
	// NULL -- HWND whose client area is taken as the target for this presentation.  If this is
	//		   NULL then the HWND set in the D3DPRESENT_PARAMETERS (set in our init() function)
	//		   is used
	// NULL -- Must be NULL unless a swap chain was created with D3DSWAPEFFECT_COPY 
	mResult = mDevice->Present(NULL, NULL, NULL, NULL);
	assert(mResult == D3D_OK);
}

// This renders a list of SVertex type vertices that is "numVerts" long
bool CD3DObj::render(SVertex *vertList, int numVerts)
{
	// The first thing we have to do before rendering is tell D3D what
	// type of vertices we'll be rendering.  We do this by passing the
	// #define that stipulates our vertex type.
	mDevice->SetFVF(SVertexType);
	
	// Now we are ready to render.  We can do this with one function call.
	// You'll notice the "UP" in the function name.  This stands for "user pointer" and 
	// means that we are passing D3D a pointer to a list of vertices to be rendered.  
	// Optionally we could use a D3D vertex buffer, this is a more specialized class that
	// increases rendering speeds.

	// This function takes vertex arrays and index arrays (declared in typical C++ fashion)
	// and renders them to the screen.  By parameter:
	// D3DPT_TRIANGLELIST -- The type of primitive to render
	// numVerts / 3 -- Number of primitives (in our case triangles) to render
	// iList -- Pointer to index array
	// vertList -- Pointer to our vertex array
	// vList -- Point to the vertex array
	// sizeof(SVertex) -- Size of an individual vertex
	mResult = mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, numVerts / 3, vertList, sizeof(SVertex));
		return (mResult == D3D_OK); // Returns status of DrawPrimitiveUP()
}

// Clears the viewport to a specified ARGB color
bool CD3DObj::clearColor(int color)
{
	// This clears our viewport to the color passed in.
	// By parameter:
	// 0 -- Number of RECT(s) passed in the second parameter.  Since we pass NULL
	//		this MUST be 0
	// NULL -- An array of D3DRECT(s) that specify the area(s) to be cleared in the
	//		   viewport.  By passing NULL we're saying "We want to clear the entire viewport"
	// D3DCLEAR_TARGET -- This is flag that says what exactly we're clearing in the viewport
	//					  it can be a combination of these three flags:
	//					  D3DCLEAR_STENCIL -- This clears the stencil buffer
	//					  D3DCLEAR_TARGET -- This clears the render targets color
	//					  D3DCLEAR_ZBUFFER -- This clears the depth buffer (Z-Buffer)
	// color -- The ARGB color to clear the render target to
	// 1.0f -- The value to clear the depth buffer to
	//		   **NOTE** Since we didn't specify that flag this parameter is ignored
	// 0 --	The value to clear the stencil buffer to
	//		**NOTE** Since we didn't specify that flag this parameter is ignored
	mResult = mDevice->Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);
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
