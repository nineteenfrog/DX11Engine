
//Constant buffer
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float3 cameraPos;
}

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
};


float4 main(VertexToPixel input) : SV_TARGET
{
    float x1, x2, y1, y2, z1, z2;
    x1 = input.normal.x;
    x2 = cameraPos.x;
    y1 = input.normal.y;
    y2 = cameraPos.y;
    z1 = input.normal.z;
    z2 = cameraPos.z;

    //https://www.omnicalculator.com/math/angle-between-two-vectors
    float angleBetween = acos(
        (mul(x1, x2) + mul(y1, y2) + mul(z1, z2)) /
        (mul(sqrt(mul(x2, x2) + mul(y2, y2) + mul(z2, z2)), sqrt(mul(x1, x1) + mul(y1, y1) + mul(z1, z1)))));
    
    if (angleBetween < 0.99)
    {
        return float4(1, 1, 1, 1);
    }
    else
    {
        return float4(0, 0, 0, 1);
    }
    
    //also cool 
    return float4(angleBetween, angleBetween, angleBetween, 1);

}