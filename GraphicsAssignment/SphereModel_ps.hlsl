#include "Common.hlsli"

Texture2D DiffuseMap : register(t0);

SamplerState TexSampler : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Extract diffuse material colour for this pixel from a texture (will cover textures in next lab)
    //https://www.gamedev.net/forums/topic/592497-texture-atlas-and-scrolling-textures/

    input.uv += (0.001f*1.0f, 0.001f*1.0f);
    
    float3 diffuseMaterial = DiffuseMap.Sample(TexSampler, input.uv).rgb;

    // Assume specular material colour is white (i.e. highlights are a full, untinted reflection of light)
    float3 specularMaterial = 1.0f;


    // Combine colours (lighting, textures) for final pixel colour
    float4 finalPixelColour;
    finalPixelColour.rgb = (diffuseMaterial * input.diffuseLight + specularMaterial * input.specularLight);
    finalPixelColour.r += 0.1f;
    finalPixelColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

    return finalPixelColour;
}