Shader "Sonolux/NodeShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _NodeWidth ("Node Width", Range(0.0, 0.15)) = 0.07
        _Radius ("Raduis", Range(0.0, 0.45)) = 0.35
        _AngleOffset ("Angle Offset", Range(0.0, 360.0)) = 0.0
        _NodeColor ("Node Color", Color) = (1, 1, 1, 1)
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

            float _NodeWidth;
            float _Radius;
            float _AngleOffset;
            fixed4 _NodeColor;

            int EmitNode(float nodePos, float theta)
            {
                float angleOffset = _AngleOffset;
                float minAngle = fmod(67.5 + angleOffset, 360.0);
                float maxAngle = fmod(minAngle + 45.0, 360.0);

                // theta が [minAngle, maxAngle] の範囲内かどうかを判定
                bool isAngleInRange = false;
                if (minAngle < maxAngle)
                {
                    isAngleInRange = (theta >= minAngle && theta <= maxAngle);
                }
                else
                {
                    isAngleInRange = (theta >= minAngle || theta <= maxAngle);
                }

                // Node の Position が [_Radius, _Radius + _NodeWidth] の範囲内かどうかを判定
                bool isPosInRange = false;
                if (nodePos <= _Radius + _NodeWidth && nodePos >= _Radius)
                {
                    isPosInRange = true;
                }

                // 二つの判定条件を満たすならtrueを返す
                return (isAngleInRange * isPosInRange == true) ? 1 : 0;
            }

            // 頂点シェーダー
            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                //o.uv = TRANSFORM_TEX(v.uv, _MainTex);
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
                // UV を [0, 1] から [-0.5, 0.5] へ変換する。
                float2 newUV = i.uv - center;
                // Sprite の範囲における各ピクセルと中心位置との距離
                float pixelRadius = length(newUV);
                // 普通のxy平面を想像して、theta は原点からある点に繋いだ直線とx軸の正の方向との角度(ラジアンから変換した)
                float theta = degrees(atan2(newUV.y, newUV.x));
                // [-180, 180) -> [0, 360)
                theta = (theta < 0) ? (theta + 360) : theta;

                // ピクセルシェーダーは、物体領域におけるすべてのピクセルで一回実行される
                // ここでは、現在のピクセルが Node のバームクーヘン範囲内に存在するかどうかを判定している
                int isInRange = EmitNode(pixelRadius, theta);

                // バームクーヘンの範囲内なら、Constant Bufferから送ったノードの色で現在のピクセルを描画する
                if (isInRange > 0)
                {
                    col = _NodeColor;
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
