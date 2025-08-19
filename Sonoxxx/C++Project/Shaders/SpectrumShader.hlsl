cbuffer TransformCBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

#define BAR_COUNT 256
#define BAR_WIDTH 0.01
#define RADIUS 0.4
#define MAX_HEIGHT 0.1
#define ZOffset -0.5
#define PI 3.1415926535

struct VOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float hash(float n)
{
    return frac(sin(n) * 43758.5453123);
}

VOut VShader(uint id : SV_VertexID)
{
    VOut output;

     // バーのID
    int barIndex = id / 6;

    // 現在のIDのバー、の第n個の頂点（三角形描画用、バーは二つの三角形で出来ている）
    int vertIndex = id % 6;

    // バーの頂点を決める（Local space、中心位置を原点に）
    float2 localVerts[6] = {
        float2(-0.5, 0.0),
        float2( 0.5, 0.0),
        float2( 0.5, 1.0),
        float2(-0.5, 0.0),
        float2( 0.5, 1.0),
        float2(-0.5, 1.0),
    };

    // バーの垂直方向
    float angle = barIndex / (float)BAR_COUNT * PI * 2.0;
    float2 dir = float2(cos(angle), sin(angle));

    // 偽ランダム数でバーの高さを決める
    //float baseHeight = hash(barIndex * 1.37);
    //float height = (sin(/*_Time.y*/1.0 * 2.0 + barIndex * 0.5 + baseHeight * 6.28) * 0.5 + 0.5) * MAX_HEIGHT;
    float height = hash(barIndex * 1.37) * MAX_HEIGHT;

    // バーの垂直方向（円周と垂直）を正規化する
    float2 up = normalize(dir);

    // バーの水平方向（右方向、upと垂直）
    float2 right = float2(-dir.y, dir.x);

    // 手動で local space における各頂点を、world space に変換する
    float2 localPos = localVerts[vertIndex];
    localPos.x *= BAR_WIDTH;
    localPos.y *= height;
    float2 worldPos = up * (RADIUS + localPos.y) + right * localPos.x;

    // world space の頂点を projection space に変換する
    output.position = mul(mul(float4(worldPos, 0.0, 1.0), view), projection);

    // 虹色のグラデーションでバーを描画する
    float hue = barIndex / (float)BAR_COUNT;
    output.color = float4(sin(hue * 6.28), cos(hue * 6.28), sin(hue * 3.14), 1);


    return output;
}

float4 PShader(float4 position : SV_POSITION, float2 uv : TEXCOORD0, float4 color : COLOR0) : SV_TARGET
{
    float4 col = color;

    return col;
}