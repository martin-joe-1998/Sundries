Shader "Ripple/RippleBWShader_uv"
{
    Properties
    {
        _RippleWidth ("Ripple Width", Float) = 0.02
        _WaveColor ("Wave Color", Color) = (1.0, 1.0, 1.0, 1.0)
        _SpawnWorldPos ("Wave Spawn Pos in World Space", Vector) = (0.0, 0.0, 0.0)
        _WaveTimeInterval ("Time Interval Between Wave", Float) = 4.0
        _RippleTimeInterval ("Time Interval Between Ripple", Float) = 0.4
        _NoiseStrength ("Sin Noise Strength", Range(0.0, 1.0)) = 0.0
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
                // coordinate of model's vertex in object space (x, y, z, 1.0) : (1,0, 0.0, 1.0, 1.0), (1,0, 0.0, -1.0, 1.0)...
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
                float3 normal : NORMAL;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                // coordinate of model's vertex in screen space
                float4 pos : SV_POSITION;
                // ... in world Position
                float2 relativePosXZ : TEXCOORD3;
                float3 normal : NORMAL;
            };

            float _RippleWidth;
            fixed4 _WaveColor;
            float3 _SpawnWorldPos;
            float _WaveTimeInterval;
            float _RippleTimeInterval;
            float _NoiseStrength;

            float deltaTime;
            float3 _ObjectScale;

            v2f vert (appdata v)
            {
                v2f o;
                // transfer normal vector from object space to world space
                o.normal = mul(unity_ObjectToWorld, v.normal).xyz;

                o.pos = UnityObjectToClipPos(v.vertex);

                // uv [0, 1] -> [-0.5, 0.5], keep as same as regular cube's range
                o.uv = v.uv - float2(0.5f, 0.5f);
                // fake uv coord as vextex.xz
                float4 fakeCenterPos = float4(o.uv.x, 0.0, o.uv.y, 1.0);
                float3 fakeCenterWorldPos = mul(unity_ObjectToWorld, fakeCenterPos).xyz;
                //float3 fakeCenterLocalPos = mul(unity_WorldToObject, float4(fakeCenterWorldPos, 1.0)).xyz;

                //float4 spawnLocalPos = mul(unity_WorldToObject, float4(_SpawnWorldPos, 1.0));
                //o.relativePosXZ = (fakeCenterLocalPos.xz - spawnLocalPos.xz) * _ObjectScale.xz;
                o.relativePosXZ = (fakeCenterWorldPos.xz - _SpawnWorldPos.xz) * _ObjectScale.xz;

                return o;
            }

            float spawnRing(float2 relativePosXZ, float timeInterval)
            {   
                // Radius
                float r2 = dot(relativePosXZ, relativePosXZ);
                float circleWidth = _RippleWidth;
                float t = max(0, deltaTime - timeInterval);

                // Make some noise on circle
                float noiseStrength = _NoiseStrength;
                float angle = atan2(relativePosXZ.y, relativePosXZ.x); // -PI ~ PI
                float fakeNoise = sin(angle * 10.0 + _Time.y * 5.0) * noiseStrength; // ¿Éµ÷·ù¶È

                float innerRadius = fmod(t, _WaveTimeInterval) + fakeNoise;
                float outerRadius = innerRadius + circleWidth;

                float inner2 = innerRadius * innerRadius;
                float outer2 = outerRadius * outerRadius;

                // Masking, in circle = 1, out circle = 0
                float isInRing = step(inner2, r2) * step(r2, outer2);

                return isInRing;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                deltaTime = _Time.y;
                // Time interval between two ripple in one wave
                float timeInterval = _RippleTimeInterval;

                float col = 0.0;

                // Now is 3 ripple in 1 wave
                col += spawnRing(i.relativePosXZ, 0.0);
                col += spawnRing(i.relativePosXZ, timeInterval);
                col += spawnRing(i.relativePosXZ, 2 * timeInterval);

                fixed4 finalCol = fixed4(col * _WaveColor.r, col * _WaveColor.g, col * _WaveColor.b, 1.0);

                return finalCol;
            }
            ENDCG
        }
    }
}