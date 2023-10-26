#include "Include.hlsli"

//Constant buffer
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float3 cameraPos;
    float roughness;
    float3 ambientColor;
    Light directionalLight1;
}
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;
    float2 uv				: TEXCOORD;
    float3 normal			: NORMAL;
    float worldPosition		: POSITION;
};

float3 normalizeLightDirection(float3 lightDirection)
{
    return normalize(-lightDirection);
}

float3 diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    
    float3 lightDir = normalizeLightDirection(directionalLight1.direction);
    float3 surfaceColor = float3(colorTint.xyz);
    
    float3 color = (diffuse(input.normal, lightDir) * directionalLight1.color * surfaceColor)
                    + (ambientColor * surfaceColor);
    return float4(color, 1.0f);
}