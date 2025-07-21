Shader "Skybox/StarrySkybox"
{
    Properties
    {
        _Exponent1 ("Exponent upward", Range(0.0, 5.0)) = 1.0
        _Exponent2 ("Exponent downward", Range(0.0, 5.0)) = 1.0
        _Intensity ("Intensity", Range(0.0, 2.0)) = 1.0
        _Color1 ("Sky Color Up", Color) = (0.0, 0.0, 0.0, 1.0)
        _Color2 ("Sky Color Middle", Color) = (0.0, 0.0, 0.0, 1.0)
        _Color3 ("Sky Color Down", Color) = (0.0, 0.0, 0.0, 1.0)
        [HDR] _StarColor ("Star Color", Color) = (0.3, 0.3, 0.8, 1.0)
        _StarDendity ("Star Dendity", float) = 90.0
        _StarSparsity ("Star Sparsity", float) = 40.0
        _StarSpeed ("Star Speed", float) = 1.0
        _StarFlickerPhase ("Star Flicker Phase", float) = 20.0
        _Reflection ("Reflection Y Stretch", Float) = 1.0
    }
    SubShader
    {
        Tags { "RenderType"="Background"
			   "Queue"="Background"
			   "PreviewType"="SkyBox" }

        Pass
        {
            ZWrite Off
			Cull Off

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
                float4 vertex : SV_POSITION;
                float2 uv : TEXCOORD0;
                float3 viewDir : TEXCOORD1;
            };

            float _Exponent1;
            float _Exponent2;
            float _Intensity;
            fixed4 _Color1;
            fixed4 _Color2;
            fixed4 _Color3;
            fixed4 _StarColor;
            float _StarSpeed;
            float _Reflection;
            float _StarDendity;
            float _StarSparsity;
            float _StarFlickerPhase;

            // hash(p) ∈ [0.0, 1.0)
            float hash(float3 p)
            {
                return frac(sin(dot(p, float3(127.1, 311.7, 74.7))) * 43758.5453);
            }

            // valueNoise(p) ∈ [0.0, 1.0)
            float valueNoise(float3 p) {
                // viewDir を一つ一つの cell に分けて、i は cell の原点位置、f は cell 内での offset
                float3 origin = floor(p);
                float3 offset = frac(p);
                // Cubeモデルを想像して、原点 i を基準に、Cube の8つの頂点に対してサンプリングする
//                 Z+
//                  ↑
//                  e--------f
//                 /|       /|
//                g--------h |
//                | |      | |
//                | a------|-b
//                |/       |/
//                c--------d----→ X+
                float a = hash(origin);
                float b = hash(origin + float3(1, 0, 0));
                float c = hash(origin + float3(0, 1, 0));
                float d = hash(origin + float3(1, 1, 0));
                float e = hash(origin + float3(0, 0, 1));
                float f = hash(origin + float3(1, 0, 1));
                float g = hash(origin + float3(0, 1, 1));
                float h = hash(origin + float3(1, 1, 1));
                
                // prilin : offset * offset * (offset * (offset * 6 - 15) + 10)
                // hermite interpolation, [0.0, 1.0) では easeInOutSine に似ている
                float3 u = offset * offset * (3.0 - 2.0 * offset);
                // 三次元空間の立体線形補間をなって、Cube内でスムーズな3D offset を取得
                return lerp(
                           lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y),
                           lerp(lerp(e, f, u.x), lerp(g, h, u.x), u.y),
                       u.z);
            }

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                o.viewDir = normalize(mul(unity_ObjectToWorld, v.vertex).xyz);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // 視線方向をサンプリングする
                float viewDirY = i.viewDir.y;
                float up = saturate(viewDirY);
                float down = saturate(-viewDirY);

                // 上の天の色の強さ
                float p1 = 1.0 - pow (1.0 - up, _Exponent1);
                // 下の天の色の強さ
                float p3 = 1.0 - pow (1.0 - down, _Exponent2);
                // 全体の色の強さ（地平線当たりの色）
                float p2 = 1.0 - p1 - p3;
                // 指数関数で三つの色を融合して天の色を決める、全体の明度を _Intensity で制御
                float4 skyCol = (_Color1 * p1 + _Color2 * p2 + _Color3 * p3) * _Intensity;

                // 現在のピクセルにおける星の強さ(Strength)を計算する
                float3 starCoord = fixed3(i.viewDir.x, i.viewDir.y * _Reflection, i.viewDir.z) * _StarDendity;
                float baseNoise = valueNoise(starCoord);
                baseNoise = pow(baseNoise, _StarSparsity);

                // 星の点滅を三角関数で制御する
                // sin(時間 + 位相差)
                float flicker = sin(_Time.y *_StarSpeed + baseNoise * _StarFlickerPhase) * 0.5 + 0.5;
                float star = baseNoise * flicker;

                // 星の色
                fixed4 starCol = star * _StarColor;

                // 天の色と星の色を alpha blend で融合
                skyCol = skyCol * (1 - starCol.a) + starCol * starCol.a;

                return skyCol;
            }
            ENDCG
        }
    }
}
