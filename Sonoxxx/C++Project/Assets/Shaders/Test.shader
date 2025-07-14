struct VOut 
{ 
    float4 position : SV_POSITION; 
    float4 color : COLOR0; 
};

VOut VShader(float3 position : POSITION, float4 color : COLOR) 
{ 
    VOut output; 

    output.position = float4(position, 1.0); 
    output.color = color; 

    return output; 
} 


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR0): SV_TARGET 
{ 
    return color; 
    //return float4(1.0, 0.0, 0.0, 1.0);
}