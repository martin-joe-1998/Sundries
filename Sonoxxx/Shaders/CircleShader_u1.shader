Shader "Sonolux/CircleShader_u1"
{
    Properties
    {
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

            //sampler2D _MainTex;
            //float4 _MainTex_ST;

            float _CircleWidth;
            float _InnerRadius;
            fixed4 _CircleColor;

            // HSV空間の色情報をRGBに変換
            float3 hsv2rgb(float3 c)
            {
                float4 K = float4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
                float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
            }

            // 頂点シェーダー
            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            // ピクセルシェーダー
            fixed4 frag (v2f i) : SV_Target
            {
                // 適当にベースカラーを決める
                fixed4 col = fixed4(0.0, 0.0, 0.0, 1.0);

                // センター位置を決める
                float2 center = float2(0.5, 0.5);
                // UV を [0, 1] から [-0.5, 0.5] へ変換する。今回はテクスチャーを使わないので、後で戻す必要がない
                float2 newUV = i.uv - center;
                // 普通のxy平面を想像して、theta は原点からある点に繋いだ直線とx軸の正の方向との角度(ラジアンから変換した)
                float theta = degrees(atan2(newUV.y, newUV.x));
                // [-180, 180) -> [0, 360)
                theta = (theta < 0) ? (theta + 360) : theta; // [0, 360)
                // 時計回りに theta を 67.5°回転、真上方向を八等分の最初のレーンにするために...
                theta = fmod(theta - 67.5 + 360.0, 360.0);
                // 360° を八等分する
                int sector = (int)floor(theta / 45.0); // 0~7

                float circleWidth = _CircleWidth;
                float innerRadius = _InnerRadius;
                float outerRadius = innerRadius + circleWidth;

                float dist = length(newUV);
                // リングの範囲を判定
                float isInRing = step(innerRadius, dist) * step(dist, outerRadius);

                if (isInRing > 0.0)
                {
                    // 同じレーンの色を同じにする
                    float3 rgb = hsv2rgb(float3(sector / 8.0, 1.0, 1.0));
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
