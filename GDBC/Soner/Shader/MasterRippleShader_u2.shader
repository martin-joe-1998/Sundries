Shader "Unlit/MasterRippleShader_u2"
{
    Properties
    {
        _FlatRippleWidth ("Ripple Width", Float) = 0.02
        _TimeScale ("Time Scale", Float) = 1.2
        _FlatNoiseStrength ("Sin Noise Strength", Range(0.0, 1.0)) = 0.0
        _FlatRippleLifetime ("Ripple Life time", Float) = 2.0
        _VertexRippleLifetime ("Ripple Lifetime", Float) = 5.0
        _VertexRippleWidth ("Ripple Width", Float) = 0.2
        _VertexRippleAmplitude ("Ripple Amplitude", Float) = 0.5
        _EdgeWidth ("Edge Width", Range(0.0, 1.0)) = 0.95
        _EdgeColor ("Edge Color", Color) = (1.0, 1.0, 1.0, 1.0)
    }
    SubShader
    {
        Tags { "RenderType"="Transparent" "Queue"="Transparent" }

        Pass
        {
            ZWrite Off
            Blend SrcAlpha OneMinusSrcAlpha
            Cull Off

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 pos : SV_POSITION;
                float2 uv : TEXCOORD0;
                float3 worldPos : TEXCOORD1;
                float3 relativePos : TEXCOORD2;
                float3 normalWorld : TEXCOORD3;
                float gradation : TEXCOORD4;
            };

            float _TimeScale;

            float _FlatRippleWidth;
            float _FlatNoiseStrength;
            float _FlatRippleLifetime;

            float _VertexRippleLifetime;
            float _VertexRippleWidth;
            float _VertexRippleAmplitude;

            #define MAX_RIPPLES 10
            // Those variable will be set by script
            float4 _FlatRippleSpawnWorldPos[MAX_RIPPLES]; // xyz = position, w = spawnTime
            fixed4 _FlatRippleColor[MAX_RIPPLES]; // RGBA
            float4 _VertexRippleSpawnWorldPos[MAX_RIPPLES];
            fixed4 _VertexRippleColor[MAX_RIPPLES];

            float _EdgeWidth;
            fixed4 _EdgeColor;

            // Calc flat ripple
            float spawnFlatRipple(float3 relativePos, float timeSinceSpawn, float deltaTime)
            {   
                float r2 = length(relativePos);
                float circleWidth = _FlatRippleWidth;

                float t = max(0, timeSinceSpawn);

                float angle = atan2(relativePos.z, relativePos.x);
                float fakeNoise = sin(angle * 10.0 + deltaTime) * _FlatNoiseStrength;

                float innerRadius = t + fakeNoise;
                float outerRadius = innerRadius + circleWidth;

                float inner2 = innerRadius * innerRadius;
                float outer2 = outerRadius * outerRadius;

                float isInRing = step(inner2, r2) * step(r2, outer2);

                float fade = saturate(1.0 - t / _FlatRippleLifetime);

                return isInRing * fade;
            }

            // Calc Vertex ripple
            float computeRippleDisplacement(float3 worldPos, float3 rippleCenter, float spawnTime, float deltaTime)
            {
                float timeSinceSpawn = deltaTime - spawnTime;
                if (spawnTime < 0.0 || timeSinceSpawn > _VertexRippleLifetime) return 0.0;

                float3 delta = worldPos - rippleCenter;
                float dist = length(delta.xz);

                float t = timeSinceSpawn;
                float waveFront = t;
                float fade = saturate(1.0 - t / _VertexRippleLifetime);

                float withinRing = step(abs(dist - waveFront), _VertexRippleWidth);
                //float ripple = sin(dist * 8 - t * 6) * _Amplitude;

                //return ripple * withinRing * fade;
                return withinRing * fade * _VertexRippleAmplitude;
            }

            // Calc edge
            float Edge(v2f i)
            {
                float edgeWidth = _EdgeWidth;
                float2 uv = (i.uv.xy - float2(0.5, 0.5)) * 2;
                float3 upNormal = float3(0.0, 1.0, 0.0);
                float3 downNormal = float3(0.0, -1.0, 0.0);

                // decide edge position
                if (dot(i.normalWorld, upNormal) >= 0.7071 || abs(dot(i.normalWorld, downNormal)) >= 0.7071)
                {
                    return 0.0; 
                }

                float edge = step(edgeWidth, abs(uv.y));
                // binarization
                edge = step(0.99, edge);

                return edge;
            }

            v2f vert (appdata v)
            {
                v2f o;

                o.worldPos = mul(unity_ObjectToWorld, v.vertex).xyz;

                // Process vertex Ripple
                float totalVertexRipple = 0.0;
                for (int i = 0; i < MAX_RIPPLES; ++i)
                {
                    float deltaTime = _Time.y * _TimeScale;
                    float3 center = _VertexRippleSpawnWorldPos[i].xyz;
                    float spawnTime = _VertexRippleSpawnWorldPos[i].w;
                    totalVertexRipple += computeRippleDisplacement(o.worldPos, center, spawnTime, deltaTime);
                }

                // apply displacement along normal (usually Y up)
                float3 displaced = v.vertex.xyz + float3(0, totalVertexRipple, 0);
                // record vertex offset
                o.gradation = totalVertexRipple;

                o.pos = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                o.normalWorld = normalize(mul(unity_ObjectToWorld, v.normal));
                
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // Initialize variable
                float deltaTime = _Time.y * _TimeScale;
                fixed4 col = fixed4(0.0, 0.0, 0.0, 0.0);
                float totalRippleStrength = 0.0;
                
                // Calc edge pattern
                float edge = Edge(i);
                col += fixed4(edge * _EdgeColor.rgb, 1.0);
                
                // Calc ripple pattern
                for (int j = 0; j < MAX_RIPPLES; ++j)
                {
                    float3 ripplePos = _FlatRippleSpawnWorldPos[j].xyz;
                    float spawnTime = _FlatRippleSpawnWorldPos[j].w;
                    float timeSinceSpawn = deltaTime - spawnTime;
                    
                    if (spawnTime < 0.0 || 
                        timeSinceSpawn > _FlatRippleLifetime || 
                        dot(i.normalWorld, float3(0.0, 1.0, 0.0)) <= 0.7071) 
                    { continue; }
                
                    float3 relative = i.worldPos - ripplePos;
                    float ripple = spawnFlatRipple(relative, timeSinceSpawn, deltaTime);
                    totalRippleStrength += ripple;

                    col += fixed4(ripple * _FlatRippleColor[j].xyz, 1.0);
                }

                // Brightness of vertex ripple
                float vertexRippleStrength = smoothstep(0.0, _VertexRippleAmplitude, i.gradation);
                totalRippleStrength += vertexRippleStrength;
                float alpha = saturate(totalRippleStrength);

                // Add vertex ripple color
                for (int j = 0; j < MAX_RIPPLES; ++j)
                {
                    if (_VertexRippleColor[j].w > 0.0)
                    {
                        col += fixed4(vertexRippleStrength * _VertexRippleColor[j].rgb, 1.0);
                    }
                }

                col.a = alpha;
                return col;
            }
            ENDCG
        }
    }
}
