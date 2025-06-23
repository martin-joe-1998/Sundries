struct Input
{
    float2 position : POSITION;
    float3 color : COLOR;
};

struct Output
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

Output main( Input input)
{
    Output output;
    
    output.position = float4(input.position, 0.0f, 1.0f); // Transform position to clip space
    output.color = input.color; // Pass through color
    
    return output;
}