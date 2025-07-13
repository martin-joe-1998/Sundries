Shader "Sonolux/CircleShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _CircleWidth ("Circle Width", Range(0.0, 0.1)) = 0.05
        _InnerRadius ("Inner Radius", Range(0.0, 0.45)) = 0.45
        _CircleColor ("Circle Color", Color) = (1, 1, 1, 1)
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;

            float _CircleWidth;
            float _InnerRadius;
            fixed4 _CircleColor;
            
            float3 hsv2rgb(float3 c)
            {
                float4 K = float4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
                float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
            }

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                //o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                fixed4 col = fixed4(0.0, 0.0, 0.0, 1.0);

                float2 center = float2(0.5, 0.5);
                float2 newUV = i.uv - center;
                //float theta = degrees(atan2(newUV.y, newUV.x));
                //theta = (theta < 0) ? (theta + 360) : theta;
                float theta = atan2(newUV.y, newUV.x); // [-PI, PI]
                float hue = (theta / (2.0 * UNITY_PI)) + 0.5; // [0,1]

                float circleWidth = _CircleWidth;
                float innerRadius = _InnerRadius;
                float outerRadius = innerRadius + circleWidth;

                float dist = length(i.uv - center);

                float isInRing = step(innerRadius, dist) * step(dist, outerRadius);

                if (isInRing > 0.0)
                {
                    //col = fixed4(i.uv.x, i.uv.y, i.uv.x * i.uv.y, 1.0);
                    float3 rgb = hsv2rgb(float3(hue, 1.0, 1.0));
                    col = fixed4(rgb, 1.0);
                }
                else
                {
                    clip(-1);
                }

                return col;
            }
            ENDCG
        }
    }
}
