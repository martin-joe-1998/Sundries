struct Input
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 color : COLOR;
};

float4 main(Input input) : SV_TARGET
{
    return float4(input.uv.x, input.uv.y, 0.0f, 1.0f);
}