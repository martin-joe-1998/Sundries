Shader "Unlit/RippleBWShader_3"
{
    Properties
    {
        _RippleSpeed ("Ripple Speed", Float) = 1.0
        _RippleFrequency ("Ripple Frequency", Float) = 10.0
        _RippleWidth ("Ripple Width", Float) = 0.1
        _TimeScale ("Time Scale", Float) = 1.0
        _WaveColor ("Wave Color", Color) = (1.0, 1.0, 1.0, 1.0)
        _SpawnWorldPos ("Wave Spawn Pos in World Space", Float) = (0.0, 0.0, 0.0)
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
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                // coordinate of model's vertex in screen space
                float4 pos : SV_POSITION;
                // ... in world Position
                float3 worldPos : TEXCOORD1;
                float3 localPos : TEXCOORD2;
                float2 relativePosXZ : TEXCOORD3;
            };

            float _RippleSpeed;
            float _RippleFrequency;
            float _RippleWidth;
            float _TimeScale;
            fixed4 _WaveColor;
            float3 _SpawnWorldPos;
            float _WaveTimeInterval;
            float _RippleTimeInterval;
            float _NoiseStrength;

            float deltaTime;
            float3 _ObjectScale;
            float2 objectScaleXZ;

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.worldPos = mul(unity_ObjectToWorld, v.vertex).xyz;
                o.localPos = mul(unity_WorldToObject, float4(o.worldPos, 1.0)).xyz;
                float4 SpawnLocalPos = mul(unity_WorldToObject, float4(_SpawnWorldPos, 1.0));
                o.relativePosXZ = (o.localPos.xz - SpawnLocalPos.xz) * _ObjectScale.xz;
                o.uv = v.uv;

                return o;
            }

            float spawnRing(float2 relativePosXZ, float timeInterval)
            {   
                // Radius
                float r2 = dot(relativePosXZ, relativePosXZ);
                float circleWidth = 0.01;
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

                return fixed4(col, col, col, 1.0);
            }
            ENDCG
        }
    }
}