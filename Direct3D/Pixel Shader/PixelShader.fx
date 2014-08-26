/*
	This is our effect file.  You might be saying, "What is an effect?" so lets 
	go over the definition.  A D3D effect is an interface which intergates vertex shaders,
	pixel shaders, and render state for drawing objects.  The effects implemention lies
	inside of an effect file like this.
	
	The code you'll see in this file is called HLSL.  You can write shaders for DirectX without
	using effect files, however, effect files are the next logical step in shader technology, 
	so this is why we're starting by using them.
	
	Overall HLSL is a very 'C' looking language and if you versed at all in C/C++ should 
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
texture gTexture; // The texture to use when drawing with the pixel shader 

// Here's a fun little piece of syntax.  If you are dabbling with pixel shaders
// then you should be aware of the concept of mipmaps (if not go check out some
// GameTutorials texturing tutorials and come right back!).  This defines the mipmap 
// flags and what texture they apply to.
sampler TextureSampler = sampler_state
{
	// Sampler's are not something you get at AppleBee's, they are rather
	// a slick way to tie together a texture and all of it's sampling states.
	// We'll use this in our shader to easily turn our UV coordinates into a
	// color  

	Texture = (gTexture); // The texture
	
	// Mipmapping flags
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

// This will hold our vertex data after it's been processed
// by the vertex shader
struct VSOut
{
	// First a quick look at the 3D pipeline:
	/*
		Model space vertex data --------> |
										  |---> Vertex ------> Pixel -------> *Rendered*
										  |---> Processing --> Processing --> *Pixels*
		Primitive data / Tesselation ---> |
	
		A vertex shader executes when "Vertex Processing" happens and a pixel shader
		exectues when "Pixel Processing" is happening.  The output of the vertex shader
		is used as the input to the pixel shader, so we label our struct "VSOut" because
		it is what is calculated and then outputted from the vertex shader.
	*/
	
	// Next on the list, what is a float4?  A float4 is simply an array of four floating
	// point numbers.  So a C++ similar declartion would be:  float pos[4];
	
	// Lastly what is with the : POSITION and : TEXCOORD?  These are called semantics.
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
	float2 tex : TEXCOORD; // Texture coordinates after vertex shader has executed
};

// This contains what is output by the pixel shader.  It will contain the 
// color that is drawn to the screen
struct PSOut
{
	float4 color : COLOR; // The pixel color to be drawn to the screen
};

/////////////////////
// Vertex Shader //
///////////////////

VSOut TheVertexShader(float3 pos : POSITION, float2 texCoords : TEXCOORD)
{
	VSOut vOut;
	
	// Create the worldViewProjection matrix
	float4x4 worldViewProjMat = mul(gWorldMat, mul(gViewMat, gProjMat)); 
	
	// Put vertex position in screen space
	vOut.pos = mul(float4(pos, 1), worldViewProjMat);
	vOut.tex = texCoords; // Just pass the texture coordinates as they are
		return vOut;
}

///////////////////
// Pixel Shader //
/////////////////

// The pixel shader will determine what color to draw to the framebuffer,
// thus the only data it needs is the interpolated texture coordinates
// of each pixel to be drawn, so that's what we pass it
PSOut ThePixelShader(float2 uv : TEXCOORD)
{
	PSOut pOut; // What will be outputted from the pixel shader
	
	// Here with the tex2D(), a function which performs a texture 2D lookup,
	// we can easily get which color to draw.
	// By parameter:
	// TextureSampler -- A sampler (defined above) that defines where to get the color from
	// uv -- The UV coordinates to use to get the color
	pOut.color = tex2D(TextureSampler, uv);
		return pOut; // Return the pixel shader output
}

/////////////////
// Techniques //
///////////////

// This is the effect file techinque.  This is what we reference when we want to 
// to run a shader from our renderer.  To declare a technique, you use the
// technique keyword and then give it a user define name, such as "SingleTexture"
technique SingleTexture
{
	// Modern day video cards of capable of several render passes.  Here we write the
	// code for each pass if there is more than one.  Note you use the keyword "pass" to
	// declare a new pass and then a user defined name to name the pass.
	pass First
	{
		Lighting = FALSE; // Turn off lights
		ZEnable = TRUE; // Turn on the Z-buffer
		
		// **NOTE** Notice the parathesis around TextureSampler.  These aren't there
		//			because we like to showcase our fast ability at typing ()'s, they
		//			are actually required %)
		Sampler[0] = (TextureSampler); // Set sampler
		
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
		
		// Lastly we set our pixel shader render state.  Lets look at the statement
		// a bit closer:
		// "PixelShader" -- The render state we're setting, similar to "Lighting" or "ZEnable"
		// "compile" -- Compiles the shader naturally %)
		// "ps_2_0" -- There are several versions of pixel shaders.   These versions
		//			   correspond to the power of your video card.  Lower versions could only
		//			   have so many instructions per shader and other restrictions.  There are
		//			   higher versions like ps_3_0.  ps_2_0 is the lowest supported currently
		//			   and should be safe on any card that supports shaders. ps_1_1 is not longer supported.
		// ThePixelShader() -- The name of the shader to compile.  Note the () at the end of 
		//					   the user defined name
		PixelShader = compile ps_2_0 ThePixelShader();
	}
}

/*
	Shaders are a entirely new language and definitely the future of graphic programming.  
	This tutorial barely scratches the surface of D3D shaders.  For more information on 
	D3D shaders head to MSDN and type in "HLSL Shader Reference" or try the below link
	http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_Summer_04/directx/graphics/reference/hlslreference/hlslreference.asp
*/   

