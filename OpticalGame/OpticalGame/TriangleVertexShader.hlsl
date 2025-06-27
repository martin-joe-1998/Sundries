struct Input
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
};

struct Output
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
};

Output main(Input input)
{
    Output output;
    
    output.position = float4(input.position, 1.0f); // Transform position to clip space
    output.uv = input.uv;
    output.color = input.color; // Pass through color
    
    return output;
}