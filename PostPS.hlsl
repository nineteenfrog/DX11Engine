#include "Include.hlsli"

// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
    
};
Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


float4 main(VertexToPixel input) : SV_TARGET
{
    float4 pixelColor = Pixels.Sample(ClampSampler, input.uv);
// NOTE: Here is where you should actually "process" the image
    return pixelColor;
}