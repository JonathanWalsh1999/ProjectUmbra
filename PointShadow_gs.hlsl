#include "Common.hlsli"

//VS --> GS --> PS
struct GSOutput
{
	float4 pos : SV_POSITION;
    uint index : SV_RenderTargetArrayIndex;
    
};

[maxvertexcount(18)]//6 * 3 = 18
void main(
	triangle float4 input[3] : SV_POSITION, //Takes the output from vertex shader * 3
	inout TriangleStream< GSOutput > outputStrean//Sends to pixel shader
)
{
	for (uint iFace = 0; iFace < 6; iFace++)
	{
        GSOutput output;
        
        output.index = iFace;//Keep track of cube face
        
        //Triangle = 3 vertices
        for (int v = 0; v < 3; ++v)
        {
            output.pos = mul(input[v], cubeViewProj[iFace]);
            outputStrean.Append(output);
        }
        outputStrean.RestartStrip();          

    }
}