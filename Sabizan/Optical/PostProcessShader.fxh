Texture2D inputTex : register(t0);
SamplerState samLinear : register(s0);

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float Max3(float3 color)
{
    return max(max(color.r, color.b), color.g);
}

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    return output;
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = inputTex.Sample(samLinear, input.uv);
    float value = Max3(texColor.rgb);
    float4 finalColor = float4(value, value, value, texColor.a);
    return texColor;
}