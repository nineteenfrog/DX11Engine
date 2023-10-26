#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2
#define MAX_SPECULAR_EXPONENT   256.0f
// ALL of your code pieces (structs, functions, etc.) go here!
struct Light
{
    int type; // Which kind of light? 0, 1 or 2 (see above)
    float3 direction; // Directional and Spot lights need a direction
    float range; // Point and Spot lights have a max range for attenuation
    float3 position; // Point and Spot lights have a position in space
    float intensity; // All lights need an intensity
    float3 color; // All lights need a color
    float spotFallOff; // Spot lights need a value to define their “cone” size
    float3 padding; // Purposefully padding to hit the 16-byte boundary
    
};


#endif