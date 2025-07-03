Texture2D texDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer CBufferPermanent : register(b0)
{
	matrix View;
	matrix Projection;
}

cbuffer CBufferEveryFrame : register(b1)
{
    matrix World;
}

struct VS_INPUT
{
    float3 positionOS : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
	float4 Color : COLOR0;
};

// --------------------- Vertex Shader ---------------------
VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
    
	output.Position = mul(mul(mul(float4(input.positionOS, 1.0f), World), View), Projection);
    output.UV = input.uv;
    output.Color = input.color;
    
	return output;
}

// --------------------- Pixel Shader ---------------------
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 color = texDiffuse.Sample(samLinear, input.UV);
    return color;
}