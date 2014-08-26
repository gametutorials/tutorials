/*
	Here's what's making this tutorial a brighter place %)
	
	What's the crux of this lighting stuff you ask?  Well here is recap of some math...
	
	Fact 1) The dot product between two normalized vectors equals the cosine of the
			angle between the vectors
	
	Fact 2) Cosine returns us numbers between (-1.0f, 1.0f)
	
	Fact 3) cos(0°) == 1
			cos(90°) == 0
			cos(180°) == -1
			cos(270°) == 0
	
	Fact 4) If two normalized vectors are perpendicular their dot product will be 0
			
	Fact 5) If two normalized vectors form an angle between (0°, 90°) their dot product will
			be between (1,0)
			
	Okay a quick recap of the how the diffuse component is calculated with vertex lighting...
			
	What happens is we take a vector from the vertex to the light and call it the light vector.
	We then take the dot product of the light vector with the normal at the vertex.  If the angle
	formed is less than 90°, the dot product will give us a number between (0,1) we will use 
	it as a percentage of how much the light is striking the vertex.  A similar concept is
	used to calculate specular lighting.
	
	Now for per-pixel lighting the main catch is that instead of doing this dot product computation
	at each vertex, we do it at each pixel which gives us much smoother results.  Of course now each
	pixel is more time consuming to compute which will lower overall framerate.  Life is always
	full of trade-offs %)
	
*/
////////////////
// Constants //
//////////////

// Shine, shine everybody shine... This constant is used in calculating the
// specular's contribution to the final lit pixel.  The larger the number the
// smaller the area lit by specular lighting
const int kShine = 64;

//////////////
// Globals //
////////////

float4x4 gProjMat; // The projection matrix (Project geometery from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)

float3 gLightPos; // The world position of the point light
float3 gEyePos; // The world position of the eye of the camera

float3 gDiffuse; // The diffuse color of the point light
float3 gSpecular; // The specular color of the point light
float3 gAmbient; // The ambient color of the point light

// This will hold our vertex data after it's been processed
// by the vertex shader for vertex lighting
struct VSOutVL
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
};

// This will hold our vertex data after it's been processed
// by the vertex shader for per-pixel lighting
struct VSOutPPL
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
	float3 normal : TEXCOORD0; // Vertex normal after vertex shader has executed
	float3 lightVec : TEXCOORD1; // The vector from the vertex in world space to the 
								// the light source
	float3 halfwayVec : TEXCOORD2; // The vector halfway between the "lightVec" described above
								  // and the vector from the vertex in world space to 
								 // the eye of the camera
};

// This is what we will pass to our pixel shader
// **NOTE** What we output from the vertex shader and what we input
//			to the pixel shader do not have to be the same.  For instance
//			since we are not going to modify "pos" in the pixel shader, there
//			is no need to pass it.
struct PSIn
{
	float4 color : COLOR; // Color after vertex shader has executed
	float3 normal : TEXCOORD0; // Vertex normal after vertex shader has executed
	float3 lightVec : TEXCOORD1; // The vector from the vertex in world space to the 
								// the light source
	float3 halfwayVec : TEXCOORD2; // The vector halfway between the "lightVec" described above
								  // and the vector from the vertex in world space to 
								 // the eye of the camera
};

// This is what is outputted from the pixel shader
struct PSOut
{
	float4 color : COLOR; // Color of pixel to be drawn to framebuffer
};

// The vertex shader for vertex lighting
VSOutVL VertexLightingVS(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	float3 diffuse, specular, ambient; // Light colors
	VSOutVL vOut = (VSOutVL)0; // The output of the vertex shader
	
	// Move the vertex into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Step 1)	First to do lighting at all, we need the normal at each vertex position
	//			The normal we get passed is in the object's space so we have to rotate
	//			it so it's in world space.
	//			**NOTE** We are making an assumption that the 3x3 portion of the world
	//					 matrix only contains a pure rotation.  If there is any scaling 
	//					 factor in the world matrix, this won't work as is.
	normal = normalize(mul(normal, (float3x3)gWorldMat));
	
	// Step 2)	We need to calculate the following vectors.
	//			eyeVec: The normalized vector, in world space, from the vertex's position to the
	//					camera's eye position			
	//			lightVec: The normalized vector, in world space, from the vertex's position to the
	//					  light source's position
	//			halfwayVec: The vector loacated halfway between lightVec and eyeVec
	float3 lightVec = normalize(gLightPos - vOut.pos);
	float3 eyeVec = normalize(gEyePos - vOut.pos);
	float3 halfwayVec = normalize(eyeVec + lightVec);

	// Step 3)	Compute the diffuse component
	// Okay lets break down this equation.  First we get the dot product of the
	// vertex's normal and the light vector.  This will be the contribution of the 
	// diffuse color.  However, it could be a negative value if the normal was 
	// pointing away from the camera's eye (and therefore isn't visisble so we don't
	// need to light it), so we clamp it to 0. 
	diffuse = max(dot(normal, lightVec), 0) * gDiffuse;
		
	// Step 4)	Compute specular component
	// This one has just a bit more going on then the diffuse component.  First
	// we calculate the the dot product between the normal and the halfway vector.  Since
	// this could be negative if the normal is pointing away from the halfway vector (and
	// therefore isn't visisble so we don't need to light it), we clamp to zero.  This
	// gives an amount of how directly the light is shining back into our eye.  We then 
	// raise that to kShine.  What kShine does, is determine how big an area will receive
	// a shiny spot.  The bigger kShine, the smaller the area that gets lit by specular
	// lighting
	specular = pow(max(dot(normal, halfwayVec), 0), kShine) * gSpecular;
		
	// Step 5)	Compute ambient component
	// The ambient represents how much something is lit, when no direct light is striking
	// it.  This is easy to compute, we simply set the ambient to our desired ambient value.
	// **NOTE** We could just use "gAmbient" directly, this is done only for clarification 
	ambient = gAmbient;
	
	// Step 6)  Computer final vertex position
	// Currently the vertex is only in world space.  We need to multiply it
	// by the view matrix and projection matrix so it is in it's final resting place
	// for this frame
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat));
	
	// Step 6)  Set the lit color
	// This goes all the way to our equation on the top of win_main.cpp.  To set the 
	// final color value, just add the diffuse, specular, and ambient color contributions
	// **NOTE** Notice how we reference color as "color.rgb".  "color" is defined to be
	//			a float4, while "diffuse", "specular", and "ambient" are defined to be
	//			a float3 since we don't need to store an alpha with our light color.  We want
	//			to operate on "color" as if it was a float3.  By using the syntax "color.rgb"
	//			we only act on the first three components of "color" which is what we want %)
	vOut.color.rgb = (diffuse * color.rgb) + (specular * color.rgb) + (ambient * color.rgb);
		return vOut; // Return the output of the vertex shader
}

// The vertex shader for per-pixel lighting
VSOutPPL PerPixelLightingVS(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	VSOutPPL vOut = (VSOutPPL)0; // The output of the vertex shader
	
	// Move the vertex into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Step 1)	First to do lighting at all, we need the normal at each vertex position
	//			The normal we get passed is in the object's space so we have to rotate
	//			it so it's in world space.
	//			**NOTE** We are making an assumption that the 3x3 portion of the world
	//					 matrix only contains a pure rotation.  If there is any scaling 
	//					 factor in the world matrix, this won't work as is.
	vOut.normal = normalize(mul(normal, (float3x3)gWorldMat));
	
	// Step 2)	We need to calculate the following vectors.
	//			eyeVec: The normalized vector, in world space, from the vertex's position to the
	//					camera's eye position			
	//			lightVec: The normalized vector, in world space, from the vertex's position to the
	//					  light source's position
	//			halfwayVec: The vector loacated halfway between lightVec and eyeVec
	float3 eyeVec = normalize(gEyePos - vOut.pos.xyz); // Compute eyeVec
	vOut.lightVec = normalize(gLightPos - vOut.pos); // Compute and set the lightVec
	vOut.halfwayVec = normalize(eyeVec + vOut.lightVec); // Compute and set the halfwayVec
	
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat)); // Finishing placing position in correct space
	vOut.color = color; // Set the color
		return vOut; // Return the output of the vertex shader
}

// The pixel shader for per-pixel lighting.  Need a picture to see what's happening?
// Check out lighting.jpg for a high-level snapshot of vertex vs per-pixel lighting. 
PSOut PerPixelLightingPS(PSIn pIn)
{
	float3 diffuse, specular; // Light colors		
	PSOut pOut = (PSOut)0; // The output for the pixel shader
	
	// Step 1) We normalize all of our vectors.  This is because when you take the
	// dot product between two normalized vectors, you get the cosine of the angle
	// between the vectors.  Now you might be thinking, "Why do we normalize these 
	// vectors again?  Aren't they all ready normalized?"  It's true that each vector
	// starts out normalized since we do so in the vertex shader, however, since each
	// vector is being interpolated across the face of a polygon they have a tendency
	// to become unnormalized, thus we renormalize them to make sure.
	pIn.normal = normalize(pIn.normal);
	pIn.lightVec = normalize(pIn.lightVec);
	pIn.halfwayVec = normalize(pIn.halfwayVec);
	
	// Step 2)	Compute the diffuse component
	// Okay lets break down this equation.  First we get the dot product of the
	// vertex's normal and the light vector.  This will be the contribution of the 
	// diffuse color.  However, it could be a negative value if the normal was 
	// pointing away from the camera's eye (and therefore isn't visisble so we don't
	// need to light it), so we clamp it to 0. 
	diffuse = max(dot(pIn.normal, pIn.lightVec), 0) * gDiffuse;
	
	// Step 3)	Compute specular component
	// This one has just a bit more going on then the diffuse component.  First
	// we calculate the the dot product between the normal and the halfway vector.  Since
	// this could be negative if the normal is pointing away from the halfway vector (and
	// therefore isn't visisble so we don't need to light it), we clamp to zero.  This
	// gives an amount of how directly the light is shining back into our eye.  We then 
	// raise that to kShine.  What kShine does, is determine how big an area will receive
	// a shiny spot.  The bigger kShine, the smaller the area that gets lit by specular
	// lighting
	specular = pow(max(dot(pIn.normal, pIn.halfwayVec), 0), kShine) * gSpecular;
	
	// Step 4)  Set the lit color
	// This goes all the way to our equation on the top of win_main.cpp.  To set the 
	// final color value, just add the diffuse, specular, and ambient color contributions
	pOut.color.rgb = (diffuse * pIn.color.rgb) + (specular * pIn.color.rgb) + (gAmbient * pIn.color.rgb);
		return pOut; // Return the output of the pixel shader
}

/////////////////
// Techniques //
///////////////

// Our technique
technique VertexLighting
{
	// Declare our one render pass
	pass First
	{
		// Set render states
		ZEnable = TRUE; // We want z-buffering enabled
		VertexShader = compile vs_2_0 VertexLightingVS(); // Use vertex shader 2.0
	}
}

// Our technique
technique PerPixelLighting
{
	// Declare our one render pass
	pass First
	{
		// Set render states
		ZEnable = TRUE; // We want z-buffering enabled
		VertexShader = compile vs_2_0 PerPixelLightingVS(); // Use vertex shader 2.0
		PixelShader = compile ps_2_0 PerPixelLightingPS(); // Use pixel shader 2.0
	}
}


	