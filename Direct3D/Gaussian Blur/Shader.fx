////////////////
// Constants //
//////////////

static const int kShine = 32; // Specular exponent 
static const int kKernelSize = 9; // Total Size of kernel (3x3)
static const float kTexSize = 256.0f; // Square size of texture(s) used

//////////////
// Globals //
////////////

float4x4 gProjMat; // The projection matrix (Project geometery from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gInvViewMat; // The inverse view matrix
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)

float3 gLightPos; // The world position of the point light
float3 gEyePos; // The world position of the eye of the camera

float3 gDiffuse; // The diffuse color of the point light
float3 gSpecular; // The specular color of the point light
float3 gAmbient; // The ambient color of the point light

float gBlurWeights[kKernelSize]; // The Guassian Distribution weights calculated for the blur

texture gTexture; // The texture

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

// Vertex lighting vertex shader output
struct VLOut
{
	float4 pos : POSITION; // Position after vertex shader has executed
	float4 color : COLOR; // Color after vertex shader has executed
};

VLOut VertexLightingVS(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	float3 diffuse, specular; // Light colors
	VLOut vOut = (VLOut)0; 
			
	// Put the vertex into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Put the normal into world space
	normal = mul(normal, (float3x3)gWorldMat);
	
	// Calculate the lighting vectors
	float3 lightVec = normalize(gLightPos - vOut.pos);
	float3 eyeVec = normalize(gEyePos - vOut.pos);
	float3 halfwayVec = normalize(eyeVec + lightVec);

	// Compute the diffuse component
	diffuse = max(dot(normal, lightVec), 0) * gDiffuse;
		
	// Compute the specular component
	specular = pow(max(dot(normal, halfwayVec), 0), kShine) * gSpecular;
		
	// Put the vertex into projection space
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat));
	
	// Set the final lit color
	// **NOTE** saturate() clamps values between 0.0f and 1.0f
	vOut.color.rgb = saturate((diffuse * color.rgb) + (specular * color.rgb) + (gAmbient * color.rgb));
		return vOut; // Return the modified vertex
}

// ***************************** //
// *** Screen Aligned Quads *** //
// *************************** //

struct SAQIn		
{					
	float4 pos : POSITION;		
	float2 uv : TEXCOORD0;		
};					
					
SAQIn SAQVS(SAQIn vIn)			
{											
	return vIn; 
};			
			
float4 SAQPS(float2 uv : TEXCOORD0) : COLOR0										
{																			
	return tex2D(TextureSampler, uv);															
};

// ******************************************* //
// *** Gaussain Blur Screen Aligned Quads *** //
// ***************************************** //

static const float kPixelKernel[kKernelSize] =
{
    -4.0,
    -3.0,
    -2.0,
    -1.0,
    0.0,
    1.0,
    2.0,
    3.0,
    4.0,
};

// r == blur radius
// N == Number of dimensions
// S == Standard deviation
// G(r) = (1 / (2pi * S²)^(N/2)) * e^(-r² / 2S²)
static const float kBlurWeights[kKernelSize] = 
{
    0.00013382983,
    0.0044318349,
    0.053990804,
    0.24196999,
    0.39894107,
    0.24196999,
    0.053990804,
    0.0044318349,
    0.00013382983,
};

float4 GausaainBlurVertPS(float2 uv : TEXCOORD0) : COLOR0
{
	float4 color = 0;

    for(int i = 0; i < kKernelSize; i++)
    {    
        color += tex2D(TextureSampler, uv + float2(0,kPixelKernel[i] / kTexSize)) * gBlurWeights[i];
    }
    
    return color;
}

float4 GausaainBlurHorzPS(float2 uv : TEXCOORD0) : COLOR0
{
	float4 color = 0;

    for(int i = 0; i < kKernelSize; i++)
    {    
		color += tex2D(TextureSampler, uv + float2(kPixelKernel[i] / kTexSize, 0)) * gBlurWeights[i];
    }
    
    return color;
}																							

/////////////////
// Techniques //
///////////////

// Renders a quad aligned to screen in screen space
technique SAQ							
{					
	pass First				
	{	
		ZEnable = FALSE; // We do not want z-buffering enabled
		Sampler[0] = (TextureSampler); // Set sampler 0 to the defined texture sampler				
								
		VertexShader = compile vs_2_0 SAQVS();					
		PixelShader  = compile ps_2_0 SAQPS();					
	}					
}

// Renders a quad aligned to screen in screen space
technique GuassianBlurVert							
{					
	pass First				
	{	
		ZEnable = FALSE; // We do not want z-buffering enabled
		Sampler[0] = (TextureSampler); // Set sampler 0 to the defined texture sampler				
								
		VertexShader = compile vs_2_0 SAQVS();					
		PixelShader  = compile ps_2_0 GausaainBlurVertPS();					
	}					
}

// Renders a quad aligned to screen in screen space
technique GuassianBlurHorz						
{					
	pass First				
	{	
		ZEnable = FALSE; // We do not want z-buffering enabled
		Sampler[0] = (TextureSampler); // Set sampler 0 to the defined texture sampler				
								
		VertexShader = compile vs_2_0 SAQVS();					
		PixelShader  = compile ps_2_0 GausaainBlurHorzPS();					
	}					
}

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


	