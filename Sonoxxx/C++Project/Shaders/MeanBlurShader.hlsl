Texture2D sceneTexture : register(t0);
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
    float4 color = sceneTexture.Sample(Sampler, uv);

    // 3テクセル分ずらすためのUV値を求める
    float offsetU = 2 / 1280.0;
    float offsetV = 2 / 720.0;

    // 基準テクセルから隣接する八つのテクセルのカラーをサンプリングする
    color += sceneTexture.Sample(Sampler, uv + float2(offsetU, 0.0f));
    color += sceneTexture.Sample(Sampler, uv + float2(-offsetU, 0.0f));
    color += sceneTexture.Sample(Sampler, uv + float2(0.0f, offsetV));
    color += sceneTexture.Sample(Sampler, uv + float2(0.0f, -offsetV));
    color += sceneTexture.Sample(Sampler, uv + float2(offsetU, offsetV));
    color += sceneTexture.Sample(Sampler, uv + float2(offsetU, -offsetV));
    color += sceneTexture.Sample(Sampler, uv + float2(-offsetU, offsetV));
    color += sceneTexture.Sample(Sampler, uv + float2(-offsetU, -offsetV));

    // 基準テクセルと近傍8テクセルの平均なので9で除算する
    color /= 9.0f;

    return color;
}