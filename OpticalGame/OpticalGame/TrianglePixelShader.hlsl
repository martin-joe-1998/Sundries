struct Input
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

float4 main(Input input) : SV_TARGET
{
    return float4(input.color);
}