Texture2D inputTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer BlurParam : register(b0)
{
    int radius;
    float sampleStep;
    float2 pad0;
    float4 weight_0;
    float4 weight_1;
}

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float2 GetTexelSize(Texture2D tex)
{
    uint w, h;
    tex.GetDimensions(w, h);
    return sampleStep / float2(w, h);
}

float ReadW(int idx /*[0, r]*/)
{
    if (idx < 4) {
        return (idx==0) ? weight_0.x : (idx==1) ? weight_0.y
             : (idx==2) ? weight_0.z : weight_0.w;
    } else {
        idx -= 4;
        return (idx==0) ? weight_1.x : (idx==1) ? weight_1.y
             : (idx==2) ? weight_1.z : weight_1.w;
    }
}

float GetWeight1D(int k /*[-r, r]*/)
{
    int idx = abs(k);
    return ReadW(idx);
}

VOut VSMain(float3 position : POSITION, float2 uv : TEXCOORD0)
{
    VOut output;
    output.position = float4(position, 1.0);
    output.uv = uv;
    return output;
}

float4 PSMain(float4 pos:SV_POSITION, float2 uv:TEXCOORD0) : SV_Target
{
    const int r  = radius;
    const float2 ts = GetTexelSize(inputTexture);

    float4 finalColor = 0;

    //[unroll] 
    for (int y = -r; y <= r; ++y)
    {
        //[unroll] 
        for (int x = -r; x <= r; ++x)
        {
            float  w  = GetWeight1D(x) * GetWeight1D(y);
            float2 du = float2(x, y) * ts;
            finalColor += w * inputTexture.SampleLevel(Sampler, uv + du, 0);
        }
    }

    return finalColor;
}