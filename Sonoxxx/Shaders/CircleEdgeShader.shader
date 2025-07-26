Shader "Unlit/CircleEdgeShader"
{
    Properties
    {
        _CircleEdgeWidth ("Circle Edge Width", Range(0.0, 0.1)) = 0.005
        _InnerEdgeRadius ("Inner Edge Radius", Range(0.0, 0.45)) = 0.45
        _OuterEdgeRadius ("Outer Edge Radius", Range(0.45, 0.5)) = 0.495
        _CircleAngleOffset ("Circle Angle Offset", Range(0.0, 359.0)) = 67.5
        _LaneEdgeMinRadius ("Lane Edge Min Radius", Range(0.0, 0.5)) = 0.0
        [HDR] _CircleEdgeColor ("Circle Edge Color", Color) = (1, 1, 1, 1)
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

            float _CircleEdgeWidth;
            float _InnerEdgeRadius;
            float _OuterEdgeRadius;
            float _CircleAngleOffset;
            float _LaneEdgeMinRadius;
            fixed4 _CircleEdgeColor;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                fixed4 col = fixed4(0.0, 0.0, 0.0, 1.0);

                // センター位置を決める
                float2 center = float2(0.5, 0.5);
                // UV を [0, 1] から [-0.5, 0.5] へ変換する。今回はテクスチャーを使わないので、後で戻す必要がない
                float2 newUV = i.uv - center;
                float dist = length(newUV);

                // ----------------------------- Inner Circle Edge ----------------------------- 
                float circleWidth = _CircleEdgeWidth;
                float innerRadius = _InnerEdgeRadius;
                float outerRadius = innerRadius + circleWidth;

                // 内側エッジの範囲を判定
                float isInInnerEdge = step(innerRadius, dist) * step(dist, outerRadius);

                // ----------------------------- Outer Circle Edge ----------------------------- 
                innerRadius = _OuterEdgeRadius;
                outerRadius = innerRadius + circleWidth;

                // 外側エッジの範囲を判定
                float isInOuterEdge = step(innerRadius, dist) * step(dist, outerRadius);

                // ----------------------------- Lane Edge ----------------------------- 
                float isInLaneEdge = 0.0;
                float minRadius = _LaneEdgeMinRadius;
                float maxRadius = _OuterEdgeRadius;

                for (int i = 0; i < 8; i++)
                {
                    // 各レーンの角度を計算し、ラジアンで取得
                    float angle = radians(fmod(i * 45.0 + _CircleAngleOffset, 360.0));
                    // 各レーンの方向上の単位ベクトルを計算
                    float2 dir = float2(cos(angle), sin(angle));
                    // 現在のピクセル（ベクトル）とレーンの単位ベクトルの内積により、ピクセルのベクトルをレーンの単位ベクトルに投影した長さを求める
                    // レーンエッジの長さを外側の円の半径以内に納めるため
                    float t = dot(newUV, dir);
                    // 現在のピクセル（座標）とレーンの単位ベクトルとの垂線距離を計算、float2(-dir.y, dir.x) は単位ベクトルの法線
                    // レーンエッジの幅を制御するため
                    float d = abs(dot(newUV, float2(-dir.y, dir.x)));
                    // 各レーンでは内側の判定しかしないので、幅を半減する必要がある
                    float width = circleWidth * 0.5;
                    if (t >= minRadius && t <= maxRadius && d < width)
                    {
                        isInLaneEdge = 1.0;
                    }
                }

                // ----------------------------- Lane Circle ----------------------------- 
                // レーンの内側の半径（円心からの距離）が 0 じゃないとき、その半径に合わせてもう一個円を描画する
                float isInLaneCircleEdge = 0.0;
                if (minRadius > 0.0)
                {
                    innerRadius = minRadius;
                    outerRadius = innerRadius + circleWidth;
                    isInLaneCircleEdge = step(innerRadius, dist) * step(dist, outerRadius);
                }

                // ----------------------------- Final Conditional Statement ----------------------------- 
                if (isInInnerEdge > 0.0 || isInOuterEdge > 0.0 || isInLaneEdge > 0.0 || isInLaneCircleEdge > 0.0)
                {
                    // 同じレーンの色を同じにする
                    // float3 rgb = hsv2rgb(float3(sector / 8.0, 1.0, 1.0));
                    // col = fixed4(rgb, 1.0);
                    col = _CircleEdgeColor;
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
