//--------------------------------------------------------------------------------------
// Per-Pixel Lighting Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader receives position and normal from the vertex shader and uses them to calculate
// lighting per pixel. Also samples a samples a diffuse + specular texture map and combines with light colour.

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D    DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular
                                                // map (shininess level) in their alpha channel. Repurposing the alpha channel means we can't use alpha blending
                                                // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
    // Lighting equations
    input.worldNormal = normalize(input.worldNormal); // Normal might have been scaled by model scaling or interpolation so renormalise
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

    // Light 1
    float3 light1Direction = normalize(gLight1Position.xyz - input.worldPosition);
    float3 vectorDistance = input.worldPosition - gLight1Position.xyz;
    float3 light1Dist = length(vectorDistance);

    float3 diffuseLight1 = gLight1Colour.xyz * max(dot(input.worldNormal, light1Direction), 0); // Equations from lighting lecture

    diffuseLight1 /= light1Dist;

    float3 halfway = normalize(light1Direction + cameraDirection);
    float3 specularLight1 =  diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference


    // Light 2
    float3 light2Vector = gLight2Position.xyz - input.worldPosition;
    float3 light2Dist = length(light2Vector);
    float3 light2Direction = light2Vector / light2Dist;
    float3 diffuseLight2 = gLight2Colour * max(dot(input.worldNormal, light2Direction), 0) / light2Dist;

    halfway = normalize(light2Direction + cameraDirection);
    float3 specularLight2 =  diffuseLight2 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

	float3 totalDiffuseLight = 0;
	float3 totalSpecularLight = 0;


    for (int i = 0; i < lightAmount; ++i)
	{
        float3 lightDirection = normalize(lightPositions[i].xyz - input.worldPosition);

        float3 vectorDistance = input.worldPosition - lightPositions[i].xyz;
        float3 lightDist = length(vectorDistance);

        float3 diffuseLight = lightColours[i].xyz * max(dot(input.worldNormal, lightDirection), 0);

        diffuseLight /= lightDist;

            halfway = normalize(lightDirection + cameraDirection);
		float3 specularLight = diffuseLight * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);

		totalDiffuseLight += diffuseLight;
		totalSpecularLight += specularLight;
	}

    // Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
    float specularMaterialColour = textureColour.a;   // Specular material colour in texture A (shininess of the surface)

    // Combine lighting with texture colours
    float3 finalColour = (gAmbientColour + diffuseLight1 + diffuseLight2 + totalDiffuseLight) * diffuseMaterialColour +
                         (specularLight1 + specularLight2 + totalSpecularLight) * specularMaterialColour;

    if (textureColour.a < 0.5f)
    {
        discard;
    }
     return float4(finalColour, 1.0f); // Always use 1.0f for output alpha - no alpha blending in this lab
}