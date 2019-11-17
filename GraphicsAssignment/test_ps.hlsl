#include "Common.hlsli"

Texture2D DiffuseMap : register(t0);
Texture2D DiffuseMap2 : register(t1);

SamplerState TexSampler : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Extract diffuse material colour for this pixel from a texture (will cover textures in next lab)

    float3 diffuseMaterial;
    float timer = 1.0f / 10.0f;

    diffuseMaterial = lerp(DiffuseMap.Sample(TexSampler, input.uv).rgb, DiffuseMap2.Sample(TexSampler, input.uv).rgb, timer);


    // Assume specular material colour is white (i.e. highlights are a full, untinted reflection of light)
    float3 specularMaterial = 1.0f;


    // Combine colours (lighting, textures) for final pixel colour
    float4 finalPixelColour;
    finalPixelColour.rgb = (diffuseMaterial * input.diffuseLight + specularMaterial * input.specularLight);
    finalPixelColour.a = 1.0f; // No alpha processing in this shader, so just set it to 1

    return finalPixelColour;
}