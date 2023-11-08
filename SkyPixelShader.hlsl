#include "Include.hlsli"

TextureCube skyTexture: register(t0); // "t" registers for textures
SamplerState basicSampler : register(s0); // "s" registers for samplers

struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 position : SV_POSITION; // XYZW position (System Value Position)
    float3 sampleDir : DIRECTION;
};;

float4 main(VertexToPixel input) : SV_TARGET
{
    return skyTexture.Sample(basicSampler, input.sampleDir);
}