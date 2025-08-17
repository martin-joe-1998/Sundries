Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VOut VSMain(float3 position : POSITION, float2 uv : TEXCOORD0)
{
    VOut output;
    output.position = float4(position, 1.0);
    output.uv = uv;
    return output;
}

float4 PSMain(float4 position : SV_POSITION, float2 uv : TEXCOORD0) : SV_TARGET
{
    float4 col = Texture.Sample(Sampler, uv);

    return col;
}