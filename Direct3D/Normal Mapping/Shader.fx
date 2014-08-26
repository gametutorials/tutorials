////////////////
// Constants //
//////////////

// Specular's light contribution to the final lit pixel
const int kSpecPower = 100;

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

bool gUseNormalMap;

texture gDiffuseMap;
texture gNormalMap;

// Here's a fun little piece of syntax.  If you are dabbling with pixel shaders
// then you should be aware of the concept of mipmaps (if not go check out some
// GameTutorials texturing tutorials and come right back!).  This defines the mipmap 
// flags and what texture they apply to.
sampler DiffuseMapSampler = sampler_state
{
	// Sampler's are not something you get at AppleBee's, they are rather
	// a slick way to tie together a texture and all of it's sampling states.
	// We'll use this in our shader to easily turn our UV coordinates into a
	// color  

	Texture = (gDiffuseMap); // The texture
	
	// Mipmapping flags
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

// Texture sampler for the normal map
sampler NormalMapSampler = sampler_state
{
	Texture = (gNormalMap); // The texture
	
	// Mipmapping flags
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

// Input to the vertex shader
struct VSIn
{
	float3 pos : POSITION;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal : NORMAL;
	float2 tc : TEXCOORD0;
};

// Out from the vertex shader
struct VSOut
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float3 lightVec : TEXCOORD0;
	float3 halfwayVec : TEXCOORD1;
	float2 tc : TEXCOORD2;
};

// Input to the pixel shader
struct PSIn
{
	float3 lightVec : TEXCOORD0;
	float3 halfwayVec : TEXCOORD1;
	float2 tc : TEXCOORD2;
};

// Output from the pixel shader
struct PSOut
{
	float4 color : COLOR; // Color of pixel to be drawn to framebuffer
};

// The vertex shader
VSOut NormalMapVertexShader(VSIn vIn)
{
	VSOut vOut; 
			
	// Move the position into world space
	vOut.pos = mul(float4(vIn.pos, 1), gWorldMat);
	
	// [-- Step 1 --]
	// Move our tangent vectors into world space.  If the
	// cube wasn't moving, we could skip this step
	vIn.tangent = mul(vIn.tangent, (float3x3)gWorldMat);
	vIn.binormal = mul(vIn.binormal, (float3x3)gWorldMat);
	vIn.normal = mul(vIn.normal, (float3x3)gWorldMat);
	
	// [-- Step 2 --]
	// Create the tangent matrix.  Notice how each axis in our tangent 
	// space basis becomes a column in the matrix.
	float3x3 tanMat = { vIn.tangent.x, vIn.binormal.x, vIn.normal.x,
						vIn.tangent.y, vIn.binormal.y, vIn.normal.y,
						vIn.tangent.z, vIn.binormal.z, vIn.normal.z };
	
	// [-- Step 3 --]
	// We need to calculate the following vectors.
	// wLightVec is the vector, in world space, from the vertex to the light source
	// wEyeVec is the vector, in world space, from the vertex to the camera's eye position
	float3 wLightVec = gLightPos - vOut.pos;
	float3 wEyeVec = gEyePos - vOut.pos;
	
	/// [-- Step 4 --]
	// Now we need to calculate the vectors we need to pass to the pixel shader
	// We want them to be in tangent space so we have to multiply them by the 
	// tangent matrix.
	// lightVec is the vector, in tangent space, from the vertex to the light source 
	// halfwayVec is a vector, in tangent space, loacated halfway between wLightVec and wEyeVec					
	vOut.lightVec = normalize(mul(wLightVec, tanMat));
	vOut.halfwayVec = normalize(mul((wEyeVec + wLightVec), tanMat));
	
	// [-- Step 5 --]
	// Finish multiplying position by the view and projection matrix,
	// also pass the texture coordinates unmodified 
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat));
	vOut.tc = vIn.tc;
		return vOut; // Return the modified vertex
}

// The pixel shader
PSOut NormalMapPixelShader(PSIn pIn)
{
	PSOut pOut;
	float3 diffuse, specular; // Light colors
	
	float4 color = tex2D(DiffuseMapSampler, pIn.tc);
	float4 normal = tex2D(NormalMapSampler, pIn.tc);
	
	// [-- Step 1 --]
	// Adjust normal from (0,1) to (-1, 1)
	normal = (normal - 0.5f) * 2.0f;
	
	// [-- Step 2 --]
	// Normalize the vector
	normal = normalize(normal);
	
	// [-- Step 3 --]	
	// Compute the diffuse component.  First we get the dot product of the
	// normal obtained from the normal map and the light vector.  This will be the 
	// contribution of the diffuse color.  However, it could be a negative value if 
	// the normal was pointing away from the camera's eye (and therefore isn't visisble 
	// so we don't need to light it), so we clamp it to 0. 
	diffuse = max(dot(normal, pIn.lightVec), 0) * gDiffuse;
		
	// [-- Step 4 --]	
    // Compute the specular component.  First we calculate the the dot product between the 
    // normal obtained from the normal map and the halfway vector.  Since this could be 
    // negative if the normal is pointing away from the halfway vector (and
	// therefore isn't visisble so we don't need to light it), we clamp to zero.  This
	// gives an amount of how directly the light is shining back into our eye.  We then 
	// raise that to kSpecPower.  kSpecPower is a constant that determines how big an area 
	// will receive a shiny spot.  The bigger kSpecPower, the smaller the area that 
	// gets lit by specular lighting
	specular = pow(max(dot(normal, pIn.halfwayVec), 0), kSpecPower) * gSpecular;
		
	// [-- Step 5 --]
	// If we are using normal mapping, set the lit color
	if(gUseNormalMap)
		pOut.color.rgb = (diffuse * color.rgb) + (specular * color.rgb) + (gAmbient * color);
	else
		pOut.color = color; // Else just use the diffuse color directly
		
	return pOut;
}

/////////////////
// Techniques //
///////////////

// Our technique
technique NormalMapping
{
	// Declare our one render pass
	pass First
	{
		// Set render states
		Lighting = FALSE;
		ZEnable = TRUE; // We want z-buffering enabled
		Sampler[0] = (DiffuseMapSampler); // Set sampler
		Sampler[1] = (NormalMapSampler); // Set sampler  
		
		// Compile and set the vertex and pixel shaders
		VertexShader = compile vs_2_0 NormalMapVertexShader(); // Use vertex shader 2.0
		PixelShader = compile ps_2_0 NormalMapPixelShader(); // Use pixel shader 2.0
	}
}


	