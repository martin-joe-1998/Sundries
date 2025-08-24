// A:
cbuffer Globals : register(b0)
{
    float2 _Resolution; // = (renderTargetWidth, renderTargetHeight) -> iResolution.xy
}

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0; // 0~1，对应 shadertoy 的 fragCoord / iResolution
};

VSOut VS(uint vid : SV_VertexID)
{
    // 生成覆盖全屏的三角形（顶点在 NDC）
    float2 p = float2(
        (vid == 2) ? 3.0 : -1.0,
        (vid == 1) ? 3.0 : -1.0
    );
    
    VSOut o;
    o.pos = float4(p, 0.0, 1.0);

    // 把 NDC(-1~1) 映射为 UV(0~1)
    o.uv = p * 0.5 + 0.5;
    // 如果你的纹理/渲染目标是顶左原点（DX 常见），而采样期待底左原点（类似 GL），可翻转一下：
    // o.uv.y = 1.0 - o.uv.y;

    return o;
}

Texture2D iChannel0 : register(t0);
Texture2D iChannel1 : register(t1);
SamplerState iSampler : register(s0);

cbuffer TimeCB : register(b1)
{
    float _Time; // 对应 iTime
    float _Padding[3];
}

float4 PS(VSOut i) : SV_Target
{
    float2 fragCoord = i.uv * _Resolution;
    float2 uv = i.uv;

    // 以下就是你贴的 Shadertoy 逻辑（GLSL->HLSL 的细微语法差异已处理）
    const float TAU = 6.2831853071;

    float o = iChannel1.Sample(iSampler, uv * 0.25 + float2(0.0, _Time * 0.025)).r;
    float d = iChannel0.Sample(iSampler, uv * 0.25 - float2(0.0, _Time * 0.02 + o * 0.02)).r * 2.0 - 1.0;

    float v = uv.y + d * 0.1;
    v = 1.0 - abs(v * 2.0 - 1.0);
    v = pow(v, 2.0 + sin((_Time * 0.2 + d * 0.25) * TAU) * 0.5);

    float3 color = 0;

    float x = (1.0 - uv.x * 0.75);
    float y = 1.0 - abs(uv.y * 2.0 - 1.0);
    color += float3(x * 0.5, y, x) * v;

    // 伪随机星星
    float2 seed = fragCoord;
    float2 r;
    r.x = frac(sin((seed.x * 12.9898) + (seed.y * 78.2330)) * 43758.5453);
    r.y = frac(sin((seed.x * 53.7842) + (seed.y * 47.5134)) * 43758.5453);

    float s = lerp(r.x, (sin((_Time * 2.5 + 60.0) * r.y) * 0.5 + 0.5) * ((r.y * r.y) * (r.y * r.y)), 0.04);
    color += pow(s, 70.0) * (1.0 - v);

    return float4(color, 1.0);
}

// B: Quad
struct VSIn
{
    float2 pos : POSITION; // NDC(-1~1) 或者先给 0~1 再映射
    float2 uv : TEXCOORD0;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOut VS(VSIn v)
{
    VSOut o;
    // 若 pos 已是 NDC(-1~1)，直接：
    o.pos = float4(v.pos, 0.0, 1.0);
    // 若 pos 是 0~1，需要 o.pos = float4(v.pos*2-1, 0, 1);

    o.uv = v.uv;
    // 需要时翻转 o.uv.y
    return o;
}