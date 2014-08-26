// Done by TheTutor

/*	
	!*WARNING*!	It is assumed that you have a decent understanding of 3D programming and
				vectors, matrices, and linear algebra.  If not read the earlier GameTutorials
				lessons to get caught up :)

	We are in the "next-generation" phase and this means Xbox 360, PS3, Vista, and the
	impending death of the fixed-function pipeline.  Vertex and pixel shaders are here to stay!
	
	Coming with them is the "norm" technique for rendering, affectionately called 
	"normal mapping".  Normal mapping is a multiple texture technique in which at least
	two texture maps are used.  One map, the diffuse map, represents the object's color.
	The second map, the normal map, represents the normals (facing outward) along the
	polygons of the object.

	This tutorial demonstrates this by creating and rendering a normal mapped cube.  A simple
	camera is hooked up and you can toggle normal mapping on/off with F1 and toggle the
	rendering of tangent space on/off with F2.

	So lets take a high level look at what normal mapping is all about...

	Normal mapping allows you to have normal data avaiable on a per pixel level.  Lets just
	talk about the rendering of one triangle for conversation sake.  Typically, you can only
	get the normal data for the triangle from one of the three vertex normals.  While you
	can interpolate these acorss the face of the triangle, you are not going to get
	a ton of variation in the direction of normals.  What normal mapping does is allow you 
	to get a unique normal for each pixel rendered.  This normal is pulled from the normal 
	map where each color in the normal map represents a 3D vector.  That normal is then used 
	in your favorite lighting equation, and batta-boom, batta-bing, you have the illusion 
	of depth.

	Now lets jump into the fray...
*/

// >>> INPUT CONTROLS <<< //
/*
	F1 = Toggle normal map on/off
	F2 = Toggle rendering of tangent space on/off
*/

#include <windows.h>
#include "d3d_obj.h"
#include "camera.h"
#include "d3d_texture.h"

// Add the following libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

/////////////
// Macros //
///////////

#define DEG2RAD(x) (x * (D3DX_PI / 180.0f)) // Converts degrees to radians
#define RAD2DEG(x) (x * (180.0f / D3DX_PI)) // Converts radians to degrees

////////////////
// Constants //
//////////////

// Width and height of the window (specifically the client rect)
const int kWinWid = 800;
const int kWinHgt = 600;

const char kClassName[] = "GT_D3DNormalMapping"; // WNDCLASSEX's name

const char kTextureName[] = "brickdiff.bmp";
const char kNormalMapName[] = "brick_NORM.bmp";

const CColor kDiffuse(1.0f, 1.0f, 1.0f); // Diffuse color of light
const CColor kSpecular(1.0f, 1.0f, 1.0f); // Specular color of light
const CColor kAmbient(0.1f, 0.1f, 0.1f); // Ambient color of light

const int kVertCount = 24;
const int kIndexCount = 36;
const int kElementCount = 6;

// **NOTE** Labeling assumes we're looking directly at the cube with the front side
//			facing us 
const SPosUV kCubeVerts[kVertCount] =
{
	// Front side
	{ -1.0f, -1.0f, -1.0f, 0.0f, 1.0f }, 
	{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, 
	{ 1.0f, -1.0f, -1.0f, 1.0f, 1.0f}, 
	{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f}, 

	// Right Side
	{ 1.0f, -1.0f, -1.0f, 0.0f, 1.0f },   
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f }, 
	{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f}, 
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, 

	// Back Side
	{ 1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, 
	{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, 
	{ -1.0f, -1.0f, 1.0f, 1.0f, 1.0f}, 
	{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, 

	// Left Side
	{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, 
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, 
	{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f}, 
	{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f}, 

	// Top Side
	{ -1.0f, 1.0f, -1.0f, 0.0f, 1.0f }, 
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f }, 
	{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f}, 
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, 

	// Bottom Side
	{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }, 
	{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f },
	{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f},  
	{ 1.0f, -1.0f, -1.0f,1.0f, 0.0f}, 
};

// Indexes for our box
const WORD kIndexList[kIndexCount] = 
{	
	0, 1, 2, // Front Side Triangle 1
	2, 1, 3, // Front Side Triangle 2
	4, 5, 6, // Right Side Triangle 1
	6, 5, 7, // Right Side Triangle 2
	8, 9, 10, // Back Side Triangle 1
	10, 9, 11, // Back Side Triangle 2
	12, 13, 14, // Left Side Triangle 1
	14, 13, 15, // Left Side Triangle 2
	16, 17, 18, // Top Side Triangle 1
	18, 17, 19, // Top Side Triangle 2
	20, 21, 22, // Bottom Side Triangle 1
	22, 21, 23 // Bottom Side Triangle 2
};

//////////////
// Globals //
////////////

ID3DXMesh *gLight = NULL; // Our D3D mesh object representing where the light is
CPos gLightPos(0.0f, 1.0f, -3.0f); // The lights position in world space

// The two texture maps for the cube
CD3DTexture gDiffuseMap;
CD3DTexture gNormalMap;

// Vertices holding all of the normal mapped information for the cube
SVertex gVerts[kVertCount];

bool gDrawTangentSpace = false; // If true, axes of tangent space are rendered

bool LockFrameRate(int frameRate = 60); // Locks the frame rate

// Helper function for setting a D3DVERTEXELEMENT9
void SetElement(D3DVERTEXELEMENT9 &e, WORD stream, WORD offset, BYTE type,
				BYTE method, BYTE usage, BYTE usageIndex);

// Creates a sphere stored in a D3D mesh object
bool CreateSphere(float radius, int color, ID3DXMesh **mesh);

// Creates a box with normal mapping
bool CreateBox(SMesh *box, float width, float height, float depth);

// Calculates left-handed tangent space
void CalculateTangentSpace(SVertex &v1, const SVertex &v2, const SVertex &v3);

// Draws the X, Y, and Z axis of a tangent space basis for each vertices on the box
void RenderTangentSpace();

// Standard WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd = NULL;
	MSG msg = {0};
    WNDCLASSEX wndclassex = {0};

	// Init fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX); // Always set to size of WNDCLASSEX
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclassex.lpszClassName = kClassName;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);
    RegisterClassEx(&wndclassex);

	RECT rect = { 0, 0, kWinWid, kWinHgt }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_CLIENTEDGE;
	DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

    hwnd = CreateWindowEx(winStyleEx, // Window extended style
					      kClassName,
						  "www.GameTutorials.com -- Normal Mapping",
						  winStyle, // Window style
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left,
						  rect.bottom - rect.top,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);
						  
	// Get the client rect and make sure our client is the size we want
	GetClientRect(hwnd, &rect);
	assert(rect.right == kWinWid && rect.bottom == kWinHgt);

	// Init our global 3D object
	if(g3D->init(hwnd) == false)
		return EXIT_FAILURE; // There's been an error, lets get out of this joint
		
	// Load our textures
	if((gDiffuseMap.load(kTextureName) == false) || (gNormalMap.load(kNormalMapName) == false))
		return false; // Couldn't load textures
	
	// Create the small white sphere to represent the light
	if(!CreateSphere(0.025f, D3DCOLOR_XRGB(255,255,255), &gLight))
		return false; // Couldn't create the sphere... Something very bad happened...
	
	SMesh box = {0};
	
	// Create the normal mapped box vertex data	
	if(!CreateBox(&box, 1.0f, 1.0f, 1.0f))
		return false; // Couldn't create the box so we have nothing to normal map
		
	// Set up our projection matrix once because it will not change
	g3D->setProjMatrix(DEG2RAD(60), (float)kWinWid / (float)kWinHgt, 0.1f, 8192.0f);
	
	// Set data for our light that will never change
	g3D->mEffect->SetFloatArray("gDiffuse", &kDiffuse.r, 3); // Lights diffuse color
	g3D->mEffect->SetFloatArray("gSpecular", &kSpecular.r, 3); // Lights specular color
	g3D->mEffect->SetFloatArray("gAmbient", &kAmbient.r, 3); // Lights ambient color
	g3D->mEffect->SetFloatArray("gLightPos", &gLightPos.x, 3); // Lights position
	
	// Initially use the normal map
	g3D->mEffect->SetBool("gUseNormalMap", true);

	ShowWindow(hwnd, ishow);
	UpdateWindow(hwnd);
	
	float angle = 0.0f;
	float angleInc = 0.5f;

    while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // If the OS has a message for us
		{
			if(msg.message == WM_QUIT)
				break;
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate()) // Else, if it's time to draw
		{
			D3DXMATRIX wMat;
			
			CPos eye = gCamera->getEye();
			g3D->mEffect->SetFloatArray("gEyePos", &eye.x, 3); // Set the camera's eye position
			
			// Set the view matrix based on the camera
			g3D->setViewMatrix(gCamera->getEye(), gCamera->getTarget());
			
			g3D->begin();
			g3D->clear(D3DCOLOR_XRGB(0, 0, 16)); // Clear to dark blue
			
			// Draw the cube, centered at (0,0,0) as it rotates around the Y-axis
			D3DXMatrixRotationY(&wMat, DEG2RAD(angle));
			g3D->setWorldMatrix(&wMat);
			
			if(gDrawTangentSpace) // If true, render tangent space on one side of the cube
				RenderTangentSpace();
			
			// Render the normal mapped cube
			gDiffuseMap.selectAsDiffuseMap();
			gNormalMap.selectAsNormalMap();
			g3D->render("NormalMapping", &box);
			
			// Draw the tiny sphere representing the light 
			D3DXMatrixTranslation(&wMat, gLightPos.x, gLightPos.y, gLightPos.z);
			g3D->setWorldMatrix(&wMat);
			g3D->render(gLight); 
			
			angle += angleInc;
			g3D->end();
		}
		else
			Sleep(1); // Otherwise, give the OS some time to process other programs
	
	} // end of while(1)

	// Free up the cube
	box.vertexDeclaration->Release();
	box.vertexBuffer->Release();
	box.indexBuffer->Release();
	
	gLight->Release(); // Free up the light
		
	g3D->deinit(); // Free up the D3D object
	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
	    return EXIT_SUCCESS; // Application was a success
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
    {
		case WM_KEYDOWN:
		
			switch(wparam)
			{
				case VK_ESCAPE: // If the user presses ESC, close the app
					SendMessage(hwnd, WM_CLOSE, 0, 0);
						break;

				case VK_F1: // Toggle normal mapping on/off
				{
					BOOL value;
					g3D->mEffect->GetBool("gUseNormalMap", &value);
					g3D->mEffect->SetBool("gUseNormalMap", !value);
						break;
				}

				case VK_F2: // Toggle drawing of tangent space on/off
					gDrawTangentSpace = !gDrawTangentSpace;
						break;

				case VK_UP:
					gCamera->move(CCamera::eForward, 0.1f);
						break;
						
				case VK_DOWN:
					gCamera->move(CCamera::eBack, 0.1f);
						break;
						
				case VK_LEFT:
					gCamera->move(CCamera::eLeft, 0.1f);
						break;
						
				case VK_RIGHT:
					gCamera->move(CCamera::eRight, 0.1f);
						break;
			}						
							
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
				return 0;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Locks the frame rate at "frameRate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int frameRate)
{
	static float lastTime = 0.0f;

	// Get current time in seconds (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	// If the desired frame rate amount of seconds has passed true
	if((currentTime - lastTime) > (1.0f / frameRate))
	{
		// Reset the last time
		lastTime = currentTime;	
		return true;
	}

	return false;
}

// Creates a sphere.  Returns true on success, false otherwise
bool CreateSphere(float radius, int color, ID3DXMesh **mesh)
{
	assert(g3D->mDevice != NULL);
	assert(mesh != NULL);
	
	const unsigned int kSlices = 12;
	const unsigned int kStacks = 12;
	
	ID3DXMesh *tempMesh; // Temp D3D mesh object

	// Create the sphere
	if(D3DXCreateSphere(g3D->mDevice, radius, kSlices, kStacks, &tempMesh, NULL) != D3D_OK)
		return false;

	// Copy the sphere, converting to our FVF 
	if(tempMesh->CloneMeshFVF(D3DXMESH_VB_MANAGED, SPosNormColor::kFVF, g3D->mDevice, mesh) != D3D_OK)
		return false;

	SPosNormColor *v;

	// Lock the vertex data of our sphere
	if((*mesh)->LockVertexBuffer(0, (void**)&v) != D3D_OK)
	{	
		(*mesh)->Release();
			return false;
	}

	// Set the sphere's color
	for(unsigned int i = 0; i < (*mesh)->GetNumVertices(); ++i)
		v[i].color = color;

	// Unlock the vertex data
	(*mesh)->UnlockVertexBuffer();

	tempMesh->Release(); // Free up the temporary mesh
		return true;	
}

// Helper function for setting all the members of D3DVERTEXELEMENT9
void SetElement(D3DVERTEXELEMENT9 &e, WORD stream, WORD offset, BYTE type,
				BYTE method, BYTE usage, BYTE usageIndex)
{
	e.Stream = stream;
	e.Offset = offset;
	e.Type = type;
	e.Method = method;
	e.Usage = usage;
	e.UsageIndex = usageIndex;
}

bool CreateBox(SMesh *box, float width, float height, float depth)
{
	// First fill in the position and texture coordinate data
	for(int i = 0; i < kVertCount; ++i)
	{
		gVerts[i].pos.x = kCubeVerts[i].x * width;
		gVerts[i].pos.y = kCubeVerts[i].y * height;
		gVerts[i].pos.z = kCubeVerts[i].z * depth;
		gVerts[i].tc.u = kCubeVerts[i].u;
		gVerts[i].tc.v = kCubeVerts[i].v;
	}
	
	// Now loop through all the vertices and calculate
	// the tangent basis at each vertex.
	for(int i = 0; i < kIndexCount; i += 3)
	{
		int index1 = kIndexList[i];
		int index2 = kIndexList[i + 1];
		int index3 = kIndexList[i + 2];

		// **NOTE** 
		// We specifically setup the vertex and index data for the cube
		// so we could use the below algorithm to loop through and only set
		// the tangent space basis at each vertex once.
		
		if(i % 2 == 0)
		{
			CalculateTangentSpace(gVerts[index1], gVerts[index2], gVerts[index3]);
			CalculateTangentSpace(gVerts[index2], gVerts[index3], gVerts[index1]);
		}
		else
		{
			CalculateTangentSpace(gVerts[index1], gVerts[index2], gVerts[index3]);
			CalculateTangentSpace(gVerts[index3], gVerts[index1], gVerts[index2]);
		}
	}
	
	HRESULT result = D3D_OK;
		
	D3DVERTEXELEMENT9 elements[kElementCount] = {0};
	int offset = 0;
	
	// Set the custom vertex attributes for the box
	// First off, we want it to have an (x,y,z) position
	SetElement(elements[0], 0, offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
	offset += sizeof(CVector);

	// Second, a tangent (Tx, Ty, Tz)
	SetElement(elements[1], 0, offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0);
	offset += sizeof(CVector);

	// Third, a binormal (bitangent) (Bx, By, Bz)
	SetElement(elements[2], 0, offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0);
	offset += sizeof(CVector);

	// Fourth, a normal (Nx, Ny, Nz)
	SetElement(elements[3], 0, offset, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
	offset += sizeof(CVector);

	// And lastly, one set of texture coordinates (U, V)
	SetElement(elements[4], 0, offset, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
	
	// To signal the end of the element array, use the D3DDECL_END() token
	SetElement(elements[5], 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0);
	
	// Create the custom vertex declaration
	result = g3D->mDevice->CreateVertexDeclaration(elements, &box->vertexDeclaration);
	
		// Error check
		if(result != D3D_OK)
			return false;

	// Calculate needed vertex buffer size
	int vbSize = sizeof(SVertex) * kVertCount;
	VOID *vbData = NULL;
		
	// Create a vertex buffer
	result = g3D->mDevice->CreateVertexBuffer(vbSize, 0, 0, D3DPOOL_MANAGED, &box->vertexBuffer, NULL);
	
		// Error check
		if(result != D3D_OK)
			return false;

	result = box->vertexBuffer->Lock(0, 0, (void**)&vbData, 0); // Lock it
	
		// Error check
		if(result != D3D_OK)
			return false;
		
	memcpy(vbData, gVerts, vbSize); // Fill it
	
	result = box->vertexBuffer->Unlock(); // Unlock it
	
		// Error check
		if(result != D3D_OK)
			return false;
			
	// Calculate needed vertex buffer size
	int ibSize = sizeof(WORD) * kIndexCount;
	VOID *ibData = NULL;

	// Create a index buffer
	result = g3D->mDevice->CreateIndexBuffer(ibSize, 0, D3DFMT_INDEX16, 
											 D3DPOOL_MANAGED, &box->indexBuffer, NULL);

		// Error check
		if(result != D3D_OK)
			return false;
			
	result = box->indexBuffer->Lock(0, 0, (void**)&ibData, 0); // Lock it

		// Error check
		if(result != D3D_OK)
			return false;

	memcpy(ibData, kIndexList, ibSize); // Fill it

	result = box->indexBuffer->Unlock(); // Unlock it
	
		// Error check
		if(result != D3D_OK)
			return false;
	
	// Set vertex and index counts
	box->vertexCount = kVertCount;
	box->indexCount = kIndexCount;
		return true;  // We made it!	
}

// Calculates the tangent space for "v1" and stores it in "v1"
void CalculateTangentSpace(SVertex &v1, const SVertex &v2, const SVertex &v3)
{
	// Together v1, v2, and v3 make a triangle.  First calculate the edges from
	// v1 to v2 and from v1 to v3
	CVector edge1 = v2.pos - v1.pos;
	CVector edge2 = v3.pos - v1.pos;
	
	// Create edge vectors for the UV coordinates from v1 to v2 and from v1 to v3
	CTexCoord uvEdge1 = v2.tc - v1.tc;
	CTexCoord uvEdge2 = v3.tc - v1.tc;

	// So far not so bad for tangent space right?  Well now we are going to 
	// calculate our tangent space vectors.  Together these vectors make an 
	// orthonormal basis.  An orthonormal basis, in 3D land, is simply 3 vectors
	// all unit length, that are all perpendicular to one another, and all have
	// the same origin.  So for instance +X (1,0,0), +Y (0,1,0), and +Z (0,0,1) are
	// the orthonormal basis defining our world space.  We need the tangent space basis,
	// to transform stuff to be relative to tangent space so we can then perform 
	// our lighting equations.

	// Now here is as complex as we get.  We know that our tangent basis has
	// three vectors.  We want one vector to point in the same direction as
	// uvEdge1 and the other to point in the same direction as uvEdge2.  When
	// we combine all of these elements, we get the following linear system:
	/*
	[ edge1.x edge1.y edge1.z ] = [ uvEdge1.u	uvEdge1.v ] * [ Tx Ty Tz ]
	[ edge2.x edge2.x edge2.z ]	  [ uvEdge2.u	uvEdge2.v ]	  [ Bx By Bz ]

	(Tx, Ty, Tz) = tangent
	(Bx, By, Bz) = binormal

	We want to solve for the tangent and binormal so by multiplying both sides of
	the above equation by the inverse of the "uvEdge matrix" we get:
	
	[ Tx Ty Tz ] = (1 / det) * [ uvEdge2.v	-uvEdge1.v ] * [ edge1.x edge1.y edge1.z ]
	[ Bx By Bz ]			   [ -uvEdge2.u	 uvEdge1.u ]   [ edge2.x edge2.x edge2.z ]

	det = (uvEdge1.u * uvEdge2.v) - (uvEdge2.u * uvEdge1.v);

	*/

	// Okay!  So we have what we need.  Below is grinding through the math to solve 
	// the equations.  The math involved is not overly complex, but a decent 
	// understanding of linear algebra is sure to help out :)

	// Calculate the determinant of the "UV edge" matrix
	float det = (uvEdge1.u * uvEdge2.v) - (uvEdge2.u * uvEdge1.v);
	
	// If our determinant is approximately zero, we're done!
	if(det < 0.0001f && det > -0.0001f)
	{
		// Set our tangent space to the identity matrix
		v1.tangent = CVector(1.0f, 0.0f, 0.0f);
		v1.binormal = CVector(0.0f, 1.0f, 0.0f);
		v1.normal = CVector(0.0f, 0.0f, 1.0f);
			return;
	} 

	// Compute tangent vector
	v1.tangent.x = ((uvEdge2.v * edge1.x) + (-uvEdge1.v * edge2.x)) * (1.0f / det);
	v1.tangent.y = ((uvEdge2.v * edge1.y) + (-uvEdge1.v * edge2.y)) * (1.0f / det);
	v1.tangent.z = ((uvEdge2.v * edge1.z) + (-uvEdge1.v * edge2.z)) * (1.0f / det);
	v1.tangent.normalize();

	// Compute binormal vector
	v1.binormal.x = ((-uvEdge2.u * edge1.x) + (uvEdge1.u * edge2.x)) * (1.0f / det);
	v1.binormal.y = ((-uvEdge2.u * edge1.y) + (uvEdge1.u * edge2.y)) * (1.0f / det);
	v1.binormal.z = ((-uvEdge2.u * edge1.z) + (uvEdge1.u * edge2.z)) * (1.0f / det);
	v1.binormal.normalize();

	// Compute normal vector
	v1.normal = v1.tangent.cross(v1.binormal);
	v1.normal.normalize();
}

// Draw lines represent a tangent space basis
void RenderTangentSpace()
{
	const int kStart = 0;
	const int kEnd = 4; // **NOTE** Change to kVertCount to see all tangent space basis's

	// Draw the lines
	for(int i = kStart; i < kEnd; ++i)
	{
		g3D->renderLine(gVerts[i].pos, gVerts[i].pos + gVerts[i].normal, D3DCOLOR_XRGB(0, 0, 200));
		g3D->renderLine(gVerts[i].pos, gVerts[i].pos + gVerts[i].tangent, D3DCOLOR_XRGB(200, 0, 0));
		g3D->renderLine(gVerts[i].pos, gVerts[i].pos + gVerts[i].binormal, D3DCOLOR_XRGB(0, 200, 0));
	}
}

// Normally we end right about here...

/*
	Normal mapping is cool.  And in 2006 and beyond you can't call yourself a graphics
	programmer without having implemented it yourself at least once.

	It should be noted that we used tangent space for our lighting equations, BUT, you can
	light in any space you want.  The key is that everything is in the same space.

	When calculating tangent space, sometimes you can cheat a little.  Below is
	an alternative way to calculate the binormal and normal vectors and should
	give similar results.  Test it out on more complex geometry and see the results! 

		// Calculate normal
		v1.normal = edge1.cross(edge2);
		v1.normal.normalize();

		// Calculate tangent (same as in function)

		// Calculate binormal
		v1.binormal = v1.normal.cross(v1.tangent);
		v1.binormal.normalize();

	Lastly, here's a little fact.  "Binormal" is not exactly a correct labeling, it 
	should really be called "bitangent", however, binormal is typically used.

	Just like normal, if you have questions or comments, post 'em on the
	forms at www.GameTutorials.com
*/
	
/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2006 GameTutorials, LLC |
\*-------------------------*/