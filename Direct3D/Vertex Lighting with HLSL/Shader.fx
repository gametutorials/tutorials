/*
	Here's what's making this tutorial a brighter place %)
	
	What's the crux of this lighting stuff you ask?  Well it all comes down to dot products.
	Lets do a quick look at math and see how all the magic happens.
	
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
			
	So back to answering the question about the lighting stuff.  For diffuse lighting, what
	happens is we take a vector from the vertex to the light and call it the light vector.
	We then take the dot product of the light vector with the normal at the vertex.  If the angle
	formed is less than 90°, the dot product will give us a number between (0,1) we will use 
	it as a percentage of how much the light is striking the vertex.
	
	A similar concept applies when calculating specular lighting which is detailed completely
	in the shader.
*/

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
// by the vertex shader
struct VSOut
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
};

// The vertex shader
// Inside we will break down step, by step, what needs to be done to calculate
// the each vertex's lit color.  To get a visual representation of what's going on
// check out the accompaning diffuse, specular, and ambient JPEGs.  
VSOut TheVertexShader(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	// Shine, shine everybody shine... This constant is used in calculating the
	// specular's contribution to the final lit pixel.
	const int kShine = 32;
	
	float3 diffuse, specular, ambient; // Light colors
	VSOut vOut = (VSOut)0; // To zero out a struct when declaring it, this 
						  // syntax is used
			
	// Put the vertex into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Step 1)	First to do lighting at all, we need the normal at each vertex position
	//			The normal we get passed is in the object's space so we have to rotate
	//			it so it's in world space.
	//			**NOTE** We are making an assumption that the 3x3 portion of the world
	//					 matrix only contains a pure rotation.  If there is any scaling 
	//					 factor in the world matrix, this won't work as is.
	normal = mul(normal, (float3x3)gWorldMat);
	
	// Step 2)	We need to calculate the following vectors.
	//			lightVec is the vector, in world space, from the vertex to the light source
	//			eyeVec is the vector, in world space, from the vertex to the camera's eye position
	//			halfwayVec is a vector loacated halfway between lightVec and eyeVec
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
	
	// Setp 6)  Multiply our vertex by the view matrix and projection matrix
	// We've only multiplied the vertex by the world matrix.  We need to mutiply by
	// the view and projection matrix so the full transforamtion from object space to
	// the viewing screen can take place.
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat));
	
	// Step 7)  Set the lit color
	// This goes all the way to our equation on the top of win_main.cpp.  To set the 
	// final color value, just add the diffuse, specular, and ambient color contributions
	// **NOTE** Notice how we reference color as "color.rgb".  "color" is defined to be
	//			a float4, while "diffuse", "specular", and "ambient" are defined to be
	//			a float3 since we don't need to store an alpha with our light color.  We want
	//			to operate on "color" as if it was a float3.  By using the syntax "color.rgb"
	//			we only act on the first three components of "color" which is what we want %)
	vOut.color.rgb = (diffuse * color.rgb) + (specular * color.rgb) + (ambient * color.rgb);
		return vOut; // Return the modified vertex
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
		VertexShader = compile vs_2_0 TheVertexShader(); // Use vertex shader 2.0
	}
}


	