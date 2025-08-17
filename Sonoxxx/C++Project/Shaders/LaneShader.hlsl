cbuffer TransformCBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer PizzaParam : register(b4)
{
    float edgeWidth;
    float innerRadius;
    float outerRadius;
    float laneNumber;
    float4 edgeColor;
    float4 bgColor;
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
    float2 center = float2(0.5, 0.5);
    float2 newUV = uv - center;
    float distance = length(newUV); // 現在のピクセルと中心位置の距離
    
    // ピザのエッジ
    int isInLaneEdge = 0;
    float width = edgeWidth * 0.5; // 内側の判定だけ取るので幅を半減
    float edgeAngle[2] = { fmod(laneNumber * 45.0 + 67.5, 360.0), fmod((laneNumber + 1) * 45.0 + 67.5, 360.0) }; // ピザのエッジ2つの角度
    
    for (int i = 0; i < 2; i++)
    {
        float angle = radians(edgeAngle[i]);
        float2 direction = float2(cos(angle), sin(angle));
        float t = dot(newUV, direction);
        float d = abs(dot(newUV, float2(-direction.y, direction.x))); // 
        
        if (t >= innerRadius && t <= outerRadius && d < width)
        {
            isInLaneEdge = true;
        }         
    }

    // レーンのエッジ
    int isInPI = 0;
    float theta = degrees(atan2(newUV.y, newUV.x)); // 現在のピクセルとnewUV.u軸との角度
    theta = (theta < 0) ? (theta + 360) : theta;
    
    bool inSector = false; // 2つのエッジの角度内か
    if (edgeAngle[0]  < edgeAngle[1])
    {
        inSector = (theta >= edgeAngle[0] && theta <= edgeAngle[1]);
    }
    else
    {
        inSector = (theta >= edgeAngle[0] || theta <= edgeAngle[1]);
    }
    
    if (inSector && distance < outerRadius && isInLaneEdge == 0)
    {
        isInPI = 1;
    }
    
    // 円周エッジ
    int isInOuterEdge = 0;
    if ( isInPI == 1 && distance < outerRadius && distance >= outerRadius - edgeWidth)
    {
        isInOuterEdge = 1;
    }
    
    float4 resultColor = float4(0.0, 0.0, 0.0, 1.0);
    if(isInLaneEdge > 0 || isInOuterEdge > 0)
    {
        resultColor = edgeColor;
    }
    else if (isInPI > 0)
    {
        resultColor = bgColor;
    }
    else
    {
        clip(-1);
    }
    
    //resultColor.a = 0.5;
    //return resultColor;
    return float4(0.8, 0.8, 0.0, 0.5);
}