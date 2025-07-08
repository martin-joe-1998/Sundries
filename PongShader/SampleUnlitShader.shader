Shader "Unlit/SampleUnlitShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _Radius ("Radius", Range(0.0, 1.0)) = 0.2
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            Blend SrcAlpha OneMinusSrcAlpha
            
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
            float _Radius;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);

                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // uv の範囲を [0, 1] -> [-1, 1] の正規化座標系に変換する（矩形メッシュの中心位置が（0.0, 0.0）になる）
                float2 uv = (i.uv - 0.5) * 2.0;
                // 半径と回転角度を決める（_Time.y は多分Unityでしか使えない）
                float radius = _Radius;
                float angle = _Time.y * 1.0;

                float cosA = cos(angle);
                float sinA = sin(angle);
                float2x2 rotMatrix = float2x2(cosA, -sinA, sinA, cosA);
                // 行列変換で回転後のuvを計算する
                float2 rotUV = mul(rotMatrix, uv);

                // x^2 + y^2 > r^2 円の方程式で円の範囲外の領域を clip （透明にする）
                if (pow(uv.x, 2) + pow(uv.y, 2) > pow(radius, 2))
                {
                    clip(-1);
                }

                // [-1, 1]範囲 の uv では正しくテクスチャーをサンプリングできないので、最初の [0, 1] に戻す
                uv = (rotUV * 0.5) + 0.5;
                // テクスチャーをサンプリングする
                fixed4 col = tex2D(_MainTex, uv);

                return col;
            }
            ENDCG
        }
    }
}
