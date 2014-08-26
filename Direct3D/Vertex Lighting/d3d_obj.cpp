#include "d3d_obj.h"

// Declare our static variables
IDirect3D9* CD3DObj::mInterface = NULL;
IDirect3DDevice9* CD3DObj::mDevice = NULL;
HRESULT CD3DObj::mResult = 0;
bool CD3DObj::mFullScreen = false; // Start out windowed
HWND CD3DObj::mHWND = NULL;
float CD3DObj::mFOV = 0.0f; // Field of view
float CD3DObj::mNearClip = 0.0f; // Near clip plane
float CD3DObj::mFarClip = 0.0f; // Far clip plane
float CD3DObj::mAspectRatio = 0.0f; // Width / Height of display screen
int CD3DObj::mMaxLights = 0; // Maximum number of lights for the display device
D3DLIGHT9* CD3DObj::mLights = NULL; // Pointer to an array of D3D lights

// Initializes our D3D object -- Returns true on success, false otherwise
bool CD3DObj::init(HWND hwnd)
{
	if(!hwnd)
		return false;

	mHWND = hwnd; // Store window handle

	// Create the D3D object, which is needed to create the D3DDevice.
	mInterface = Direct3DCreate9(D3D_SDK_VERSION);

	// Error Check
	if(mInterface == NULL)
		return false;

	// Create the D3D device
	if(!createDevice())
		return false;
		
	// Init the lights
	if(!initLights())
		return false;

	// Setup default render states	
	setDefaultRenderState();
		return true; // We got loaded!
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

	// Present the current buffer (we have two in our case) to the screen
	mResult = mDevice->Present(NULL, NULL, NULL, NULL);
	assert(mResult == D3D_OK);
}

// Set all the render states to a default value
void CD3DObj::setDefaultRenderState()
{
	assert(mDevice != NULL);

	// Turn on the z-buffer
	mResult = mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	assert(mResult == D3D_OK);
}

// Sets the type of light for "which" light
void CD3DObj::setLightType(int which, D3DLIGHTTYPE type)
{
	assert(which >= 0 && which < mMaxLights);

	mLights[which].Type = type;
}

// Sets the world position of "which" light
void CD3DObj::setLightPos(int which, const CPos &pos)
{
	assert(which >= 0 && which < mMaxLights);

	mLights[which].Position.x = pos.x;
	mLights[which].Position.y = pos.y;
	mLights[which].Position.z = pos.z;
}

// Sets the direction "which" light is shinning
void CD3DObj::setLightDir(int which, const CVector &dir)
{
	assert(which >= 0 && which < mMaxLights);

	mLights[which].Direction.x = dir.x;
	mLights[which].Direction.y = dir.y;
	mLights[which].Direction.z = dir.z;
}

// Sets "which" light three color values
// D3DLIGHT9 color values are between (0.0f, 1.0f)
void CD3DObj::setLightColor(int which, int diffuse, int specular, int ambient)
{
	assert(which >= 0 && which < mMaxLights);

	mLights[which].Diffuse.a = GET_A(diffuse) / 255.0f;
	mLights[which].Diffuse.b = GET_B(diffuse) / 255.0f;
	mLights[which].Diffuse.g = GET_G(diffuse) / 255.0f;
	mLights[which].Diffuse.r = GET_R(diffuse) / 255.0f;

	mLights[which].Specular.a = GET_A(specular) / 255.0f;
	mLights[which].Specular.b = GET_B(specular) / 255.0f;
	mLights[which].Specular.g = GET_G(specular) / 255.0f;
	mLights[which].Specular.r = GET_R(specular) / 255.0f;

	mLights[which].Ambient.a = GET_A(ambient) / 255.0f;
	mLights[which].Ambient.b = GET_B(ambient) / 255.0f;
	mLights[which].Ambient.g = GET_G(ambient) / 255.0f;
	mLights[which].Ambient.r = GET_R(ambient) / 255.0f;
}

// Sets "which" light's inner and outer radius that define
// it's intensity.  "innerCone" is the angle in radians of where the full intensity of the
// light will hit.  It must in the range of (0.0f, outerCone).  "outerCone" is the angle
// in radians of the complete area the light will illuminate.  It must be in the
// range of (0.0f, PI) 
void CD3DObj::setLightRadius(int which, float innerCone, float outerCone)
{
	assert(which >= 0 && which < mMaxLights);
	assert(innerCone >= 0.0f && innerCone <= outerCone);
	assert(outerCone >= 0.0f && outerCone <= D3DX_PI);	

	mLights[which].Theta = innerCone;
	mLights[which].Phi = outerCone;
}

// Sets the status (either on or off) of "which" light
void CD3DObj::setLightActive(int which, bool onOrOff)
{
	assert(which >= 0 && which < mMaxLights);

	// Tell the device about the light 
	mResult = mDevice->SetLight(which, &mLights[which]);
	assert(mResult == D3D_OK);

	// Either turn it on or off
	mResult = mDevice->LightEnable(which, onOrOff);
	assert(mResult == D3D_OK);
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

	// Reset the device to the new parameters
	if(mDevice->Reset(&params) != D3D_OK)
	{
		// Put flag back
		mFullScreen = !mFullScreen;
		return false;
	}

	// If we've changed to windowed mode
	if(mFullScreen == false)
	{
		// Move and resize window
		SetWindowPos(mHWND, HWND_NOTOPMOST, winRect.left, winRect.top, winRect.right - winRect.left,
					 winRect.bottom - winRect.top, SWP_SHOWWINDOW);
	}

	// Calculate the new aspect ratio
	float aspectRatio = (float)params.BackBufferWidth / (float)params.BackBufferHeight;

	// We must reset the render states
	setDefaultRenderState();
	setProjMatrix(mFOV, aspectRatio, mNearClip, mFarClip);
	return true;
}

// This renders a list of SVertex type vertices that is "numVerts" long
bool CD3DObj::render(SVertex *vertList, int numVerts)
{
	// We're rendering triangles so the number of verts should evenly be divisible by 3
	assert(numVerts % 3 == 0); 

	// Set the flexible vertex format
	mDevice->SetFVF(SVertexType);

	// Draw the vertex list
	mResult = mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, numVerts / 3, vertList, sizeof(SVertex));
	return (mResult == D3D_OK);
}

// Renders a indexed list of SVertex as triangles
bool CD3DObj::render(SVertex *vertList, int numVerts, WORD *indexList, int numIndices)
{
	// We're rendering triangles so the number of indices should evenly be divisible by 3
	assert(numIndices % 3 == 0); 

	// Set the flexible vertex format
	mDevice->SetFVF(SVertexType);

	// Draw the indexed vertex list
	mResult = mDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, numVerts, numIndices / 3,
											  indexList, D3DFMT_INDEX16, vertList, sizeof(SVertex));
	return (mResult == D3D_OK);
}

bool CD3DObj::renderLine(SVertex *vertList, int numVerts)
{
	// We're rendering line segments so there should  be an even number of vertices
	assert(numVerts % 2 == 0); 

	// Set the FVF 
	mDevice->SetFVF(SVertexType);

	// Render the line segments
	mResult = mDevice->DrawPrimitiveUP(D3DPT_LINELIST, numVerts / 2, vertList, sizeof(SVertex));
		return (mResult == D3D_OK);
}

// Renders a list of SVertex as a triangle strip
bool CD3DObj::renderTriStrip(SVertex *vertList, int numVerts)
{
	// Set the FVF 
	mDevice->SetFVF(SVertexType);

	// Render the line segments
	mResult = mDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, numVerts - 2, vertList, sizeof(SVertex));
		return (mResult == D3D_OK);
}

// Sets up the view of the scene based on passed in eye and target
void CD3DObj::setViewMatrix(const CPos &eye, const CPos &lookAt)
{
	D3DXMATRIXA16 matrix;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // World's up vector

	// Create and set the view matrix
	D3DXMatrixLookAtLH(&matrix, (D3DXVECTOR3*)(&eye), (D3DXVECTOR3*)(&lookAt), &up);
	mDevice->SetTransform(D3DTS_VIEW, &matrix); // Set our view of the world
}

// Sets the projection matrix, which dictates how our 3D scene is projected onto our 2D monitor
// using the passed in field of view, aspect ratio, near clip plane, and far clip plane
void CD3DObj::setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip)
{
	D3DXMATRIXA16 matrix;

	mFOV = fov;
	mAspectRatio = aspectRatio;
	mNearClip = nearClip;
	mFarClip = farClip;

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
	// This clears our viewport to the color and zDepth passed in
	mResult = mDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, zDepth, 0);
	return (mResult == D3D_OK);
}

// Free up all the memory
void CD3DObj::deinit()
{
	if(mLights)
		delete[] mLights;

	if(mDevice != NULL)
		mDevice->Release();

	if(mInterface != NULL)
		mInterface->Release();

	// Zero out our D3D interface, device, and light pointer 
	mDevice = NULL;
	mInterface = NULL;
	mLights = NULL;

	// Zero out window handle
	mHWND = NULL;
}

////////////////////////////////
// *** Private Functions *** //
//////////////////////////////

bool CD3DObj::createDevice()
{
	assert(mInterface != NULL);
	assert(mDevice == NULL); // Should be destroyed before reallocated

	D3DPRESENT_PARAMETERS params = {0}; // Presentation parameters to be filled

	// Fill the presentation parameters
	setPresentationParams(params);

	// Set the Aspect Ratio
	mAspectRatio = (float)params.BackBufferWidth / (float)params.BackBufferHeight;

	mResult = mInterface->CreateDevice(D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										mHWND,
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
											mHWND,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											&params,
											&mDevice);

		if(mResult != D3D_OK)
			return false; // Couldn't create a D3D 9.0 device
	}

	return true; // Device got loaded
}

bool CD3DObj::initLights()
{
	D3DCAPS9 caps; // Struct to hold the device's capabilites

	// First we need to get the device's capabilites so we can determine
	// how many lights we can have
	if(mInterface->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps) != D3D_OK)
		return false;

	// Store the maximum number of lights
	mMaxLights = caps.MaxActiveLights;

	// Now we'll create an array of lights that is as big as 
	// the maximum number of lights we could possibly have
	mLights = new D3DLIGHT9[mMaxLights];

		// Error Check
		if(!mLights) return false;
		
	// Zero out the light structs
	ZeroMemory(mLights, sizeof(D3DLIGHT9) * mMaxLights);
		
	// Init each light struct to default values
	for(int i = 0; i < mMaxLights; ++i)
	{
		mLights[i].Type = D3DLIGHT_POINT;
		mLights[i].Attenuation1 = 1.0f;
		mLights[i].Falloff = 1.0f;
		mLights[i].Range = 255.0f;
		mLights[i].Diffuse.a = 1.0f; // Set diffuse color to solid white with full alpha
		mLights[i].Diffuse.b = 1.0f;
		mLights[i].Diffuse.g = 1.0f;
		mLights[i].Diffuse.r = 1.0f;
		mLights[i].Specular = mLights[i].Diffuse; // Specular is the same as diffuse color
		mLights[i].Phi = DEG2RAD(180);
		mLights[i].Theta = DEG2RAD(45);
		mLights[i].Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}
	
	return true; // Lights initialized properly
}

// Fill the D3DPRESENT_PARAMETERS with the correct parameters based 
// on windowed or full screen mode
void CD3DObj::setPresentationParams(D3DPRESENT_PARAMETERS &params)
{
	// Zero out the params struct 
	ZeroMemory(&params, sizeof(D3DPRESENT_PARAMETERS));

	// Set all parameters that can be shared between windowed and full screen mode
	params.hDeviceWindow = mHWND; // Handle to window
	params.BackBufferCount = 1; // Number of render surfaces
	params.SwapEffect = D3DSWAPEFFECT_FLIP; // We want to treat the back buffers as a circular
											// queue and rotate through them
	params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Draw everything immediately, don't
																// wait for V-sync
	params.EnableAutoDepthStencil = TRUE; // We want a Z-buffer
	params.AutoDepthStencilFormat = D3DFMT_D16; // 16-bit Z-buffer (should be safe on most cards...)

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
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object
