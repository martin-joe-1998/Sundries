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

cbuffer CBufferTime : register(b2)
{
    float _Time;
    float3 pad;
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
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    output.Position = mul(mul(mul(float4(input.positionOS, 1.0f), World), View), Projection);
    output.UV = input.uv;
    output.Color = input.color;
    
    return output;
}

// --- Tone map helpers ---
float3 TonemapACESApprox(float3 x)  // Narkowicz 2015
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float rand(float2 uv)
{
    static const float a = 12.9898;
    static const float b = 78.233;
    static const float c = 43758.5453;
    
    float dt = dot(uv, float2(a, b));
    float sn = fmod(dt, 3.1415);
    
    return frac(sin(sn) * c);
}

float Luma(float3 c)
{
    return dot(c, float3(0.2126, 0.7152, 0.0722));
}

float timeMultiplier = 5.0;
static const float FREQ_MIN = 0.2; // Hz
static const float FREQ_MAX = 0.4; // Hz
static const float STAR_SHARP = 4.0;
static const float DENSITY_TH = 0.988;

void draw_stars(inout float4 color, float2 uv)
{
    float starMask = step(DENSITY_TH, rand(uv * 37.0 + 11.3));
    if (starMask < 0.5)
        return;
    
    float rFreq = rand(uv * 19.27 + 7.23);
    float rPhase = rand(uv * 29.91 + 1.57);
    float freq = lerp(FREQ_MIN, FREQ_MAX, rFreq);
    float phase = 6.2831853 * rPhase;
    
    float pulse = 0.5 + 0.5 * sin(_Time * 6.2831853 * freq + phase);
    float t = pow(saturate(pulse), STAR_SHARP);
    
    color.rgb += starMask * t;
}

#define nsin(x) (sin(x) * 0.5 + 0.5)

void draw_auroras(inout float4 color, float2 uv)
{
    const float4 aurora_color_a = float4(0.0, 1.0, 0.5, 1.0);
    const float4 aurora_color_b = float4(0.0, 0.4, 0.6, 1.0);
    
    float t = nsin(-_Time * timeMultiplier + uv.x * 100.0) * 0.075 +
              nsin(_Time * timeMultiplier + uv.x * abs(uv.x - 0.5) * 100.0) * 0.1 - 0.5;
    t = 1.0 - smoothstep(uv.y - 4.0, uv.y * 2.0, t);
    
    float4 final_color = lerp(aurora_color_a, aurora_color_b, clamp(1.0 - uv.y * t, 0.0, 1.0));

    color += final_color * t * (t + 0.5) * 0.5;
}

// --------------------- Pixel Shader ---------------------
float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float2 uv = input.UV;
    uv.y = 1.0 - uv.y;
    
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
    
    draw_stars(color, uv);
    draw_auroras(color, uv);
    
    // --------- ToneMapping ---------
    float3 rgb = color.rgb * 1.1;
    rgb = TonemapACESApprox(rgb);
    float Gamma = 2.2;
    rgb = pow(rgb, 1.0 / Gamma);
    
    return float4(rgb, 1.0);
}