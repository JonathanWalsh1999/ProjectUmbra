//--------------------------------------------------------------------------------------
// GPU particle rendering geometry shader
//--------------------------------------------------------------------------------------
// Vertex shader that passes on particle data to the geometry shader without change
// The geometry shaders do all the update rendering work for this particle system

#include "Common.hlsli"

// Structure containing data for rendering a particle
struct ParticleData
{
    float3 position : position; // World position of particle, the geometry shader will expand it into a camera-facing quad
    float alpha : alpha; // Overall transparency of particle (the particle texture can also contain per-pixel transparency)
    float scale : scale; // Size of the quad created by the geometry shader from the particle point
    float rotation : rotation; // Rotation of the quad created by the geometry shader
};
//-----------------------------------------------------------------------------
// Main function
//-----------------------------------------------------------------------------

// Main vertex shader function
void main(in ParticleData input, out ParticleData output)
{
    output = input;
}
