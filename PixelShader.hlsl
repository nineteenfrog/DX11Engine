#include "Include.hlsli"

//Constant buffer
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float3 cameraPos;
    float roughness;
    float3 ambientColor;
    Light directionalLight1;
    Light directionalLight2;
    Light directionalLight3;
    Light pointLight1;
    Light pointLight2;
}

Texture2D SurfaceTexture    : register(t0); // "t" registers for textures
Texture2D SpecTexture       : register(t1); // "t" registers for textures
Texture2D NormalTexture     : register(t2); // "t" registers for textures
SamplerState BasicSampler   : register(s0); // "s" registers for samplers

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
    float3 worldPosition	: POSITION;
    float3 tangent          : TANGENT;
};

float3 normalizeLightDirection(float3 lightDirection)
{
    return normalize(-lightDirection);
}

float3 diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
}

float3 calculateDirLight(Light light, VertexToPixel input, float3 baseColor, float specVal)
{
    float3 lightDir = normalizeLightDirection(light.direction);
    
    float3 V = normalize(input.worldPosition - cameraPos);
    float3 R = reflect(lightDir, input.normal);
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float spec = pow(saturate(dot(R, V)), specExponent);
    spec *= specVal;
    
    float3 diff = (diffuse(input.normal, lightDir));
    float3 lightFinal = diff * baseColor + spec; // Tint specular?
    
    return lightFinal * light.intensity * light.color;
}

float3 calculatePointLight(Light light, VertexToPixel input, float3 baseColor, float specVal)
{
    float3 lightDir = normalizeLightDirection(input.worldPosition - light.position);
    
    float3 V = normalize(input.worldPosition - cameraPos);
    float3 R = reflect(lightDir, input.normal);
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float spec = pow(saturate(dot(R, V)), specExponent);
    spec *= specVal;
    
    float3 diff = diffuse(input.normal, lightDir);
    float3 lightFinal = diff * baseColor + spec; // Tint specular?
    
    return lightFinal * light.intensity * light.color * Attenuate(light, input.worldPosition);
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
    //NORMAL MAPPING
    input.normal = normalize(input.normal);
    float3 unpackedNormal = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don�t forget to normalize!
    
    float3 N = input.normal; // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!

    //BASE COLOR AND LIGHT
    float3 surfaceColor = (SurfaceTexture.Sample(BasicSampler, input.uv).rgb);
    
    float3 totalLight = colorTint * ambientColor;

    float3 specularMap = (SpecTexture.Sample(BasicSampler, input.uv).rgb);
    
    totalLight += calculateDirLight(directionalLight1, input, surfaceColor, specularMap.r);
    totalLight += calculateDirLight(directionalLight2, input, surfaceColor, specularMap.r);
    totalLight += calculateDirLight(directionalLight3, input, surfaceColor, specularMap.r);
    totalLight += calculatePointLight(pointLight1, input, surfaceColor, specularMap.r);
    totalLight += calculatePointLight(pointLight2, input, surfaceColor, specularMap.r);
    
    return float4(totalLight, 1.0f);
}