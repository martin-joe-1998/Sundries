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

cbuffer LineParams : register(b3)
{
    float2 circleCenter; //(UV???? ex: 0.5, 0.5)
    float2 direction; // ?~????????????_ (x, y) ?s?N?Z?????W
    float radius; // ?~????a 
    float lineThickness; 
    float2 padding;
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

float4 PShader(VOut input) : SV_TARGET
{
    float2 toCenter = input.uv - circleCenter;
    float distToCenter = length(toCenter);
    if (distToCenter > radius)
    {
        clip(-1); // ?~?O??j??
    }

    float2 lineVec = normalize(direction); //normalize(pos - circleCenter);
    float2 pixelVec = input.uv - circleCenter;
    
    float projection = dot(pixelVec, lineVec);

    // ??????????`?F?b?N?i???a?????????????O?j
    if (projection < 0 || projection > radius)
    {
        clip(-1);
    }
    
    // ????????????????
    float distance = abs(dot(pixelVec, float2(-lineVec.y, lineVec.x)));
    if (distance < lineThickness * 0.5)
    {
        return colorChange;
    }
    
    clip(-1); // ???O??j??
    return (1.0, 0.0, 1.0, 1.0);
}

