//--------------------------------------------------------------------------------------
// Common include file for all shaders
//--------------------------------------------------------------------------------------
// Using include files to define the type of data passed between the shaders


// The structure below describes the vertex data to be sent into vertex shaders that need tangents
//****| INFO | Models that contain tangents can only be sent into shaders that accept this structure ****//
struct TangentVertex
{
    float3 position : position;
    float3 normal : normal;
    float3 tangent : tangent;
    float2 uv : uv;
};

struct VertexInput
{
    float3 position : position;
    float3 normal : normal;
    float3 tangent : tangent;
    float2 uv : uv;
    
    uint material : BLENDINDICES;
};

struct NormalMappingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition; // Data required for lighting calculations in the pixel shader
    float3 modelNormal : modelNormal; // --"--
    float3 modelTangent : modelTangent; // --"--
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
    

};

// Structure containing data for rendering a particle
//struct ParticleData
//{
//    float3 position : position; // World position of particle, the geometry shader will expand it into a camera-facing quad
//    float alpha : alpha; // Overall transparency of particle (the particle texture can also contain per-pixel transparency)
//    float scale : scale; // Size of the quad created by the geometry shader from the particle point
//    float rotation : rotation; // Rotation of the quad created by the geometry shader
//};

// Data passed from particle geometry shader to the particle pixel shader
struct ParticlePixelShaderInput
{
    float4 projectedPosition : SV_Position; // 2D projection space position of particle
    float2 uv : uv; // Texture coordinates
    float alpha : alpha; // Overall transparency of particle
    float scale : scale; // Size of the particle (scaling doesn't happen in the pixel shader but the scale is used for depth value adjustment - see lab)
};

// Output from soft particle pixel shader includes depth (to be added as part of the lab exercise)
// Rarely use a structure for the pixel shader output as we normally output a float4 colour only but in this exercise we will output an adjusted depth value too.
struct PixelColourAndDepth
{
    float4 colour : SV_Target;
    float depth : SV_Depth;

};


struct PixelShaderInputMulti
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition; // Data required for lighting calculations in the pixel shader
    float3 modelNormal : modelNormal; // --"--
    float3 modelTangent : modelTangent; // --"--
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
    
    uint material : BLENDINDICES;
};


//--------------------------------------------------------------------------------------
// Shader input / output
//--------------------------------------------------------------------------------------

// The structure below describes the vertex data to be sent into the vertex shader.
struct BasicVertex
{
    float3 position : position;
    float3 normal   : normal;
    float2 uv       : uv;
};


// This structure describes what data the lighting pixel shader receives from the vertex shader.
// The projected position is a required output from all vertex shaders - where the vertex is on the screen
// The world position and normal at the vertex are sent to the pixel shader for the lighting equations.
// The texture coordinates (uv) are passed from vertex shader to pixel shader unchanged to allow textures to be sampled
struct LightingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition;   // The world position and normal of each vertex is passed to the pixel...
    float3 worldNormal   : worldNormal;     //...shader to calculate per-pixel lighting. These will be interpolated
                                            // automatically by the GPU (rasterizer stage) so each pixel will know
                                            // its position and normal in the world - required for lighting equations
    
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
};



// This structure is similar to the one above but for the light models, which aren't themselves lit
struct SimplePixelShaderInput
{
    float4 projectedPosition : SV_Position;
    float2 uv : uv;
};

struct PixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    float3 diffuseLight : diffuseLight;
    float3 specularLight : specularLight;
    float2 uv : uv;
};


//const static int lightAmount = 1;
//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

// These structures are "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the matrices used to position the camera are updated from C++ to GPU every frame along with lighting information
// These variables must match exactly the gPerFrameConstants structure in Scene.cpp



cbuffer PerFrameConstants : register(b0) // The b0 gives this constant buffer the number 0 - used in the C++ code
{
    const static int MAX_LIGHTS = 20;

    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix; // The above two matrices multiplied together to combine their effects

    float4   gLight1Position; // 3 floats: x, y z
                               // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
                              // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
                              // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
    float4   gLight1Colour;

    float4   gLight2Position;

    float bigPad;
    float gDayNightCycle;
    float gParallaxDepth;
    int lightCount;

    float3   gLight2Colour;
    float blendAmount;

    float3   gAmbientColour;
    float    gSpecularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

    float3   gCameraPosition;
    int shadowEffect;//Each shadow effect will have a number assigned to them, so that it will be easy to change on demand in C++. e.g. z-buffer = 0 pcf = 1 etc.

    float4 lightFacings[MAX_LIGHTS];


    float4x4 lightViewMatrix[MAX_LIGHTS];
    float4x4 lightProjectionMatrix[MAX_LIGHTS];
    
    float4 lightPositions[MAX_LIGHTS];
    float4 lightColours[MAX_LIGHTS];

    float4 cubeViewProj[6];

    float gViewportWidth;
    float gViewportHeight;
    float2 padding;
    
    float4x4 gCameraMatrix;

}
// Note constant buffers are not structs: we don't use the name of the constant buffer, these are really just a collection of global variables (hence the 'g')
static const int MAX_BONES = 64;

// If we have multiple models then we need to update the world matrix from C++ to GPU multiple times per frame because we
// only have one world matrix here. Because this data is updated more frequently it is kept in a different buffer for better performance.
// We also keep other data that changes per-model here
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The b1 gives this constant buffer the number 1 - used in the C++ code
{
    float4x4 gWorldMatrix;

    float4   gObjectColour;
    //float    padding6;  // See notes on padding in structure above
    float4x4 gBoneMatrices[MAX_BONES];
}


