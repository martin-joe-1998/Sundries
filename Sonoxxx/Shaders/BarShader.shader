Shader "Unlit/BarShader"
{
    Properties { 
        _ZOffset ("Z offset", float) = -0.5 
        }

    SubShader
    {
        Tags { "RenderType" = "Opaque" }
        Pass
        {
            ZWrite On
            ZTest Always
            Blend SrcAlpha OneMinusSrcAlpha
            Cull Off

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            #define BAR_COUNT 256
            #define BAR_WIDTH 0.04
            #define RADIUS 2.6
            #define MAX_HEIGHT 0.6

            struct v2f
            {
                float4 pos : SV_POSITION;
                float4 color : COLOR;
            };

            // 偽ランダム数を生成する関数
            float hash(float n)
            {
                return frac(sin(n) * 43758.5453123);
            }

            float _ZOffset;

            // 頂点シェーダー
            v2f vert(uint id : SV_VertexID)
            {
                v2f o;

                // バーのID
                int barIndex = id / 6;
                // 現在のIDのバー、の第n個の頂点（三角形描画用、バーは二つの三角形で出来ている）
                int vertIndex = id % 6;

                // バーの頂点を決める（Local space、中心位置を原点に）
                float2 localVerts[6] = {
                    float2(-0.5, 0.0),
                    float2( 0.5, 0.0),
                    float2( 0.5, 1.0),

                    float2(-0.5, 0.0),
                    float2( 0.5, 1.0),
                    float2(-0.5, 1.0),
                };

                // バーの垂直方向
                float angle = barIndex / (float)BAR_COUNT * UNITY_PI * 2.0;
                float2 dir = float2(cos(angle), sin(angle));

                // 偽ランダム数でバーの高さを決める
                //float height = hash(barIndex * 1.37) * MAX_HEIGHT;
                float baseHeight = hash(barIndex * 1.37);
                float height = (sin(_Time.y * 2.0 + barIndex * 0.5 + baseHeight * 6.28) * 0.5 + 0.5) * MAX_HEIGHT;

                // バーの垂直方向（円周と垂直）を正規化する
                float2 up = normalize(dir);
                // バーの水平方向（右方向、upと垂直）
                float2 right = float2(-dir.y, dir.x);

                // 手動で local space における各頂点を、world space に変換する
                float2 localPos = localVerts[vertIndex];
                localPos.x *= BAR_WIDTH;
                localPos.y *= height;

                float2 worldPos = up * (RADIUS + localPos.y) + right * localPos.x;

                // world space の頂点を projection space に変換する（これで3D描画になる）
                o.pos = mul(UNITY_MATRIX_VP, float4(worldPos, _ZOffset, 1.0));

                // 虹色のグラデーションでバーを描画する
                float hue = barIndex / (float)BAR_COUNT;
                o.color = float4(sin(hue * 6.28), cos(hue * 6.28), sin(hue * 3.14), 1);

                return o;
            }

            // ピクセルシェーダー
            fixed4 frag(v2f i) : SV_Target
            {
                return i.color;
            }
            ENDCG
        }
    }
}
