////////////////
// Constants //
//////////////

// Exponent to raise the specular contribution to when doing
// per-pixel lighting
const int kShine = 64;

// Amount to offset the Z-Depth value to help avoid z-fight and z-achne 
const float kShadowEpsilon =  0.0005f;

//////////////
// Globals //
////////////

float4x4 gLightViewMat; // View matrix for the light
float4x4 gLightProjMat; // Projection matrix for the light

float4x4 gProjMat; // The projection matrix (Project geometery from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gInvViewMat; // The inverse view matrix
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)

float3 gLightPos; // The world position of the point light
float3 gEyePos; // The world position of the eye of the camera

float3 gDiffuse; // The diffuse color of the point light
float3 gSpecular; // The specular color of the point light
float3 gAmbient; // The ambient color of the point light

texture gTexture; // The texture (aka the shadow map!)

sampler TextureSampler = sampler_state
{
	Texture = (gTexture); // The texture
	
	// Mipmapping flags
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// ******************************************** //
// *** Per pixel lighting with shadow maps *** //
// ****************************************** //

// The output from the pixel shader containing the lit pixel to be
// drawn in the framebuffer
struct PSOut
{
	float4 color : COLOR;
};

struct VSOut
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
	float3 normal : TEXCOORD0; // Vertex normal after vertex shader has executed
	float3 lightVec : TEXCOORD1; // The vector from the vertex in world space to the 
								// the light source
	float3 halfwayVec : TEXCOORD2; // The vector halfway between the "lightVec" described above
								  // and the vector from the vertex in world space to 
								 // the eye of the camera
	float4 ltVertPos : TEXCOORD3; // Position of vertex in light space
};

struct PSIn
{
	float3 normal : TEXCOORD0; // Vertex normal after vertex shader has executed
	float3 lightVec : TEXCOORD1; // The vector from the vertex in world space to the 
								// the light source
	float3 halfwayVec : TEXCOORD2; // The vector halfway between the "lightVec" described above
								  // and the vector from the vertex in world space to 
								 // the eye of the camera
	float4 ltVertPos : TEXCOORD3; // Position of vertex in light space
	
	float4 color : COLOR; // Color	
};

// The vertex shader for per-pixel lighting and shadow mapping
VSOut PerPixelLitWithShadowMapVS(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	float3 diffuse, specular, ambient; // Light colors
	VSOut vOut = (VSOut)0; // To zero out a struct when declaring it, this 
						  // syntax is used
			
	// Put the vertex into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Rotate normal into world space
	vOut.normal = mul(normal, (float3x3)gWorldMat);
	
	// Calculate vector from vertex to the light source
	vOut.lightVec = normalize(gLightPos - vOut.pos);
	
	// Calculate the vector halfway between the vector from the vertex to the camera's pos and
	// the lightVec
	vOut.halfwayVec = normalize(normalize(gEyePos - vOut.pos) + vOut.lightVec);
	
	// Put vertex into view space
	vOut.pos = mul(vOut.pos, gViewMat);
	
	// Calculate the vertex position in light space
	vOut.ltVertPos = mul(vOut.pos, mul(gInvViewMat, mul(gLightViewMat, gLightProjMat)));
	
	// Put vertex into projection space
	vOut.pos = mul(vOut.pos, gProjMat);
		
	// Set the final color
	vOut.color = color;
		return vOut; // Return the modified vertex
}

// The pixel shader for per-pixel lighting and shadow mapping
PSOut PerPixelLitWithShadowMapPS(PSIn pIn)
{
	PSOut pOut = (PSOut)0;
	
	// Renormalize the vertex normal
	pIn.normal = normalize(pIn.normal);
			
	// Calculate diffuse light color
	float3 diffuse = max(dot(pIn.normal, pIn.lightVec), 0) * gDiffuse;
		
	// Calculate specular light color
	float3 specular = pow(max(dot(pIn.normal, pIn.halfwayVec), 0), kShine) * gSpecular;
	
	// Calculate lit color
	pOut.color.rgb = (diffuse * pIn.color.rgb) + (specular * pIn.color.rgb) + (gAmbient * pIn.color.rgb);
	pOut.color.a = 1.0f;
		
	// Calculate UVs into the shadow map from the vertex's position in light space.  This
	// is the same way you compute projected texture coordinates 
    float2 shadowMapUV = 0.5f * pIn.ltVertPos.xy / pIn.ltVertPos.w // Scale
						 + float2(0.5f, 0.5f); // Bias by (0.5,0.5)
    
    // We have to flip the 'v' coordinate
    shadowMapUV.y = 1.0f - shadowMapUV.y;
    
    // Check to make sure the UV's are within 0 - 1 to avoid back projections
    if(shadowMapUV.x >= 0 && shadowMapUV.x <= 1.0f && shadowMapUV.y >= 0.0f && shadowMapUV.y <= 1.0f)
    {
		// Get the shadow value
		float shadowVal = tex2D(TextureSampler, shadowMapUV);
	
		// If the pixel is in a shadow region
		if((shadowVal + kShadowEpsilon) < (pIn.ltVertPos.z / pIn.ltVertPos.w))
			pOut.color *= (shadowVal * 0.75f);
	}
	
	return pOut;
}

// ****************************** //
// *** Shadow Map generation *** //
// **************************** //

struct VSShadowIn
{
	float3 pos : POSITION;
};

struct VSShadowOut
{
	float4 pos : POSITION;
	float2 depth : TEXCOORD0;
};

// Simple vertex shader for moving vertex into light projection space
VSShadowOut ShadowMapVS(float3 pos : POSITION)
{
	VSShadowOut vOut;

    // Move the vertex into light projection space 
	vOut.pos = mul(float4(pos, 1), mul(gWorldMat, mul(gLightViewMat, gLightProjMat)));

    // Store z and w
    vOut.depth.xy = vOut.pos.zw;
		return vOut;
}	

// The "color" of the texture is the z-value of the vertex 
PSOut ShadowMapPS(VSShadowOut pIn)
{
	PSOut pOut;
    pOut.color = pIn.depth.x / pIn.depth.y; // Save off the depth
		return pOut;
}


/////////////////
// Techniques //
///////////////

// Renders the geometery with per pixel lighting and shadow mapping
technique PerPixelLightingWithShadowMap
{
	pass First
	{
		// *** Set render states ***
		
		ZEnable = TRUE; // We want z-buffering enabled
		
		Sampler[0] = (TextureSampler); // Set sampler 0 to the defined texture sampler
		
		VertexShader = compile vs_2_0 PerPixelLitWithShadowMapVS(); // Use vertex shader 2.0
		PixelShader = compile ps_2_0 PerPixelLitWithShadowMapPS(); // Use vertex shader 2.0
	}
}

// Renders the shadow map
technique ShadowMap
{
	// This is the first render pass in our technique.  It is responsible for 
	// drawing the geometry with per-pixel lighting.
	pass First
	{
		// *** Set render states ***
		ZEnable = TRUE; // We want z-buffering enabled
		
		VertexShader = compile vs_2_0 ShadowMapVS(); // Use vertex shader 2.0
		PixelShader = compile ps_2_0 ShadowMapPS(); // Use pixel shader 2.0
	}
	
}


	