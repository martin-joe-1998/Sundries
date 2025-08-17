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

cbuffer NodeParam : register(b5)
{
    float width;
    float radius;
    float angleOffset;
    int Separation;
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

int EmitNode(float nodePos, float theta)
{
    float separationAngle = 360.0 / Separation;
    float minAngle = fmod(90.0 - (separationAngle * 0.5) + angleOffset, 360.0);
    float maxAngle = fmod(minAngle + separationAngle, 360.0);
    float nodeWidth = width * 5/6; // * (CircleScale / NodeScale);
    
    bool isAngleInRange = false;
    if (minAngle < maxAngle)
    {
        isAngleInRange = (theta >= minAngle && theta <= maxAngle);
    }
    else
    {
        isAngleInRange = (theta >= minAngle || theta <= maxAngle);
    }
    
    bool isPosInRange = false;
    if(nodePos <= radius + nodeWidth && nodePos >= radius)
    {
        isPosInRange = true;
    }
    
    return (isAngleInRange && isPosInRange == true) ? 1 : 0;
}

float4 PShader(float4 position : SV_POSITION, float2 uv : TEXCOORD0, float4 color : COLOR0) : SV_TARGET
{
    float4 resultColor = float4(0.0, 1.0, 0.0, 1.0);

    float2 center = float2(0.5, 0.5);
    float2 newUV = uv - center;
    float pixelRadius = length(newUV);
    float theta = degrees(atan2(newUV.y, newUV.x));
    theta = (theta < 0) ? (theta + 360) : theta;
    
    int isInRange = EmitNode(pixelRadius, theta);

    if (isInRange > 0)
    {
        resultColor = colorChange;
        resultColor.a = smoothstep(0.45, 0.321, radius);
    }
    else
    {
        clip(-1);
    }

    return resultColor;
}