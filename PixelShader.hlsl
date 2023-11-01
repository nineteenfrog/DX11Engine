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
};

float3 normalizeLightDirection(float3 lightDirection)
{
    return normalize(-lightDirection);
}

float3 diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float3 calculateDirLight(Light light, VertexToPixel input)
{
    float3 lightDir = normalizeLightDirection(light.direction);
    float3 surfaceColor = colorTint.rgb;
    
    float3 color = (diffuse(input.normal, lightDir) * light.color * surfaceColor)
                    + (ambientColor * surfaceColor);
    
    float3 V = normalize(cameraPos - input.worldPosition);
    float3 R = reflect(lightDir, input.normal);
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float spec = pow(saturate(dot(R, V)), specExponent);
        
    float3 lightFinal = surfaceColor * (color + spec); // Tint specular?
    
    return lightFinal;
}

float3 calculatePointLight(Light light, VertexToPixel input)
{
    float3 lightDir = normalizeLightDirection(light.position - input.worldPosition);
    float3 surfaceColor = colorTint.rgb;
    
    float3 color = (diffuse(input.normal, lightDir) * light.color * surfaceColor)
                    + (ambientColor * surfaceColor);
    
    float3 V = normalize(cameraPos - input.worldPosition);
    float3 R = reflect(lightDir, input.normal);
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;

    float spec = pow(saturate(dot(R, V)), specExponent);
        
    float3 lightFinal = surfaceColor * (color + spec); // Tint specular?
    
    return lightFinal;
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
    return att * att;
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
    
    
    float3 totalLight = calculateDirLight(directionalLight1, input) +
                        calculateDirLight(directionalLight2, input) +
                        calculateDirLight(directionalLight3, input) +
                        (calculatePointLight(pointLight1, input)  * Attenuate(pointLight1, input.worldPosition)) +
                        (calculatePointLight(pointLight2, input) * Attenuate(pointLight2, input.worldPosition));

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    
    return float4(surfaceColor, 1.0f);
}