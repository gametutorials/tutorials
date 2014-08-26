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

// ****************************************** //
// *** Sobel Filter Screen Aligned Quads *** //
// **************************************** //

static const float kSobelFilterXAxis[kKernelSize] =
{
    -1.0f, 0.0f, 1.0f,
    -2.0f, 0.0f, 2.0f,
    -1.0f, 0.0f, 1.0f,
};

static const float kSobelFilterYAxis[kKernelSize] =
{
     1.0f,  2.0f,  1.0f,
     0.0f,  0.0f,  0.0f,
    -1.0f, -2.0f, -1.0f,
};

static const float3 kLuminanceConv = { 0.2125f, 0.7154f, 0.0721f };

float4 SobelFilterPS(float2 uv : TEXCOORD0) : COLOR0
{
	float4 diffuseColor = tex2D(TextureSampler, uv);
	float4 color = 0;

    for(int i = 0; i < kKernelSize; i++)
    {   
		float2 sobelFilterUV = { kSobelFilterXAxis[i] / kTexSize, kSobelFilterYAxis[i] / kTexSize };
        color += abs(diffuseColor - tex2D(TextureSampler, uv + sobelFilterUV));
    }
    
    return saturate(dot(kLuminanceConv, color));
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

technique SobelFilter
{
	pass First
	{
		ZEnable = FALSE; // We do not want z-buffering enabled
		Sampler[0] = (TextureSampler); // Set sampler 0 to the defined texture sampler				
								
		VertexShader = compile vs_2_0 SAQVS();					
		PixelShader  = compile ps_2_0 SobelFilterPS();	
	}
}
	