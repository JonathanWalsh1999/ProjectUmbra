#include "Common.hlsli"

PixelShaderInput main(BasicVertex modelVertex)
{
    PixelShaderInput output; // This is the data the pixel shader requires from this vertex shader

    // Input position is x,y,z only - need a 4th element to multiply by a 4x4 matrix. Use 1 for a point (0 for a vector) - recall lectures
    float4 modelPosition = float4(modelVertex.position, 1);

    // Multiply by the world matrix passed from C++ to transform the model vertex position into world space. 
    float4 worldPosition = mul(gWorldMatrix, modelPosition);

	//Input Normal
    float4 normalVector = float4(modelVertex.position, 0);

	//Multiply above float4 by world matrix to find normal in world space
    float4 worldNormal = mul(normalVector, gWorldMatrix);
    worldNormal = normalize(worldNormal); //Make normals equal to 1 by normalisation

  
    //worldPosition.x += sin(modelPosition.y + wiggle) * 0.1f;
   // worldPosition.y += sin(modelPosition.x + wiggle) * 0.1f;
   //worldPosition += worldNormal * (sin(wiggle) + 1.0f) * 0.1f;

    // In a similar way use the view matrix to transform the vertex from world space into view space (camera's point of view)
    // and then use the projection matrix to transform the vertex to 2D projection space
    float4 viewPosition = mul(gViewMatrix, worldPosition); // Read the comments, ah, but if you're reading this then you already knew that...
    output.projectedPosition = mul(gProjectionMatrix, viewPosition);

    // Temporary lines, replace each with the full lighting code as you do the lab exercise
    float4 worldLightVector = float4(gLight1Position.xyz - worldPosition.xwz, gLight1Position.w); //vector from vertex to light
    worldLightVector = normalize(worldLightVector);
    float4 worldLightVector2 = (lightPositions[0] - worldPosition); //vector from vertex to light
    worldLightVector2 = normalize(worldLightVector2);

    float4 dotProduct = dot(worldNormal, worldLightVector); //dotproduct between vertex normal and vertex to light
    float4 dotProduct2 = dot(worldNormal, worldLightVector2); //dotproduct between vertex normal and vertex to light

    float3 distance = worldPosition.xyz - gLight1Position.xyz; //Distance between vertex and light
    distance = normalize(distance);
    float3 distance2 = worldPosition.xyz - lightPositions[0].xyz; //Distance between vertex and light
    distance2 = normalize(distance2);

    //float3 attenuated = gLight1Colour  / 1; //Attenuated light


    output.diffuseLight = /*(gLight1Colour * max(dotProduct, 0)) *//*+ (lightColours[0] * max(dotProduct2, 0))*/+ /*gAmbientColour*/float3(0.25f, 0.25f, 0.25f); //Diffuse light equation


    float4 cameraVector = (worldNormal - viewPosition);
    float4 halfNormal = normalize(worldLightVector + cameraVector);
    float4 halfNormal2 = normalize(worldLightVector2 + cameraVector);

    float4 dotVertexHalf = dot(worldNormal, halfNormal);
    float4 dotVertexHalf2 = dot(worldNormal, halfNormal2);

    output.specularLight = 0.0f; // (gLight1Colour * pow(max(dotVertexHalf, 0), gSpecularPower)) + (lightColours[0] * pow(max(dotVertexHalf2, 0), gSpecularPower));

    // Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
    output.uv = modelVertex.uv;

    return output; // Ouput data sent down the pipeline (to the pixel shader)
}