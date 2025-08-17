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

cbuffer NoteParam  : register(b2)
{
    float theta;
    float3 padding;
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
    float4 finalCol = float4(0.0, 1.0, 0.0, 1.0);
    finalCol = colorChange;
    
    float2 center = float2(0.5, 0.5);
    float2 newUV = uv - center;
    float theta_ = degrees(atan2(newUV.y, newUV.x));
    theta_ = (theta_ < 0) ? (theta_ + 360) : theta_; // [0, 360)
    
    float circleWidth = 0.025;
    float innerRadius = 0.3;
    float outerRadius = innerRadius + circleWidth;
    
    float dist = length(newUV);
    float isInRing = step(innerRadius, dist) * step(dist, outerRadius);
    float isInAngle = step(0.0, theta_) * step(theta_, theta);
    float isInSector = isInRing * isInAngle;

    if (isInSector > 0.0)
    {
        finalCol = colorChange;
    }
    else
    {
        clip(-1);
    }
    //finalCol.xyz *= 2.0;
    return finalCol; 
}