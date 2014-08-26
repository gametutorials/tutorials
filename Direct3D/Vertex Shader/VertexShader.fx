/*
	This is our effect file.  You might be saying, "What is an effect?" so lets 
	go over the definition.  A D3D effect is an interface which intergates vertex shaders,
	pixel shaders, and render state for drawing objects.  The effects implemention lies
	inside of an effect file like this.
	
	The code you'll see in this file is called HLSL.  You can write shaders for DirectX without
	using effect files, however, effect files are the next logical step in shader technology, 
	so this is why we're starting by using them.
	
	Overall HLSL is a very 'C' looking language and if you are versed at all in C/C++, it should 
	be something you can pick up in little time.  Lets go through the effect file, line by
	line.
*/   


//////////////
// Globals //
////////////

/*
	First off what's a float4x4.  Well as the name suggest a float4x4 is a 4x4 matrix of floats
	A similar C++ declaration would be:  float gProjMat[4][4];
*/
float4x4 gProjMat; // The projection matrix (Project geometery from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)

////////////////////////
// Vertex Definitons //
//////////////////////

// This will hold our vertex data after it's been processed
// by the vertex shader
struct VSOut
{
	// First a quick picture of the 3D pipeline
	/*
		Model space vertex data --------> |
										  |---> Vertex ------> Pixel -------> *Rendered*
										  |---> Processing --> Processing --> *Pixels*
		Primitive data / Tesselation ---> |
	*/
	
	// Next on the list what is a float4?  A float4 is simply an array of four floating
	// point numbers.  So a C++ similar declartion would be:  float pos[4];
	
	// Lastly what is with the : POSITION and : COLOR?  These are called semantics.
	// A semantic is a identifier that specifies what the variable is going to be used for.
	// If you think about it, it makes sense that this has to happen in some way.  We're sending
	// the graphics card data, but it has no idea what that data is.  Two ways come to mind, either
	// have hardcoded variables names to specify what the data is (GLSL goes this route) or tag
	// each variable with an identifier so the GPU knows what it is.  This is the route D3D went,
	// and they call this tagging convesion, "semantics".  For more information on semantics, 
	// check out the bottom of this file.

	// The syntax for a semantic is as follows:
	// float4 pos : POSITION;
	//   |	   |  |		|
	//	 |	   |  |		|--> semantic
	//	 |	   |  |
	//	 |	   |  |--> A colon ':'
	//   |	   |
	//   |	   |--> variable name, you pick the name as with any other variable
	//   |
	//   |--> variable type, the type of variable to declare, HLSL it's own set of data types
	  
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
};

/////////////////////
// Vertex Shaders //
///////////////////

// Here we declare our vertex shader.  Lets break down each piece of the declaration:
// "VSOut" -- This is the return type of the shader, very similar to the return type 
//			  from a C/C++ function
// "TheVertexShader" -- This is the name of the shader, very similar to the user defined 
//						name of a C/C++ function
// "float3 pos : POSITION" -- This is the first argument being passed to the shader.  It is 
//							  of type float3 (an array of 3 floats).  It's user defined name
//							  is "pos" and it's been identifed as a position, by it's POSITION
//							  semantic
// float4 color : COLOR -- This is the second argument being passed to the shader.  It is
//						   of type float3.  It's user defined name is "color" and it's been identified
//						   as a color, by it's COLOR semantic 
VSOut TheVertexShader(float3 pos : POSITION, float4 color : COLOR)
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
	
	// Now we multiply our verts by the "worldViewProjMat" to put it in screen
	// space.  Notice how we have to make it a four element vector, so that we
	// can mathamatically perform the multiplication operation.
	vOut.pos = mul(float4(pos, 1), worldViewProjMat);
	vOut.color = color; // Just set the color as is
		return vOut;
}

/////////////////
// Techniques //
///////////////

// This is the effect file techinque.  This is what we reference when we want to 
// to run a shader from our renderer.  To declare a technique, you use the
// technique keyword and then give it a user define name.  Here we pick "DiffuseColor"
technique DiffuseColor
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
	Shaders are a entirely new language and definitely the future of graphic programming.  
	This tutorial barely scratches the surface of D3D shaders.  For more information on 
	D3D shaders head to MSDN and type in "HLSL Shader Reference" or try the below link
	http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_Summer_04/directx/graphics/reference/hlslreference/hlslreference.asp
*/   

