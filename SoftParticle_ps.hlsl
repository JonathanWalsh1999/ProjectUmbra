//--------------------------------------------------------------------------------------
// Soft Particle Pixel Shader
//--------------------------------------------------------------------------------------
// Saples a diffuse texture map and adjusts depth buffer value to give fake depth to the particle

#include "Common.hlsli" // Shaders can also use include files - note the extension

// Structure containing data for rendering a particle
struct ParticleData
{
    float3 position : position; // World position of particle, the geometry shader will expand it into a camera-facing quad
    float alpha : alpha; // Overall transparency of particle (the particle texture can also contain per-pixel transparency)
    float scale : scale; // Size of the quad created by the geometry shader from the particle point
    float rotation : rotation; // Rotation of the quad created by the geometry shader
};
//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D DiffuseMap : register(t0); // A diffuse map is the main texture for a model.
                                        // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic

// Access depth buffer as a texture map - see C++ side to see how this was set up
Texture2D DepthMap : register(t1);
SamplerState PointClampSampler : register(s1); // This sampler switches off filtering (e.g. bilinear, trilinear) when accessing depth buffer


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
PixelColourAndDepth main(ParticlePixelShaderInput input) : SV_Target
{
    PixelColourAndDepth output;
                                      
    // Extract diffuse material colour for this pixel from a texture. Using alpha channel here so use float4
    float4 diffuseMapColour = DiffuseMap.Sample(TexSampler, input.uv);

    float depthAdjust = diffuseMapColour.a * input.scale / 5;
    float pixelDepth = (input.projectedPosition.z * input.projectedPosition.w - depthAdjust) / (input.projectedPosition.w - depthAdjust);
    output.depth = pixelDepth;

    float2 viewPortUV = input.projectedPosition.xy;
    viewPortUV.x /= gViewportWidth;
    viewPortUV.y /= gViewportHeight;

    float viewPortDepth = DepthMap.Sample(PointClampSampler, viewPortUV);
    pixelDepth *= input.projectedPosition.w;
    viewPortDepth *= input.projectedPosition.w;

    float depthDiff = viewPortDepth - pixelDepth;

    if (depthDiff <= 0)
    {
        discard;
    }
    float depthFade = saturate(depthDiff / 0.025f);
	

	// Combine texture alpha with particle alpha
    output.colour = diffuseMapColour;
    output.colour.a *= input.alpha * depthFade; // If you increase the number of particles then you might want to reduce the 1.0f here to make them more transparent
    return output;
}