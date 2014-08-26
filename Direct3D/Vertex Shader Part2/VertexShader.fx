/*
	A D3D effect is an interface which intergates vertex shaders, pixel shaders, 
	and render state for drawing objects.  This file is the implmentation of 
	our effect.  If HLSL syntax is a bit fuzzy, check out the refresher at the bottom
*/

////////////////
// Constants //
//////////////

// Define our max green and blue color
// **NOTE** Colors are defined in RGBA form
const float4 kGreen = { 0.0f, 200.0f / 255.0f, 0.0f, 1.0f }; 
const float4 kBlue = { 0.0f, 0.0f, 200.0f / 255.0f, 1.0f };

//////////////
// Globals //
////////////

float4x4 gProjMat; // The projection matrix (Project geometery from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)
float gTime; // A time value that oscillates between 0.0f and 1.0f

////////////////////////
// Vertex Definitons //
//////////////////////

// This will hold our vertex data after it's been processed
// by the vertex shader
struct VSOut
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
};

/////////////////////
// Vertex Shaders //
///////////////////

// Here we declare our vertex shader.  Lets break down each piece of the declaration:
// "VSOut" -- This is the return type of the shader, very similar to the return type 
//				  from a C/C++ function
// "TheVertexShader" -- This is the name of the shader, very similar to the user defined 
//						name of a C/C++ function
// "float3 pos : POSITION" -- This is the first argument being passed to the shader.  It is 
//							  of type float3 (an array of 3 floats).  It's user defined name
//							  is "pos" and it's been identifed as a position, by it's POSITION
//							  semantic
// float3 normal : NORMAL -- This is the second argument being passed to the shader.  It's user
//							 defined name is "normal" and it's been identifed as a vertex normal
//							 by it's NORMAL semantic.
// "float4 color : COLOR" -- This is the second argument being passed to the shader.  It is
//						     of type float3.  It's user defined name is "color" and it's been identified
//						     as a color, by it's COLOR semantic 
VSOut TheVertexShader(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	VSOut vOut;
			
	// Here we perform the math to move our vertices from world space
	// to screen space.  First we multiply the world matrix by the view matrix
	// then multiply that by the projection matrix.  This creates the a matrix
	// that will translate our vertices from world space to screen space.
	
	// **NOTE** the mul() operator performs matrix multiplication.  If the first 
	//			argument happens to be a vector (such as float4, it treats it as a row vector)
	//			If the second argument is a vector, it treats it as a column vector 
	float4x4 worldViewProjMat = mul(mul(gWorldMat, gViewMat), gProjMat);
	
	// First we modify the position of the vertex by moving it along it's normal.
	// **NOTE** To scale the box, we need to modify it's position BEFORE we apply
	//			any 3D transformations.
	pos += normal * gTime;
	
	// Then we modify the color by multipling it by the current linearly interpolated
	// color between our kGreen and kBlue.
	// Not sure what lerp (linear interopolation) is?  Check out the refresher at
	// the bottom of this file
	color *= lerp(kGreen, kBlue, gTime);
	
	// Now we multiply our verts by the "worldViewProjMat" to put it in screen
	// space.  Notice how we have to make it a four element vector, so that we
	// can mathamatically perform the multiplication operation.
	vOut.pos = mul(float4(pos, 1), worldViewProjMat);
	vOut.color = color; // Set the modified color
		return vOut;
}

/////////////////
// Techniques //
///////////////

// This is the effect file techinque.  This is what we reference when we want to 
// to run a shader from our renderer.  To declare a technique, you use the
// technique keyword and then give it a user define name.  Here we pick "DiffuseColor"
technique DiffuseColorWithScaling
{
	// Modern day video cards of capable of several render passes.  Here we write the
	// code for each pass if there is more than one.  Note you use the keyword "pass" to
	// declare a new pass and then a user defined name to name the pass.
	pass First
	{
		// Here is the beauty of effect files.  They handle render state change for
		// you.  AND once there done, they put all the render states back the way
		// they were before you called the technique.  Below we set our two render
		// states.
		Lighting = FALSE; // We don't want to use vertex lighting
		ZEnable = TRUE; // We want z-buffering enabled
		
		// Here we set our vertex shader render state.  Lets look at the statement
		// a bit closer:
		// "VertexShader" -- The render state we're setting, same as "Lighting" and "ZEnable"
		// "compile" -- Compiles the shader naturally %)
		// "vs_2_0" -- There are already several versions of vertex shaders.   These versions
		//			   correspond to the power of your video card.  Lower versions could only
		//			   have so many instructions per shader and other restrictions.  The most
		//			   current version is vs_3_0.  However, vs_2_0 is kinda in the middle and
		//			   should compile on any decent (for 2005) video card.
		// TheVertexShader() -- The name of the shader to compile.  Note the () at the end of 
		//						the user defined name
		VertexShader = compile vs_2_0 TheVertexShader();
	}
}

/* 
	Shader Refresher

	Here's a quick refresher for some HLSL syntax:
	
	1) Semantics
	
	A semantic is an identifier for a variable, specifing additional type
	information about the variable.
	
	Example: 
	
	// float4 pos : POSITION;
						|
						|--> semantic that says this variable "pos" representaions a position
						
	2) float4x4
	
	A float4x4 is a data type that is a 4x4 array of floats.  It's C++ 
	equivalent declaration would look like this: float anArray[4][4];
	
	3) LERP (Linear Interpolation)
	
	Linear interpolation calculate a new value inbetween two known values, using the line
	between those two values
	
	Example
	
	A------------B  If A and B are both points and we say we start at time 0.0f at A and
					end at time 1.0f at B, we can calculate any point on the line from A to B
					if we have a time t such that (t >= 0.0f and t <= 1.0f)
					
	HLSL lerp() can take scalars, vectors, or matrices as arguments and is
	defined as:  ret lerp(x, y, t)
	It returns x + t(y - x).  This linearly interpolates between x and y,
	such that the return value is x when t is 0, and y when t is 1.
*/