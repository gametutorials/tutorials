/*
	This where the rubber meets the road and all the toon-shading greatness unfolds.  A little
	bit of linear algerbra knowledge will be helpful in digesting the finer points of the
	algorithim.
	
	Lets look at ASCII versions of 1D textures and explain the ideas behind toon shading.
	
	First we mentioned that toon-shaded objects have a black outline for the border.  The
	texture used for this is 16x1 pixels and consists of black and white pixels arranged
	like this:
	
	BBBBWWWWWWWWWWWW	16x1 pixels with B = (0,0,0) and W = (255, 255, 255)
	0			   1	The texture coordinate range
	
	We take a vector from the vertex being render to the eye and dot it with the vertex normal.
	We ignore negative dot products because a negative dot product means the normal is pointing
	away from the eye so we can't see the vertex.  A dot product of 1 would mean we were looking
	directly at the vertex and 1 in our texture would give us white.  As we start to look 
	less directly at the vertex the dot product goes towards 0.  Conceptually when only 25% or less
	of the vertex is directly in our sight, the texture look up will return black.  This is how
	the edges, the portions of the object that are barely in our eye sight, get turned to black.
	
	Moving on...
	
	The texture used for shading is 16x1 pixels and is made up of gray, light gray, and white pixels
	arranged like this:
	
	GGGGLLLLLLLLWWWW	16x1 pixels with G = (192, 192, 192), L = (224, 224, 224), and W (255, 255, 255)
	0			   1	The texture coordinate range
	
	We take a vector from the vertex being render to the light position and dot it with the vertex normal.
	We ignore negative dot products because a negative dot product means the normal is pointing
	away from the light so it will not get lit.  A dot product of 1.0 would the light is directly striking
	the vertex while a dot product of 0.0 would mean the light is perpendicular to the vertex and not 
	lighting it at all.  Conceptually when only 75% or more of the light strikes the vertex
	we return full white, when that texture color is modulated with the vertex color we will get 
	back the orginal color unmodified.  When 25-75% of the light is directly striking the vertex, 
	we return light grey.  When modulated with the vertex color, this will slightly darken the final color.
	Lastly, if 25% or less of the light is striking the vertex we return darker grey.  This will darken the
	the final color even more than just light grey.
*/

//////////////
// Globals //
////////////

float4x4 gProjMat; // The projection matrix (Project geometry from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)

float3 gLightPos; // The world position of the light
float3 gEyePos; // The world position of the eye of the camera

texture gShadeTexture; // The texture to use when shading the object
texture gEdgeTexture; // The texture to use when coloring the edges of the object

// The texture sampler for our shade texture
sampler ShadeSampler = sampler_state
{
	// Samplers are not something you get at AppleBee's, they are rather
	// a slick way to tie together a texture and all of it's sampling states.
	// We'll use this in our shader to easily use our UV coordinates to look 
	// up a color
	Texture = (gShadeTexture);
};

// The texture sampler for our edge texture
sampler EdgeSampler = sampler_state
{
	Texture = (gEdgeTexture);
};

// This will be the output from our vertex shader
struct VSOut
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
	float shade : TEXCOORD; // Lookup into shade texture
	float edge : TEXCOORD1; // Lookup into edge texture 
};

// This is the input for the pixel shader
struct PSIn
{
	float4 color : COLOR; // Vertex color coming into the pixel shader
	float shade : TEXCOORD; // Lookup into shade texture
	float edge : TEXCOORD1; // Lookup into edge texture 
};

// This is the output of the pixel shader and is what will be 
// drawn to the framebuffer
struct PSOut
{
	float4 color : COLOR; // The final pixel's color
};

// The vertex shader
VSOut TheVertexShader(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	VSOut vOut = (VSOut)0; // Zero out return value from vertex shader
			
	// Move the position into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Step 1)	First to do toon-shading at all, we need the normal at each vertex position
	//			The normal we get passed is in the object's space so we have to rotate
	//			it so it's in world space.
	//			**NOTE** We are making an assumption that the 3x3 portion of the world
	//					 matrix only contains a pure rotation.  If there is any scaling 
	//					 factor in the world matrix, this won't work as is.
	normal = mul(normal, (float3x3)gWorldMat);
	
	// Step 2)	We need to calculate the following vectors.
	//			lightVec: The normalized vector, in world space, from the vertex to the light source
	//			eyeVec: The normalized vector, in world space, from the vertex to the camera's eye position
	float3 lightVec = normalize(gLightPos - vOut.pos);
	float3 eyeVec = normalize(gEyePos - vOut.pos);
		
	// Step 3)  Now we can compute our first set of texture coordinates
	//			Lets break down this equation.  First we take a dot product
	//			between the vertex's normal and "lightVec", this will return 
	//			a number between (-1, 1) since both "normal" and "lightVec" are
	//			unit length.  We use this number as a direct UV lookup into our
	//			texture.  Now UV coordinates are in the range of (0, 1) so that is
	//			where the max() comes to play.  If the dot product is negative, 
	//			we clamp it to zero. 
	vOut.shade = max(dot(normal, lightVec), 0);
	
	// Step 4)	We compute the second set of texture coordinates in a similar way
	//			Lets break down this equation.  First we take a dot product
	//			between the vertex's normal and "eyeVec", this will return 
	//			a number between (-1, 1) since both "normal" and "eyeVec" are
	//			unit length.  We use this number as a direct UV lookup into our
	//			texture.  Now UV coordinates are in the range of (0, 1) so that is
	//			where the max() comes to play.  If the dot product is negative, 
	//			we clamp it to zero. 
	vOut.edge = max(dot(normal, eyeVec), 0);
	
	// Step 5) Put the vertex into it's final space by multipling it by the view and 
	//		   projection matrix
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat));
	
	// Step 6)  Last step is to simply pass our color unmodified to the pixel shader
	vOut.color = color;
		return vOut; // Return the output of the vertex shader
}

// The pixel shader
PSOut ThePixelShader(const PSIn pIn)
{
	PSOut pOut = (PSOut)0; // Zero out return value from pixel shader
	
	// Using the texture coordinates we calculated in the vertex shader
	// we do a 1D texture look up into our edge and shade sampler and multiply those colors
	// by our vertex color to get the final color	
	pOut.color = tex1D(EdgeSampler, pIn.edge) * tex1D(ShadeSampler, pIn.shade) * pIn.color;
		return pOut; // Return the output of the pixel shader
}

/////////////////
// Techniques //
///////////////

// Our technique
technique ToonShading
{
	// Declare our one render pass
	pass First
	{
		// Set render states
		ZEnable = TRUE; // We want z-buffering enabled
		
		// Set the samplers to the texture's we want sampled
		Sampler[0] = (ShadeSampler); 
		Sampler[1] = (EdgeSampler);
		
		VertexShader = compile vs_2_0 TheVertexShader(); // Use vertex shader 2.0
		PixelShader = compile ps_2_0 ThePixelShader(); // Use pixel shader 2.0
	}
}


	