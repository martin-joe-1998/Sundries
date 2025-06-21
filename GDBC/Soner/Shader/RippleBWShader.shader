Shader "Unlit/RippleBWShader"
{
    Properties
    {
        _RippleCenter ("Ripple Center (World Pos)", Vector) = (0, 0, 0)
        _RippleSpeed ("Ripple Speed", Float) = 1.0
        _RippleFrequency ("Ripple Frequency", Float) = 10.0
        _RippleWidth ("Ripple Width", Float) = 0.1
        _TimeScale ("Time Scale", Float) = 1.0
        _WaveColor ("Wave Color", Color) = (1.0, 1.0, 1.0, 1.0)
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata
            {
                // Homogeneous position in object space (x, y, z, 1.0)
                float4 vertex : POSITION;
            };

            struct v2f
            {
                // Homogeneous clipping coordinate
                float4 pos : SV_POSITION;
                // world Position
                float3 worldPos : TEXCOORD0;
            };

            float3 _RippleCenter;
            float _RippleSpeed;
            float _RippleFrequency;
            float _RippleWidth;
            float _TimeScale;
            fixed4 _WaveColor;

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.worldPos = mul(unity_ObjectToWorld, v.vertex).xyz;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // vector from ripple center to vertex on plane in world space
                float3 offset = i.worldPos - _RippleCenter;
                float dist = length(offset);

                // ripple = sin(Fr * D(x, z) - v * dt)
                float ripple = sin(dist * _RippleFrequency - _Time.y * _RippleSpeed);
                // make ripple wave binary
                // abs() <= 0.0 -> 1.0     abs() >= 1.0 -> 0.0
                float alpha = smoothstep(_RippleWidth, 0.0, abs(ripple));

                fixed4 col = fixed4(_WaveColor.xyz * alpha, 1.0);
                return col;
            }
            ENDCG
        }
    }
}
