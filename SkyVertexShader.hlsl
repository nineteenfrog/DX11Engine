#include "Include.hlsli"

//Constant buffer
cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}

struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : UV;
};

struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 position : SV_POSITION; // XYZW position (System Value Position)
    float3 sampleDir : DIRECTION;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel output;
	
    matrix viewUnchanged = view;
    view._14 = 0;
    view._24 = 0;
    view._34 = 0;
    
    matrix outputPos = mul(projection, viewUnchanged);
    output.position = mul(outputPos, float4(input.localPosition, 1.0f));
    
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;
	
    return output;
}