Texture2D Texture : register(t0);

SamplerState Sampler : register(s0);

cbuffer TransformCBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer ColorCBuffer : register(b1)
{
    float4 colorChange;
}

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

VOut VShader(float3 position : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR)
{
    VOut output;

    output.position = mul(mul(mul(float4(position, 1.0), world), view), projection);
    output.uv = uv;
    output.color = color;

    return output;
}

float4 PShader(float4 position : SV_POSITION, float2 uv : TEXCOORD0, float4 color : COLOR0) : SV_TARGET
{
    float4 resultColor = Texture.Sample(Sampler, uv) * colorChange;
    if(resultColor.a <= 0)
    {
        clip(-1);
    }
    
    return resultColor;
}