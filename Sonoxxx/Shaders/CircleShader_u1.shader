Shader "Sonolux/CircleShader_u1"
{
    Properties
    {
        _CircleWidth ("Circle Width", Range(0.0, 0.1)) = 0.05
        _InnerRadius ("Inner Radius", Range(0.0, 0.45)) = 0.45
        _CircleAngleOffset ("Circle Angle Offset", Range(0.0, 359.0)) = 67.5
        //_CircleColor ("Circle Color", Color) = (1, 1, 1, 1)
        [HDR] _SectorColor0 ("Sector 0 Color", Color) = (1,0,0,1)
        [HDR] _SectorColor1 ("Sector 1 Color", Color) = (1,0.5,0,1)
        [HDR] _SectorColor2 ("Sector 2 Color", Color) = (1,1,0,1)
        [HDR] _SectorColor3 ("Sector 3 Color", Color) = (0,1,0,1)
        [HDR] _SectorColor4 ("Sector 4 Color", Color) = (0,1,1,1)
        [HDR] _SectorColor5 ("Sector 5 Color", Color) = (0,0,1,1)
        [HDR] _SectorColor6 ("Sector 6 Color", Color) = (0.5,0,1,1)
        [HDR] _SectorColor7 ("Sector 7 Color", Color) = (1,0,1,1)
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

            //sampler2D _MainTex;
            //float4 _MainTex_ST;

            float _CircleWidth;
            float _InnerRadius;
            float _CircleAngleOffset;
            //fixed4 _CircleColor;
            fixed4 _SectorColor0;
            fixed4 _SectorColor1;
            fixed4 _SectorColor2;
            fixed4 _SectorColor3;
            fixed4 _SectorColor4;
            fixed4 _SectorColor5;
            fixed4 _SectorColor6;
            fixed4 _SectorColor7;

            // HSV空間の色情報をRGBに変換
            float3 hsv2rgb(float3 c)
            {
                float4 K = float4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
                float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
                return c.z * lerp(K.xxx, saturate(p - K.xxx), c.y);
            }
            // RGB空間の色情報をHSVに変換
            float3 rgb2hsv(float3 c)
            {
                float4 K = float4(0.0, -1.0/3.0, 2.0/3.0, -1.0);
                float4 p = (c.g < c.b) ? float4(c.bg, K.wz) : float4(c.gb, K.xy);
                float4 q = (c.r < p.x) ? float4(p.xyw, c.r) : float4(c.r, p.yzx);
            
                float d = q.x - min(q.w, q.y);
                float e = 1.0e-10;
                float h = abs(q.z + (q.w - q.y) / (6.0 * d + e));
                float s = d / (q.x + e);
                float v = q.x;
                return float3(h, s, v);
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
                fixed4 sectorColors[8] = {
                    _SectorColor0, _SectorColor1, _SectorColor2, _SectorColor3,
                    _SectorColor4, _SectorColor5, _SectorColor6, _SectorColor7
                };

                // センター位置を決める
                float2 center = float2(0.5, 0.5);
                // UV を [0, 1] から [-0.5, 0.5] へ変換する。今回はテクスチャーを使わないので、後で戻す必要がない
                float2 newUV = i.uv - center;
                // 普通のxy平面を想像して、theta は原点からある点に繋いだ直線とx軸の正の方向との角度(ラジアンから変換した)
                float theta = degrees(atan2(newUV.y, newUV.x));
                // [-180, 180) -> [0, 360)
                theta = (theta < 0) ? (theta + 360) : theta; // [0, 360)
                // 時計回りに theta を 67.5°回転、真上方向を八等分の最初のレーンにするために...
                theta = fmod(theta - _CircleAngleOffset + 360.0, 360.0);
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
                    //float3 rgb = hsv2rgb(float3(sector / 8.0, 1.0, 1.0));
                    //col = fixed4(rgb, 1.0);
                    col = sectorColors[sector];
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
