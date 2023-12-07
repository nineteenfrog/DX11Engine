#include "Include.hlsli"

//Constant buffer
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float3 cameraPos;
    float3 ambientColor;
    Light directionalLight1;
    Light directionalLight2;
    Light directionalLight3;
    Light pointLight1;
    Light pointLight2;
}

Texture2D Albedo : register(t0); // "t" registers for textures
Texture2D NormalMap : register(t1);
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);
Texture2D ShadowMap : register(t4); // Adjust index as necessary
SamplerState BasicSampler : register(s0); // "s" registers for samplers
SamplerComparisonState ShadowSampler : register(s1);

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
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;
    float4 shadowMapPos : SHADOW_POSITION;
};

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

float3 calculateDirLight(
    Light light,
    VertexToPixel input,
    float3 baseColor,
    float3 specularColor,
    float roughness,
    float metalness)
{
    float3 lightDir = normalize(-light.direction);
    float3 V = normalize(cameraPos - input.worldPosition);

    float3 diff = DiffusePBR(input.normal, lightDir);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, lightDir, V, roughness, specularColor, F);

    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    float3 lightFinal = balancedDiff * baseColor + spec;
    
    return lightFinal * light.intensity * light.color;
}

float3 calculatePointLight(
    Light light,
    VertexToPixel input,
    float3 baseColor,
    float3 specularColor,
    float roughness,
    float metalness)
{
    float3 lightDir = normalize(-light.direction);
    float3 V = normalize(cameraPos - input.worldPosition);

    float3 diff = DiffusePBR(input.normal, lightDir);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, lightDir, V, roughness, specularColor, F);
    
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
    
    float3 lightFinal = balancedDiff * baseColor + spec; // Tint specular?
    
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
    // Perform the perspective divide (divide by W) ourselves
    input.shadowMapPos /= input.shadowMapPos.w;
    // Convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
    // Grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    // Get a ratio of comparison results using SampleCmpLevelZero()
    float shadowAmount = ShadowMap.SampleCmpLevelZero(
    ShadowSampler,
    shadowUV,
    distToLight).r;
    
    //NORMAL MAPPING
    input.normal = normalize(input.normal);
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal); // Don’t forget to normalize!
    
    float3 N = input.normal; // Must be normalized here or before
    float3 T = normalize(input.tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    input.normal = mul(unpackedNormal, TBN); // Note multiplication order!

    //BASE COLOR AND LIGHT
    float3 surfaceColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float3 totalLight = (0, 0, 0);
    
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    // Specular color determination -----------------
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);

    totalLight += calculateDirLight(directionalLight1, input, surfaceColor, specularColor, roughness, metalness);
    float3 shadowLight = calculateDirLight(directionalLight2, input, surfaceColor, specularColor, roughness, metalness);
    shadowLight *= shadowAmount;
    totalLight += shadowLight;
    totalLight += calculateDirLight(directionalLight3, input, surfaceColor, specularColor, roughness, metalness);
    totalLight += calculatePointLight(pointLight1, input, surfaceColor, specularColor, roughness, metalness);
    totalLight += calculatePointLight(pointLight2, input, surfaceColor, specularColor, roughness, metalness);

    totalLight = pow(totalLight, 1.0f / 2.2f);
    return float4(totalLight, 1);
}