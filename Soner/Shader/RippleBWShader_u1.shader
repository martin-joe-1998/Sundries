Shader "Unlit/RippleBWShader_u1"
{
    Properties
    {
        _RippleWidth ("Ripple Width", Float) = 0.02
        _TimeScale ("Time Scale", Float) = 0.5
        _WaveColor ("Wave Color", Color) = (1.0, 1.0, 1.0, 1.0)
        _NoiseStrength ("Sin Noise Strength", Range(0.0, 1.0)) = 0.0
        _RippleLifetime ("Ripple Life time", Float) = 2.0
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
                float3 worldPos : TEXCOORD1;
                float3 relativePos : TEXCOORD2;
                float3 normalWorld : TEXCOORD3;
            };

            float _RippleWidth;
            float _TimeScale;
            fixed4 _WaveColor;
            float _NoiseStrength;
            float _RippleLifetime;

            float deltaTime;

            #define MAX_RIPPLES 10
            float4 _SpawnWorldPos[MAX_RIPPLES]; // xyz = position, w = spawnTime

            v2f vert (appdata v)
            {
                v2f o;
                o.pos = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                o.normalWorld = normalize(mul(unity_ObjectToWorld, v.normal));
                
                o.worldPos = mul(unity_ObjectToWorld, v.vertex).xyz;

                return o;
            }

            float spawnRipple(float3 relativePos, float timeSinceSpawn)
            {   
                float r2 = length(relativePos);
                float circleWidth = _RippleWidth;

                float t = max(0, timeSinceSpawn);

                float angle = atan2(relativePos.z, relativePos.x);
                float fakeNoise = sin(angle * 10.0 + deltaTime) * _NoiseStrength;

                float innerRadius = t + fakeNoise;
                float outerRadius = innerRadius + circleWidth;

                float inner2 = innerRadius * innerRadius;
                float outer2 = outerRadius * outerRadius;

                float isInRing = step(inner2, r2) * step(r2, outer2);

                float fade = saturate(1.0 - t / _RippleLifetime);

                return isInRing * fade;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                deltaTime = _Time.y * _TimeScale;

                float col = 0.0;

                if (dot(i.normalWorld, float3(0.0, 1.0, 0.0)) <= 0.7071)
                {
                    return fixed4(col, col, col, 1.0);
                }

                for (int j = 0; j < MAX_RIPPLES; ++j)
                {
                    float3 ripplePos = _SpawnWorldPos[j].xyz;
                    float spawnTime = _SpawnWorldPos[j].w;
                    float timeSinceSpawn = deltaTime - spawnTime;
                    
                    if (spawnTime < 0.0 || timeSinceSpawn > _RippleLifetime) continue;

                    float3 relative = i.worldPos - ripplePos;
                
                    col += spawnRipple(relative, timeSinceSpawn);
                }

                return fixed4(col * _WaveColor.rgb, 1.0);
            }
            ENDCG
        }
    }
}