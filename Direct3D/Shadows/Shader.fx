////////////////
// Constants //
//////////////

// Exponent to raise the specular contribution to when doing
// per-pixel lighting
const int kShine = 64;

//////////////
// Globals //
////////////

float4x4 gProjMat; // The projection matrix (Project geometery from 3D to 2D window) 
float4x4 gViewMat; // The view matrix (Our current camera view of the world)
float4x4 gWorldMat; // The world matrix (Current world orientation of our object)
float4x4 gShadowMatrix; // The matrix for projecting shadows to a planar surface

float3 gLightPos; // The world position of the point light
float3 gEyePos; // The world position of the eye of the camera

float3 gDiffuse; // The diffuse color of the point light
float3 gSpecular; // The specular color of the point light
float3 gAmbient; // The ambient color of the point light

// The output of the vertex shader for rendering shadows
struct VSOut
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

// The input to our pixel shader for per-pixel lighting
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

// The output from the pixel shader containing the lit pixel to be
// drawn in the framebuffer
struct PSOut
{
	float4 color : COLOR;
};

// This is the vertex shader responsible for drawing shadow
VSOut ShadowVS(float3 pos : POSITION)
{
	VSOut vOut = (VSOut)0; // The output of the vertex shader
	
	// As you can see it's not that much code to make planar shadows happen!
	
	// First multiply the position by the world matrix and shadow matrix
	// this puts the vertex's in world space
	vOut.pos = mul(float4(pos, 1), mul(gWorldMat, gShadowMatrix));
	
	// Now, like normal, multiply by the view matrix and the projection matrix  
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat));
	
	// Lastly, set the color to whatever the shadow color is.  With (0.1f, 0.1f, 0.1f) for the
	// RGB part of the color, our shadow will be pretty dark and with the 0.85f for the alpha
	// portion of the shadow, when it blends it will cover up most of what is rendered on top of. 
	vOut.color = float4(0.1f, 0.1f, 0.1f, 0.85f);
		return vOut;
}

// The vertex shader for per-pixel lighting
VSOutPPL PerPixelLightingVS(float3 pos : POSITION, float3 normal : NORMAL, float4 color : COLOR)
{
	VSOutPPL vOut = (VSOutPPL)0; // The output of the vertex shader
	
	// Move the vertex into world space 
	vOut.pos = mul(float4(pos, 1), gWorldMat);
	
	// Move the normal into world space
	vOut.normal = mul(normal, (float3x3)gWorldMat);
	
	// Calculate the following vectors needed for lighting
	float3 eyeVec = normalize(gEyePos - vOut.pos.xyz); // Compute eyeVec
	vOut.lightVec = normalize(gLightPos - vOut.pos.xyz); // Compute and set the lightVec
	vOut.halfwayVec = normalize(eyeVec + vOut.lightVec); // Compute and set the halfwayVec
	
	vOut.pos = mul(vOut.pos, mul(gViewMat, gProjMat)); // Finishing placing position in correct space
	vOut.color = color; // Set the color
		return vOut; // Return the output of the vertex shader
}

// The pixel shader for per-pixel lighting.  It outputs the final color to
// be placed in the framebuffer
PSOut PerPixelLightingPS(PSIn pIn)
{
	float3 diffuse, specular; // Light colors		
	PSOut pOut = (PSOut)0; // The final color output from the pixel shader
	
	// Renormalize the vertex normal since it is interpollated and could become
	// unormalized
	pIn.normal = normalize(pIn.normal);
		
	// Calculate the diffuse and specular contributions of the light using 
	// the standard lighting equation
	diffuse = max(dot(pIn.normal, pIn.lightVec), 0) * gDiffuse;
	specular = pow(max(dot(pIn.normal, pIn.halfwayVec), 0), kShine) * gSpecular;
	
	// Add the diffuse, specular and ambient contributions to calculat the final color
	pOut.color.rgb = (diffuse * pIn.color.rgb) + (specular * pIn.color.rgb) + (gAmbient * pIn.color.rgb);
		return pOut; // Return the output of the pixel shader
}

/////////////////
// Techniques //
///////////////

// Our technique
technique PerPixelLightingWithShadows
{
	// This is the first render pass in our technique.  It is responsible for 
	// drawing the geometry with per-pixel lighting.
	pass First
	{
		// *** Set render states ***
		
		ZEnable = TRUE; // We want z-buffering enabled
		
		/*
			We want to record in the stencil where we have drawn objects using
			the "PerPixelLightingWithShadows" technique.  So first we enable stencil
			buffering, then on this first render pass we ALWAYS want to pass to the 
			stencil test, so we set the stencil test function to always pass.  Finally
			when we do pass the stencil test, we want to increment the stencil value(s)
			in the stencil buffer.  If we reach the max value in the stencil buffer (which
			for us would be 255) we want to clamp to the max value.  So we set the stencil
			pass to INCRSAT which says "Increment the value at cap at the max value".
		*/
		StencilEnable = TRUE; // Turn on the stencil buffer
		StencilFunc = ALWAYS; // Always pass the stencil test
		StencilPass = INCRSAT; // When the stencil test is passed, increment the current
							  // stencil value, capping at the maximum stencil value
		VertexShader = compile vs_2_0 PerPixelLightingVS(); // Use vertex shader 2.0
		PixelShader = compile ps_2_0 PerPixelLightingPS(); // Use pixel shader 2.0
	}
	
	// This is the second render pass in the technique.  It is responsible for
	// drawing the shadow of the object.
	pass Second
	{
		// *** Set render states ***
		
		ZEnable = FALSE; // Turn off z-buffer so no depth test is performed
		ZWriteEnable = FALSE; // Turn off z-write so the depth buffer is not upated
		
		AlphaBlendEnable = TRUE; // Turn on alpha blending
		SrcBlend = SRCALPHA; // Set SrcBlend flag for standard alpha blending
		DestBlend = INVSRCALPHA; // Set DestBlend flag for standard alpha blending
		
		/*
			Now we change the stencil flags that need to be changed for rendering shadows.  
			Notice how we do not have to enable the stencil buffer again.  This is because
			we turned that flag on in the first pass and therefore do not have to turn
			it on again in the second pass.  Now we know that at the beginning of every frame
			we clear the stencil buffer to zero (we do this with the CD3DObj::clear() method)
			and we know that for every location in the stencil buffer that an object is rendered,
			we increment the stencil buffer value (that's what we do in the first pass).  Therefore,
			only locations that equal zero in the stencil buffer are valid for drawing shadows.
			
			So we change the stencil funnction to EQUAL, meaning that it will only pass if the
			stencil reference value (StencilRef) is equal to what is stored in the buffer.  We
			also change StencilPass to KEEP because we do not want to update the stencil buffer
			at all while rendering the shadows
		*/
		StencilRef = 0; // The value to use by the stencil comparision function (StencilFunc) 
						// **NOTE** By default this value is already zero
		StencilFunc = EQUAL; // Check to see if the StencilRef is equal to what is stored in the
						    // stencil buffer.  If it is equal, go ahead and render the pixel
						   // otherwise do not render the pixel.
		StencilPass = KEEP; // If the test passes, do not modify the value in the stencil buffer
		
		VertexShader = compile vs_2_0 ShadowVS(); // Use vertex shader 2.0
		PixelShader = NULL; // We don't have a pixel shader for the second pass so set it to 
						   // NULL so the standard pixel shading routine gets executed
	}
}


	