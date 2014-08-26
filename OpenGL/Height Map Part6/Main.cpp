//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		HeightMap6										 //
//																		 //
//		$Description:	Using shaders we show realistic-refracted water  //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"								// This includes our main header file

bool  g_bFullScreen = true;						// Set full screen as default
HWND  g_hWnd;									// This is the handle for the window
RECT  g_rRect;									// This holds the window dimensions
HDC   g_hDC;									// General HDC - (handle to device context)
HGLRC g_hRC;									// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;							// This holds the global hInstance for UnregisterClass() in DeInit()


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// * IMPORTANT * You must have a video card with GLSL shader support to run this.
//
// You will be blown away with this tutorial as we show you how to generate
// realistic water using shaders.  Be sure to put a towel over your keyboard so
// your drool doesn't get into the keys :)  This builds off of our previous tutorial 
// called HeightMap5 (volumetric fog).  In the past, to create realistic water that
// was moving and refracting you had to have a great deal of triangles to
// simulate the water's wave; however, using normal and dudv maps we can create
// visually stunning effects that make it appear that we have a highly tessellated
// surface for the water.  In this tutorial we use a single QUAD (four points) to
// perform our water effect.  The texturing and shaders gives us the effect.  There
// is no water simulation with physics.  
//
// The idea and mentoring for this tutorial comes from Michael Horsch with Bonzai
// Software.  Let's go over many of the terms that you will need to understand for 
// this and future tutorials.
//
// You can also view this tutorial with pictures by opening RealisticWater.pdf.
//
// * What is Reflection? *
//
// Reflection is just taking the environment and flipping it upside down, clipping
// it to the surface that we want to show the reflection.  In this lesson we only
// reflect our scene into the water surface.
//
// * What is refraction? *
//
// The best way to understand refraction is to run this demo and look at the
// reflection of the world on the water.  Notice that the reflection moves as if
// there are waves moving it around?  This is called "refraction".  Basically, you
// are bending light around.  Go to your cupboard and grab a glass.  Look through
// the glass and notice the world reflects funny through the glass.  This is also
// refraction.  When dealing with water and glass surfaces you will use refraction
// to give the appearance of photorealism.
//
// How is refraction done in real-time rendering?  People a lot smarter than me
// created something called a dudv map.  Check below for the description on dudv maps.
//
// * What is a normal map? *
//
// If you are familiar with bump mapping then you probably have heard of normal maps.
// Open normalmap.bmp and notice the pink, blue and purple colors?  Each one of those
// pixels has an RGB value right?  Well, instead of using each pixel for a color, it
// stores a normalized normal from -1 to 1.  The normals naturally give the image a
// purple, pink and blue color.  What we do with this image is basically use it to bump 
// map our water to give it the appearance of being highly tessellated and realistic.
// This realism comes when we use per-pixel lighting on the water.  Basically, if
// we didn't have a normal map we would just depend on the normal of the water surface,
// which is pointing straight up.  The light would look horrible and the realism would
// be absent.  We also use the normal map for our Fresnel term calculations.  More on
// this concept below.  In the fragment shader set the normal to vec4(0, 1, 0, 0) and
// see the difference without the normal map.
//
// There are a couple ways to calculate normal maps.  First, you start with the normal
// image of the water (needs to be seamless or really close otherwise it will show
// funny seams in the water).  I used the ATI's program called TGAtoDot3.exe.  
// You can find it here:
//
// http://ati.amd.com/developer/sdk/radeonSDK/html/Tools/ToolsPlugIns.html
//
// You can use a plug-in for Photoshop that NVIDIA distributes: 
//
// http://developer.nvidia.com/object/photoshop_dds_plugins.html
//
// NVIDIA also has a program called "Melody" that loads a high-poly model and
// a low-poly version.  It then creates a normal map from the high-poly version.
//
// http://developer.nvidia.com/object/melody_home.html
//
// HeliosDoubleSix gives a comprehensive list of links about normal mapping:
//
// http://sv3.3dbuzz.com/vbforum/showthread.php?s=&threadid=77037
//
// * What is a DUDV map? *
//
// Open dudvmap.bmp in the Textures\ directory.  Notice the funny colors of yellow, 
// red and green?  This is called a dudv map.  You can probably imagine why it is
// called that because it's actually the derivative of a normal map.  In mathematics
// the notation for a derivative is du/dv.  What this texture does is give us a way
// to calculate refraction and how the light will react and bend on the object.  In
// our case, our object is the water.  Notice how the dudv map looks like waves of
// water?
//
// I used the TGAtoDUDV.exe program that ATI created.  To create the DUDV map you do
// it from the normal map of the water image, not the original diffuse map.  You can 
// download this program on the same page as listed above on ATI's tools page:
//
// http://ati.amd.com/developer/sdk/radeonSDK/html/Tools/ToolsPlugIns.html
//
// * DUDV texture trick for realistic water *
//
// You will probably find that just using the dudv map spit out from
// the ATI program will give you less than desirable realism for water.
// To make the dudv texture work well you can do a little trick.  Before creating
// the DUDV map from the normal map generated, create a separate normal image with
// a little Gaussian blur applied to it.  Then use that image to create the dudv map.
// Then apply a Gaussian blur to the resulting dudv map so it's not so perfect.  This
// will make the water less "perfect" and sharp looking.  Be sure to test different
// blur values for the effect that looks best with your water image.
//
// * So where is the actual water texture? *
//
// This might sound strange, but we actually don't even use the original water
// texture that we create the normal and dudv maps from.  Can you see why?  This
// is because our water texture is the reflection of the world onto the water.
// We create a texture on the fly by rendering the reflected world to a texture,
// then applying that texture to the water surface.
// 
// * What is Fresnel term? *
//
// We mentioned above about the Fresnel term (also called Fresnel reflectance).  
// This is a calculation that is used to determine realistic light reflecting off 
// of a surface.  In the 19th century there was a French man named Augustine Jean 
// Fresnel (1788-1827) who was a pioneer in studying light and it's reflective 
// nature.  He developed a formula to calculate reflection of light that will 
// simulate realism with light reflections.  You might have heard of a Fresnel 
// lens before.  It originally was used as a lens for lighthouses.  Now days 
// projectors use Fresnel’s discoveries to project light.
//
// Anyway, enough history.  In our tutorial we don't use the full equation for
// Fresnel reflectance, but it can be approximated with the following equation:
//
// Fresnel = 1 - SurfaceNormal . IncidentLightVector;
// 
// This means that we subtract the dot product of the normal and the incident 
// vector (reflected light vector).  If you are unfamiliar with an incident
// vector, this vector is the vector that "bounces" off the surface when hit.
// For example, if bouncing ball hits a surface at 45 degrees, it will most
// likely bounce off the surface at a 45 degree angle (assuming no other
// gravity and physics calculations).  That bouncing back vector is the
// incident vector.  So when the light hits on the surface, we need to find
// the reflecting vector coming off the surface.  This is necessary for finding
// the Fresnel term.
//
// Notice that it also takes the surface normal.  For our water, since we only
// have a flat surface we will want to use our normal map values for each pixel
// to approximate the appropriate light reflections.
//
// If you comment out the Fresnel reflectance calculations in our shader you can
// see the necessity for using Fresnel term.  The contrast looks horrible and not
// very realistic.
// 
// * The use of a depth map *
//
// We can calculate the fogginess of the water by the use of a depth map.  We render 
// our current view to a texture using just the depth values in the z-buffer.  We 
// use these depth values to further add realism to our water lighting and reflection.  
// The depth buffer will tell us how far away the pixel is from the camera.  We can 
// use that value to make certain areas of the water darker and certain areas lighter.  
// The depth texture is created by just the world and the terrain, not the water.  This 
// way deeper parts of the water will show that.  It's a great way to handle the 
// fogginess of the water.
//
// Since we already calculate our own fog we don't actually need the depth map, but
// I chose to use it to show you how to calculate fog this way.  I also added it
// because it makes the surface a lot darker, which looks better in my opinion.
//
// * What is bump mapping? *
//
// This tutorial uses bump mapping to make the water look tessellated.
// If you have a low-poly model and you want to make it look like it's
// more detailed you can create a normal map to give it that effect.
// If we use per-pixel lighting then the model will have incredible
// detail and will light the model as if there is extruded detail.
// To do bump mapping we need to do some calculations in "tangent space".
//
// * What is tangent space? (also called texture space) *
//
// You may have heard of tangent space before, also sometimes referred to as
// texture space (by NVIDIA).  Each vertice basically has it's own axis, with 
// the normal pointing up as the Y axis and a tangent and bi-tangent creating 
// the X and Z axis respectively.  This XYZ tangent-space axis is calculated 
// for every vertice that will be bump mapped.
//
// The reason why we need to convert to tangent space is because we use a normal
// map that stores normals from only one view (the view it was created).  When
// the world and objects in the world are transform, rotated and scaled, it
// doesn't line up with the normals in the texture map.
//
// Think of it this way: you create a normal map of a 3D character facing
// forward, but when the character is moving in tons of different directions
// you need to either rotate the normals in the texture map every time you
// rotate the character, or you just convert the vertices, light and camera
// position to tangent space.  Can you see why it's also called "texture space"?
//
// In this tutorial we don't focus on calculation the tangent space for vertices
// because we only have one flat quad that has a tangent-space axis of:
//
// tangent   = (1.0, 0.0, 0.0, 0.0);
// normal    = (0.0, 1.0, 0.0, 0.0);
// biTangent = (0.0, 0.0, 1.0, 0.0);
//
// However, we do need to calculate tangent space for the view and light vectors.
// In code we do it like this:
//
// viewTangetSpace.x = dot(viewDirection, tangent);
// viewTangetSpace.y = dot(viewDirection, biTangent);
// viewTangetSpace.z = dot(viewDirection, normal);
// viewTangetSpace.w = 1.0;
//
// lightTangetSpace.x = dot(lightDirection, tangent);
// lightTangetSpace.y = dot(lightDirection, biTangent);
// lightTangetSpace.z = dot(lightDirection, normal);
// lightTangetSpace.w = 1.0;
//
// Now when we calculate our bump mapping depending on our light and view, it will
// already be aligned with the normal map's original axis.
//
// * BiTangent verses BiNormal *
//
// Apparently BiTangent and BiNormal have been used synonymously as the same thing,
// which isn't true.  Though BiNormal is used all over the place, many have pointed
// out in the graphics world that it is incorrect.  BiTangent should be used when
// speaking about tangent space in 3D.  We will use BiTangent instead of BiNormal
// for this tutorial to try and correct this minor mistake.
//
// * What are caustics? *
//
// Caustics are the effect of texturing objects underwater to give the
// illusion of light reflecting/refracting through the surface of the water.
// We have a few dozen textures that simulate this effect.  The textures are
// multi-textured onto the surface of the terrain.  We only show them when we
// are under the water.
//
// * SUMMARY - How does the water effect work? *
//
// In summary, let's explain in a nutshell how our water effect works.
// We have a single QUAD across the terrain for our water.  Every frame
// we render to three textures (reflection, refraction and depth).  Then 
// in our shaders we tile the normal and dudv map, while incorporating the
// the correct equations to add all of the texture maps together.  We then
// add specular lighting and voila!
//
// By viewing this tutorial we assume that you have looked at the previous
// tutorial HeightMap5, as well as our beginner GLSL shader tutorials.  The 
// files that you will want to look at for this lesson is Main.cpp, Main.h, 
// Water.cpp, Water.h, Water.vert and Water.frag.  We added CShader.cpp and
// CShader.h to this project, but there is nothing new in those files from
// the beginner GLSL shader tutorials.
//
// Be sure to look at the bottom of this file in the QUICK NOTES section to
// get a detailed line-by-line explanation of the two shader files.
//
// CONTROLS:
//
// The controls are just like the last tutorials.  The camera is moved by
// the mouse and the arrow keys (also w, a, s, d).  The mouse buttons
// turn detail texturing on/off as well as switch to wireframe.  The fog in
// the water can be changed by the + and minus keys.  We also added these keys:
//
// F1 - Makes the water appear farther away by shrinking the water texture
// F2 - Makes the water appear closer by enlarging the water texture
// F3 - Makes the water move faster
// F4 - Makes the water slower
//
// *** Be sure to use the F-keys to get some amazing effects. ***
//
// Let's go down and look at all the new additions.
// 


// This is our global shader object that will handle our shaders
CShader g_Shader;

// The size of the textures that we will render on the fly (reflection/refraction/depth)
int g_TextureSize = 512;

// The height of the water that is above the terrain floor
float g_WaterHeight = 30.0f;

// The scale for the water textures
float g_WaterUV = 35.0f;

// The speed of the water flow
float g_WaterFlow = 0.0015f;

// The scale for the caustics (light textures underwater)
const float kCausticScale = 4.0f;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This is our fog extension function pointer to set a vertice's depth
PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;

// This stores the desired depth that we want to fog
float g_FogDepth = 30.0f; // (* NEW *)

// Our function pointers for the ARB multitexturing functions
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;

// This controls if we have detail texturing on or off
bool g_bDetail = true;

// This handles the current scale for the texture matrix for the detail texture
int g_DetailScale = 16;

// This is our global camera object
CCamera g_Camera;								

// This holds the height map data
BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];			

// This tells us if we want lines or fill mode
bool  g_bRenderMode = true;						

// This holds the texture info by an ID
UINT g_Texture[MAX_TEXTURES] = {0};		


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the application
/////
///////////////////////////////// INIT  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Here we initialize our multitexturing functions
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	// Make sure our multi-texturing extensions were loaded correctly
	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your current setup does not support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// Find the correct function pointer that houses the fog coordinate function
	glFogCoordfEXT	= (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");

	// Before trying to use this function pointer, we need to make sure it was
	// given a valid address.  If not, then we need to quit because something is wrong.
	if(!glFogCoordfEXT)
	{
		// Print an error message and quit.
		MessageBox(g_hWnd, "Your current setup does not support volumetric fog", "Error", MB_OK);
		PostQuitMessage(0);
	}

	// Pick a tan color for our fog with a full alpha
	float fogColor[4] = {0.2f, 0.2f, 0.9f, 1.0f};

	glEnable(GL_FOG);						// Turn on fog
	glFogi(GL_FOG_MODE, GL_LINEAR);			// Set the fog mode to LINEAR (Important)
	glFogfv(GL_FOG_COLOR, fogColor);		// Give OpenGL our fog color
	glFogf(GL_FOG_START, 0.0);				// Set the start position for the depth at 0
	glFogf(GL_FOG_END, 50.0);				// Set the end position for the depth at 50

	// Now we tell OpenGL that we are using our fog extension for per vertex fog
	glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

	// Here we read in the height map from the .raw file
	LoadRawFile("Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);	

	glEnable(GL_DEPTH_TEST);							// Enables depth testing
	glEnable(GL_TEXTURE_2D);							// Enable texture mapping
	glEnable(GL_CULL_FACE);								// Turn on back-face culling


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// The extra initialization for this tutorial is to (1) position the camera
	// above the water, (2) create 3 textures for our reflection, refraction and
	// depth, (3) load all of our bitmap textures, including the caustic textures,
	// (4) as well as init and load our shaders.

	// Point our camera above the water, looking at the sun and reflected water
	g_Camera.PositionCamera( 475, 52, 301,  474, 52, 300,  0, 1, 0);

	// Since our water is reflecting the world that our camera is looking at,
	// we need to have 3 custom textures that we will render to.  These textures
	// will then be used in the shaders to do the reflection and refraction.
	// The depth texture will just store a single value for the depth of
	// each pixel.  This is why we use GL_DEPTH_COMPONENT and require 1 channel.
	CreateRenderTexture(g_Texture, g_TextureSize, 3, GL_RGB, REFLECTION_ID);
	CreateRenderTexture(g_Texture, g_TextureSize, 3, GL_RGB, REFRACTION_ID);
	CreateRenderTexture(g_Texture, g_TextureSize, 1, GL_DEPTH_COMPONENT, DEPTH_ID);
	
	CreateTexture(g_Texture[NORMAL_ID],	 "Textures\\normalmap.bmp");// Load the normal map water texture
	CreateTexture(g_Texture[DUDVMAP_ID], "Textures\\dudvmap.bmp");	// Load the dudv map water texture
	CreateTexture(g_Texture[TERRAIN_ID], "Textures\\Terrain.bmp");	// Load the terrain texture
	CreateTexture(g_Texture[DETAIL_ID],	 "Textures\\Detail.bmp");	// Load the detail texture for the terrain
	CreateTexture(g_Texture[BACK_ID],	 "Textures\\Back.bmp");		// Load the Sky box Back texture
	CreateTexture(g_Texture[FRONT_ID],	 "Textures\\Front.bmp");	// Load the Sky box Front texture
	CreateTexture(g_Texture[BOTTOM_ID],  "Textures\\Bottom.bmp");	// Load the Sky box Bottom texture
	CreateTexture(g_Texture[TOP_ID],	 "Textures\\Top.bmp");		// Load the Sky box Top texture
	CreateTexture(g_Texture[LEFT_ID],	 "Textures\\Left.bmp");		// Load the Sky box Left texture
	CreateTexture(g_Texture[RIGHT_ID],	 "Textures\\Right.bmp");	// Load the Sky box Right texture
	
	// Just like in our Win32 animation tutorial, load the 32 animated caustic 
	// textures starting with the prefix "caust".
	LoadAnimTextures(g_Texture, "Textures\\caust", WATER_START_ID, NUM_WATER_TEX);

	// Let's init our GLSL functions and make sure this computer can run the program.
	InitGLSL();

	// Here we pass in our vertex and fragment shader files to our shader object.
	g_Shader.InitShaders("water.vert", "water.frag");

	// Since we only use the shaders for the water we turn off the shaders by default.
	g_Shader.TurnOff();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


}


///////////////////////////////// CREATE SKY BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a sky box centered around X Y Z with a width, height and length
/////
///////////////////////////////// CREATE SKY BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	// Turn on texture mapping if it's not already
	glEnable(GL_TEXTURE_2D);

	// Bind the BACK texture of the sky map to the BACK side of the cube
	glBindTexture(GL_TEXTURE_2D, g_Texture[BACK_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// This centers the sky box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the BACK Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// Bind the FRONT texture of the sky map to the FRONT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[FRONT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);	
	
		// Assign the texture coordinates and vertices for the FRONT Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[BOTTOM_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
	
		// Assign the texture coordinates and vertices for the BOTTOM Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// Bind the TOP texture of the sky map to the TOP side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[TOP_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the TOP Side
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// Bind the LEFT texture of the sky map to the LEFT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[LEFT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the LEFT Side
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[RIGHT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

		// Assign the texture coordinates and vertices for the RIGHT Side
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the terrain and caustic effects
/////
///////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderWorld(bool bRenderCaustics)
{
	// This function will be called a total of three times to
	// create our realistic water effect.  The first call is
	// render the world to our reflection texture.  The next
	// call is to render the world to our refraction and depth
	// texture.  The last call is to render the world normally.

	// This next function draws the terrain with the caustic animation.
	// A simple way we do this is to render the caustics all over
	// the terrain and then clip the terrain above the water.
	// This is the simplest way, however it is slower because we
	// have to render the terrain an extra time.  A better way would
	// be to use texturing in a shader that would slowly
	// fade out as it reached the water's surface.  This would also cut
	// out the hard break of the caustics when it reaches above water.
	// Projective textures are also an options.

	// I chose a scale of four for the caustic textures.  This would
	// depend on the water depth of course.  Adjust for realism in your world.
	// The water height is used for clipping the top part of the terrain.
	
	// Only render the caustics if we want to (i.e. not rendering reflection texture)
	if(bRenderCaustics)
		RenderCaustics(g_WaterHeight, kCausticScale);

	// Since we already are rendering the bottom part of the terrain with
	// caustics, we don't need to render the entire terrain again, just the
	// top part.  This is why we create a clipping plane reject all the
	// data below the water.  This could be done in the RenderHeightMap()
	// function so that we don't try and draw anything below the water in
	// this case, but like we said before, this is just a simple way to make
	// the effect happen and is not intended show a faster approach.  We would
	// also want to put the terrain in a display list if we wanted to increase
	// speed.

	// Anything above the water surface is okay to draw, but reject data below.
	// The first three values are the normal of the plane, and the last value
	// is the distance from the plane to clip.  Since this plane equation
	// is transformed by the inverse of the modelview matrix we have the distance
	// a negative number.  This can be confusing because it seems backwards.
	double TopTerrainPlane[4] = {0.0, 1.0, 0.0, -g_WaterHeight};
	
	// Turn a clipping plane on and assign our plane equation to it
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, TopTerrainPlane);

	// Render the clipped height map that is above the water
	RenderHeightMap(g_HeightMap);

	// Turn our clipping plane off
	glDisable(GL_CLIP_PLANE0);
	
	// Render our background sky box.
	CreateSkyBox(500, 0, 500, 2000, 2000, 2000);
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene()
{


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Before we start rendering the world at our set view port, we need to
	// render to our reflection, refraction and depth textures.  The view
	// port is set to our g_TextureSize and then render our textures.
	// Once that is finished, we need to reset the view port to our normal
	// window size.

	// Render the screen to a texture for our reflection, refraction and depth.
	// The texture size is for our view port, and the water height is for clipping.
	CreateReflectionTexture(g_WaterHeight, g_TextureSize);
    CreateRefractionDepthTexture(g_WaterHeight, g_TextureSize);

    // Restore the OpenGL view port to our original screen size
    glViewport(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Clear our color and depth bits, as well as reset our current matrix.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

	// Get the current position of the camera
	CVector3 vPos		= g_Camera.Position();
	CVector3 vNewPos    = vPos;

	// * Basic Collision *
	// Check if the camera is below the height of the terrain at x and z,
	// but we add 10 to make it so the camera isn't on the floor.
	if(vPos.y < Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10)
	{
		// Set the new position of the camera so it's above the terrain + 10
		vNewPos.y = (float)Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10;

		// Get the difference of the y that the camera was pushed back up
		float temp = vNewPos.y - vPos.y;

		//  Get the current view and increase it by the different the position was moved
		CVector3 vView = g_Camera.View();
		vView.y += temp;

		// Set the new camera position.
		g_Camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
								vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Set the current camera position and view
	g_Camera.Look();

	// Render the caustics, terrain and the sky box
    RenderWorld(true);

	// Now let's draw the water!  First we want to turn shaders on.
	// This will create the realistic effect.  Notice below that we
	// turn off culling for the water.  This is because we want to 
	// see both sides of the water.  When we go under we still want 
	// the water to be showing when we look up.  Keep in mind that 
	// the water is just a flat quad.  We could also just turn the
	// culling to FRONT when we are under the water, the reset it
	// to BACK after drawing the water.  This might increase speed.

	// Turn the shaders on for the water
	g_Shader.TurnOn();

	// Disable culling, draw the water at the g_WaterHeight, then turn
	// culling back on so we can see the water from above or below.
	glDisable(GL_CULL_FACE);
	RenderWater(g_WaterHeight);
	glEnable(GL_CULL_FACE);

	// Turn the shaders off again until we redraw the water
	g_Shader.TurnOff();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);	
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop
/////
///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											// if there wasn't a message
		{ 		
			if(AnimateNextFrame(60))					// Make sure we only animate 60 FPS
			{
				g_Camera.Update();						// Update the camera data
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
        } 
	}
	
	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;


	case WM_LBUTTONDOWN:
	
		g_bDetail = !g_bDetail;
		break;

	case WM_RBUTTONDOWN:								// If the left mouse button was clicked
		
		g_bRenderMode = !g_bRenderMode;

		// Change the rendering mode to and from lines or triangles
		if(g_bRenderMode) 				
		{
			// Render the triangles in fill mode		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
		else 
		{
			// Render the triangles in wire frame mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		}
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_SPACE:								// Check if we hit the SPACE bar

				// Times the current scale value by 2 and loop when it gets to 128
				g_DetailScale = (g_DetailScale * 2) % 128;

				// If the scale value is 0, set it to 1 again
				if(g_DetailScale == 0)
					g_DetailScale = 1;
				break;

			case VK_ADD:								// Check if we hit the + key

				g_FogDepth += 1;						// Increase the fog height

				if(g_FogDepth > 200)					// Make sure we don't go past 200
					g_FogDepth = 200;

				break;

			case VK_SUBTRACT:							// Check if we hit the - key

				g_FogDepth -= 1;						// Decrease the fog height

				if(g_FogDepth < 0)						// Make sure we don't go below 0
					g_FogDepth = 0;

				break;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			// To control the water's realism we added F1-F4 keys to change the
			// water's speed and texture size.  F1 is my favorite :)

			case VK_F1:									// Check if we hit the F1 key

				g_WaterUV += 1.0f;						// Increase the water UV texture
				break;
			
			case VK_F2:									// Check if we hit the F2 key

				g_WaterUV -= 1.0f;						// Increase the water UV texture

				if(g_WaterUV < 0.0f)					// If water's UV is below zero, reset it.
					g_WaterUV = 0.0f;
				break;

			case VK_F3:									// Check if we hit the F3 key

				g_WaterFlow += 0.00003f;				// Increase the water's speed
				break;

			
			case VK_F4:									// Check if we hit the F4 key

				g_WaterFlow -= 0.00003f;				// Decrease the water's speed

				if(g_WaterFlow < 0.0f)					// If speed is below zero, reset it.
					g_WaterFlow = 0.0f;
				break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


		}
		break;
 
    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Tell windows we want to quit
        break; 
    } 
 
    return DefWindowProc (hWnd, uMsg, wParam, lParam); 	// Return the default
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// You must REALLY want to create realistic water if you have reached this
// part of the tutorial :)  I know there is a ton of explanation, but I felt
// that ever bit was important because there isn't a lot of information out
// there about things covered in this tutorial.
//
// Once again, let's write a summary on how the water effect works:
//
// * SUMMARY - How does the water effect work? *
//
// We have a single QUAD across the terrain for our water.  Every frame
// we render to three textures (reflection, refraction and depth).  Then 
// in our shaders we tile the normal and dudv map, while incorporating the
// the correct equations to add all of the texture maps together.  We then
// add specular lighting and voila!
//
// Water.cpp is where the bulk of the OpenGL code is for the water.  Be sure
// to view that file before continuing on to see the detailed explanations
// of the shader files.
//
// Assuming you now understand how the OpenGL code works for this effect, 
// let's go through the vertex shader line-by-line.  First the code uncommented:
//
// * Water.vert shader file *

/*

varying vec4 refrCoords; 
varying vec4 normCoords; 
varying vec4 viewCoords;
varying vec4 viewTangetSpace;
varying vec4 lightTangetSpace;

uniform vec4 lightPos, cameraPos;


void main()
{
	vec4 tangent = vec4(1.0, 0.0, 0.0, 0.0);
	vec4 normal = vec4(0.0, 1.0, 0.0, 0.0);
	vec4 biTangent = vec4(0.0, 0.0, 1.0, 0.0);

	vec4 viewDir = cameraPos - gl_Vertex;

	viewTangetSpace.x = dot(viewDir, tangent);
	viewTangetSpace.y = dot(viewDir, biTangent);
	viewTangetSpace.z = dot(viewDir, normal);
	viewTangetSpace.w = 1.0;

	vec4 lightDir = lightPos - gl_Vertex;

	lightTangetSpace.x = dot(lightDir, tangent);
	lightTangetSpace.y = dot(lightDir, biTangent);
	lightTangetSpace.z = dot(lightDir, normal);
	lightTangetSpace.w = 1.0;

	refrCoords = gl_MultiTexCoord1;
	normCoords = gl_MultiTexCoord2;

	viewCoords = gl_ModelViewProjectionMatrix * gl_Vertex;

	gl_Position = viewCoords;
}

*/

// Like we explained at the top of this file, we need to convert our
// light and camera view to tangent space in order to correctly bump
// map the water.  We already know the tangent space axis for our flat
// water quad so we just hard code those values for our normal, tangent
// and biTangent.
//
// The vertex shader next stores the UV coordinates for the normal and
// refraction map, also the screen coordinates are stored for looking
// up the reflection, refraction and depth maps.
//
// Let's explain every line now:

/*

*** This stores the refraction texture UV coordinates
varying vec4 refrCoords; 

*** This stores the normal map texture UV coordinates
varying vec4 normCoords; 

*** This stores the screen coordinates to be used as a lookup
*** into the reflection, refraction and depth map.
varying vec4 viewCoords;

*** These store our converted view and light vectors into tangent space
varying vec4 viewTangetSpace;
varying vec4 lightTangetSpace;

*** These hold our current light and camera position
uniform vec4 lightPos, cameraPos;

*** Start our vertex shader program
void main()
{
	*** Because we have a flat plane for water we hard code the tangent space vectors
	vec4 tangent = vec4(1.0, 0.0, 0.0, 0.0);
	vec4 normal = vec4(0.0, 1.0, 0.0, 0.0);
	vec4 biTangent = vec4(0.0, 0.0, 1.0, 0.0);

	*** Calculate the vector coming from the vertex to the camera
	vec4 viewDir = cameraPos - gl_Vertex;

	*** Compute tangent space for the view direction
	viewTangetSpace.x = dot(viewDir, tangent);
	viewTangetSpace.y = dot(viewDir, biTangent);
	viewTangetSpace.z = dot(viewDir, normal);
	viewTangetSpace.w = 1.0;

	*** Calculate the vector where the light hits the vertex
	vec4 lightDir = lightPos - gl_Vertex;

	*** Compute tangent space for the light direction
	lightTangetSpace.x = dot(lightDir, tangent);
	lightTangetSpace.y = dot(lightDir, biTangent);
	lightTangetSpace.z = dot(lightDir, normal);
	lightTangetSpace.w = 1.0;

	*** Store the UV coordinates that we set for the refraction
	*** and normal map set for texture unit 1 and 2.
	refrCoords = gl_MultiTexCoord1;
	normCoords = gl_MultiTexCoord2;

	*** Store the coordinates that are projected onto our screen for
	*** looking up coordinates in the reflection, refraction and depth map.
	viewCoords = gl_ModelViewProjectionMatrix * gl_Vertex;

	*** Set the current position of the vertex in screen coordinates
	gl_Position = viewCoords;
}

*/

// 
// Let's look at the fragment shader Water.frag:
//
// * Water.frag shader file explanation *

/*

varying vec4 refrCoords; 
varying vec4 normCoords; 
varying vec4 viewCoords;
varying vec4 viewTangetSpace;
varying vec4 lightTangetSpace;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D normalMap;
uniform sampler2D dudvMap;
uniform sampler2D depthMap;
uniform vec4 waterColor;

void main()
{
	const float kShine = 128.0;
	const float kDistortion = 0.015;
	const float kRefraction = 0.009;

	vec4 distOffset = texture2D(dudvMap, normCoords.xy) * kDistortion;
	vec4 dudvColor = texture2D(dudvMap, vec2(refrCoords + distOffset));
	dudvColor = normalize(dudvColor * 2.0 - 1.0) * kRefraction;

	vec4 normalVector = texture2D(normalMap, vec2(refrCoords + distOffset));
	normalVector = normalVector * 2.0 - 1.0;
	normalVector.a = 0;

	vec4 lightReflection = normalize( reflect(-lightTangetSpace, normalVector) );

	vec4 invertedFresnel = vec4( dot(normalVector, lightReflection ) );
	vec4 fresnelTerm = 1.0 - invertedFresnel;

	vec4 projCoord = viewCoords / viewCoords.q;
	projCoord = (projCoord + 1.0) * 0.5;
	projCoord += dudvColor;
	projCoord = clamp(projCoord, 0.001, 0.999);
	
	vec4 reflectionColor  = texture2D(reflection, projCoord.xy);
	vec4 refractionColor  = texture2D(refraction, projCoord.xy);
	vec4 depthValue = texture2D(depthMap, projCoord.xy);
		
	vec4 invDepth = 1.0 - depthValue;
	refractionColor *= invertedFresnel * invDepth;
	refractionColor +=  waterColor * depthValue * invertedFresnel;

	reflectionColor *= fresnelTerm;

	vec4 localView = normalize(viewTangetSpace);		
	float intensity = max(0.0, dot(lightReflection, localView) );
	vec4 specular = vec4(pow(intensity, kShine));

	gl_FragColor = refractionColor + reflectionColor + specular; 
}

*/

// If you are like me, at first glance this looks really complicated
// and probably not very intuitive to understand.  You are right :)
// I won't lie, it's pretty advanced concepts.  Once we go through
// it line-by-line you will feel comfortable with the concepts and
// it won't be so scary.  Since this shader is pretty complex we
// will go over it line-by-line instead of explaining any further:

/*

*** This stores the refraction texture UV coordinates
varying vec4 refrCoords; 

*** This stores the normal map texture UV coordinates
varying vec4 normCoords; 

*** This stores the screen coordinates to be used as a lookup
*** into the reflection, refraction and depth map.
varying vec4 viewCoords;

*** These store our converted view and light vectors into tangent space
varying vec4 viewTangetSpace;
varying vec4 lightTangetSpace;

uniform sampler2D reflection;	*** The reflection texture map
uniform sampler2D refraction;	*** The refraction texture map
uniform sampler2D normalMap;	*** The normal map texture
uniform sampler2D dudvMap;		*** The dudv texture map
uniform sampler2D depthMap;		*** The depth texture map
uniform vec4 waterColor;		*** The water color

void main()
{
	*** This is the specular light exponent value (higher value == less shiny)
	const float kShine = 128.0;			

	*** This is a distortion constant that makes the water move like waves.
	*** As kDistortion moves closer to 1.0 the water will look oily.
	*** It's a cool effect if you want to simulate oil in the water.
	*** As kDistortion moves closer to 0 the waves stop moving realistically.
	const float kDistortion = 0.015;

	*** This value tells us how much refraction we want in the water (basically,
	*** how wavy we want the reflection to be).  This doesn't effect the bump
	*** mapped effect.  A value of 0 will give no refraction in the water.
	*** The higher this value goes the more metallic and plastic the water looks.
	const float kRefraction = 0.009;

	*** Calculate the distortion wave offset with the normal map's UVs.
	*** The distOffset creates the random wavyness in the water.
	*** We use use the dudv because it holds the waves appearance.
	vec4 distOffset = texture2D(dudvMap, normCoords.xy) * kDistortion;
	
	*** Now that we have an offset for distortion, we look again into the dudv
	*** map and get a pixel for our refraction.  We use the refraction coordinates
	*** added to the distortion offset as the lookup into our dudv map.
	vec4 dudvColor = texture2D(dudvMap, vec2(refrCoords + distOffset));

	*** Just like normals, the dudv values need to be between -1 and 1. When the
	*** pixels are stored in the bitmap they are stored from 0 to 1.  To reverse
	*** this change we use the formula: originalDuDvValues = (RGBColor * 2) - 1;
	*** This needs to be done to our normal pixels as well later.  After we convert
	*** the dudv values back to normal we then multiply it by our refraction intensity.
	dudvColor = normalize(dudvColor * 2.0 - 1.0) * kRefraction;

	*** Next we get a pixel from the normal map at the same position as the dudv color.
	vec4 normalVector = texture2D(normalMap, vec2(refrCoords + distOffset));

	*** Like the dudv values, we must convert the normal back to the -1 to 1 range.
	*** Remember, we do this because normals can be pointing up or down; however, you
	*** can't save negative values in an RGB image.  The trick to get around this is to 
	*** just convert the values to be between 0 (which is -1) and 1 (which is 1) by a
	*** simple formula.  This formula is:  RGBColor = (originalNormalValues + 1) / 2;
	*** Finally, since a normal is not a color but an XYZ value we set it's alpha to 0.
	normalVector = normalVector * 2.0 - 1.0;
	normalVector.a = 0;

	*** In order to calculate the Fresnel term we need the incident light vector.  That
	*** means that we need calculate light vector that bounces off of the water's surface,
	*** according to the current normal map's normal that is.  We do this by using the
	*** GLSL function called reflect().  This function takes an incident vector and
	*** a normalized normal, then spits out a reflected vector.  Since reflect() asks
	*** for an incident vector we just negate our current light vector in tangent space.
	*** The normalized result gives us our reflection vector to compute the Fresnel term.
	vec4 lightReflection = normalize( reflect(-lightTangetSpace, normalVector) );

	*** We will compute the approximated Fresnel reflectance and inverted Fresnel.
	*** The inverted Fresnel is used with the refraction color, where the reflection
	*** uses the normal Fresnel term.  Fresnel term is appoxmated with: F = 1 - (n . i)
	vec4 invertedFresnel = vec4( dot(normalVector, lightReflection ) );
	vec4 fresnelTerm = 1.0 - invertedFresnel;

	*** Remember how we stored the screen coordinates in the vertex shader?  We want to
	*** do a projective divide to get the projection coordinates.  We then apply a bias
	*** to the coordinates to adjust the projection.  These coordinates will be used to
	*** lookup pixels in our reflection, refraction and depth texture.
	vec4 projCoord = viewCoords / viewCoords.q;
	projCoord = (projCoord + 1.0) * 0.5;

	*** This is the line necessary for the refraction to work.  By adding this value
	*** it change the coordinates used to lookup the pixels in our reflection, refraction
	*** and depth textures.  This is what adds the waviness.
	projCoord += dudvColor;

	*** We want to make sure the projected coordinates are greater than 0 and less than 1 
	*** to get rid of black artifacts that can show up.  We use GLSL's clamp() function.
	projCoord = clamp(projCoord, 0.0001, 0.9999);
	
	*** Now we can store the reflection, refraction and depth pixels by using the 
	*** projection coordinates we calculated as the UV coordinates for our textures.
	vec4 reflectionColor  = texture2D(reflection, projCoord.xy);
	vec4 refractionColor  = texture2D(refraction, projCoord.xy);
	vec4 depthValue = texture2D(depthMap, projCoord.xy);
		
	*** If you want your water to be really foggy you can use the inverse depth value
	*** with your refraction map.  By inverting the depth map we make objects that are
	*** closer to us brighter and farther away darker.  Since we calculate our own fog,
	*** using any of the depth calculations isn't really necessary, but I added it to
	*** this tutorial so you know how to do it when you don't use your own fog.
	vec4 invDepth = 1.0 - depthValue;

	*** We multiply the current refraction color by the inverse Fresnel and depth because 
	*** our refraction color is upside down.  Using the invDepth for our scene makes the
	*** water extremely foggy and you can't see through it.  If you want your water to
	*** be more see through you will want to get rid of the invDepth, or just minimize it.
	refractionColor *= invertedFresnel * invDepth;

	*** Currently, refractionColor is really dark.  If we want to increase the contrast
	*** of the water to a more blue color, we add the water color multiplied by the
	*** depth and inverted Fresnel.  Remember, we don't need the depth value since we
	*** calculate our own fog, but I have it to darken the water color.
	refractionColor +=  waterColor * depthValue * invertedFresnel;

	*** For more realistic contrast of the light on the water we multiply the Fresnel 
	*** term by the current reflection pixel.  The magic Frensel term makes it better :)
	reflectionColor *= fresnelTerm;

	*** We need to normalize our camera's view in tangent space
	vec4 localView = normalize(viewTangetSpace);		

	*** Now we get to the actual bump mapping of the water.  We first want to
	*** store the intensity of the specular light by dotting the view vector in
	*** tangent space and the light reflection.  Make sure it's between 0 and 1.
	float intensity = max(0.0, dot(lightReflection, localView) );

	*** Now that we have the specular intensity, raise the intensity to the power of kShine.
	vec4 specular = vec4(pow(intensity, kShine));

	*** FINALLY!  All we need to do is just add the refraction, reflection and light together.
	gl_FragColor = refractionColor + reflectionColor + specular; 
}

*/

// We better end this tutorial before you go crazy.  All the topics should be
// covered on how to do realistic bump-mapped water.  In this tutorial we don't
// have the water see through.  I think it looks better since the water is so large,
// but if you want to have smaller water surfaces that are see through here are the
// things you need to change:
//
// *** This line either needs to get rid of the "* invDepth" or minimize the value.
// refractionColor *= invertedFresnel * invDepth;
//
// You then want to have a constant value for reflection, like:
//
// const float kReflection = 0.8;
//
// Then, before adding the reflectionColor at the end, do this:
// 
// reflectionColor *= kReflection;
//
// The higher the kReflection is, the less see through the water will be.
// You can mess around those the values to get the desired effect you want.
// Also, play around with the +/- keys to add and subtract fog in the water.
//
// * ATI Verses NVIDIA *
//
// One thing that is important to realize is that sometimes when shaders get
// more complicated you will find that some work on NVIDIA cards and not on
// ATI cards.  Before you think that it is a compatibility issue, first check
// to see if the code compiles with GLSLValidator by 3DLabs.  Something as
// simple as normalMap.a = 0; won't work for ATI cards, it must be
// normalMap.a = 0.0;  This is something to always look out for.  ATI is more
// picky.
//
// This is all we have to teach on this topic for now, good luck!
//
//
// © 2000-2005 GameTutorials