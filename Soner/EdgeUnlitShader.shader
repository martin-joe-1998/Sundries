Shader "Unlit/EdgeUnlitShader"
{
    Properties
    {
        _EdgeWidth ("Edge Width", Range(0.0, 1.0)) = 0.95
        _EdgeColor ("Edge Color", Color) = (1.0, 1.0, 1.0)
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
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            float _EdgeWidth;
            fixed4 _EdgeColor;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                float edgeWidth = _EdgeWidth;
                float2 uv = (i.uv.xy - float2(0.5, 0.5)) * 2;

                // decide edge position
                float edge = step(edgeWidth, abs(uv.x)) + step(edgeWidth, abs(uv.y));
                // binarization
                edge = step(0.99, edge);

                fixed4 col = fixed4(edge * _EdgeColor.rgb, 1.0);

                return col;
            }
            ENDCG
        }
    }
}
