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
	
	params.Windowed = TRUE; // The application is windowed
	params.BackBufferCount = 1; // We want one back buffer for the application
	params.SwapEffect = D3DSWAPEFFECT_FLIP; // Each frame flip between the back buffer and frame buffer						  
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
	
	// Set default states
	mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); // Turn on Z-buffer
	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // Turn off vertex lighting
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
void CD3DObj::setProjMatrix(float fov, float aspectRatio, float nearClip, float farClip)
{
	// Create the projection matrix
	D3DXMATRIXA16 matrix;
	D3DXMatrixPerspectiveFovLH(&matrix, fov, aspectRatio, nearClip, farClip);
	
	// Set the projection matrix
	mResult = mDevice->SetTransform(D3DTS_PROJECTION, &matrix);
	assert(mResult == D3D_OK);
}

// Sets up the view of the scene based on passed in eye and target
void CD3DObj::setViewMatrix(const CPos &eye, const CPos &lookAt)
{
	D3DXMATRIXA16 matrix;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f); // World's up vector

	// Create the view matrix
	// **NOTE** We can cast to (D3DXVECTOR3*) because our CPos has it's data members (x,y,z)
	// declared the same way a D3DXVECTOR3 does
	D3DXMatrixLookAtLH(&matrix, (D3DXVECTOR3*)(&eye), (D3DXVECTOR3*)(&lookAt), &up);
	
	// Set the view matrix
	mResult = mDevice->SetTransform(D3DTS_VIEW, &matrix);
	assert(mResult == D3D_OK);
}

// Sets the world matrix to the matrix passed in
void CD3DObj::setWorldMatrix(const D3DXMATRIX *matrix)
{
	// Set the world matrix
	mResult = mDevice->SetTransform(D3DTS_WORLD, matrix);
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
		
	// Zero out our D3D interface, device, and effect
	mDevice = NULL;
	mInterface = NULL;
}

CD3DObj theD3DObj; // Create our 3D Object
CD3DObj *g3D = &theD3DObj; // Set the global pointer to our 3D Object
