Shader "Unlit/LaneShader"
{
    Properties
    {
        _CircleEdgeWidth ("Circle Edge Width", Range(0.0, 0.1)) = 0.005
        _LaneEdgeMinRadius ("Lane Edge Min Radius", Range(0.0, 0.5)) = 0.0
        _InnerEdgeRadius ("Inner Edge Radius", Range(0.0, 0.45)) = 0.45
        _OuterEdgeRadius ("Outer Edge Radius", Range(0.45, 0.5)) = 0.495
        _LaneNumber ("Lane Number", Range(0.0, 7.0)) = 0
        [HDR] _LaneEdgeColor ("Lane Edge Color", Color) = (0, 0, 0, 1)
        [HDR] _LaneBackgroundColor ("Lane Background Color", Color) = (0.4, 0.4, 0.4, 0.4)
    }
    SubShader
    {
        Tags { "RenderType"="Transparent" "Queue"="Transparent" }

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
                float4 vertex : SV_POSITION;
                float2 uv : TEXCOORD0;
            };

            float _CircleEdgeWidth;
            float _InnerEdgeRadius;
            float _OuterEdgeRadius;
            float _LaneEdgeMinRadius;
            int _LaneNumber;
            fixed4 _LaneEdgeColor;
            fixed4 _LaneBackgroundColor;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = v.uv;

                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // 適当にベースカラーを決める
                fixed4 col = fixed4(0.0, 0.0, 0.0, 1.0);

                // センター位置を決める
                float2 center = float2(0.5, 0.5);
                // UV を [0, 1] から [-0.5, 0.5] へ変換する。今回はテクスチャーを使わないので、後で戻す必要がない
                float2 newUV = i.uv - center;
                // 現在のピクセルと中心位置との距離を計算
                float dist = length(newUV);

                // ----------------------------- レーンのエッジ（二つ）----------------------------- 
                int isInLaneEdge = 0;
                float circleWidth = _CircleEdgeWidth;
                // 各レーンでは内側の判定しかしないので、幅を半減する必要がある
                float width = circleWidth * 0.5;
                float minRadius = _LaneEdgeMinRadius;
                float maxRadius = _OuterEdgeRadius;
                // 二つのエッジの角度
                float edgeAngle[2] = { fmod(_LaneNumber * 45.0 + 67.5, 360.0), fmod((_LaneNumber + 1) * 45.0 + 67.5, 360.0) };

                for (int i = 0; i < 2; i++)
                {
                    // 各レーンの角度を計算し、ラジアンで取得
                    float angle = radians(edgeAngle[i]);
                    // 各レーンの方向上の単位ベクトルを計算
                    float2 dir = float2(cos(angle), sin(angle));
                    // 現在のピクセル（ベクトル）とレーンの単位ベクトルの内積により、ピクセルのベクトルをレーンの単位ベクトルに投影した長さを求める
                    // レーンエッジの長さを外側の円の半径以内に納めるため
                    float t = dot(newUV, dir);
                    // 現在のピクセル（座標）とレーンの単位ベクトルとの垂線距離を計算、float2(-dir.y, dir.x) は単位ベクトルの法線
                    // レーンエッジの幅を制御するため
                    float d = abs(dot(newUV, float2(-dir.y, dir.x)));
                    
                    // サイドエッジの範囲内なら
                    if (t >= minRadius && t <= maxRadius && d < width)
                    {
                        isInLaneEdge = 1;
                    }
                }

                // ----------------------------- レーンの背景 ----------------------------- 
                int isInPI = 0;
//                          ⇑ v+ [-0.5, 0.5]
//                          |     * 現在のピクセル
//                          |    / 
//                          |   /
//                          |  /
//                          | /  ∠theta
//            ーーーーーー newUV ーーーーーー⇒ u+ [-0.5, 0.5]
//                          |
//                          |
//                          |
//                          |
//                          |
                // 現在のピクセルと新しい u軸 との角度
                float theta = degrees(atan2(newUV.y, newUV.x));
                // thetaを [0, 360) に納める 
                theta = (theta < 0) ? (theta + 360) : theta;
                
                // theta が二つのエッジの角度の間にあるかどうか判定
                bool inSector = false;
                if (edgeAngle[0] < edgeAngle[1]) {
                    inSector = (theta >= edgeAngle[0] && theta <= edgeAngle[1]);
                } else {
                    inSector = (theta >= edgeAngle[0] || theta <= edgeAngle[1]);
                }
                
                // 扇形領域内にあり、中心位置との距離がエッジ長より短く、かつ isInLaneEdge の範囲外なら、半透明背景
                if (inSector && dist < maxRadius && isInLaneEdge == 0) {
                    isInPI = 1;
                }

                // ----------------------------- 円周エッジ ----------------------------- 
                int isInOuterEdge = 0;
                if (isInPI == 1 && dist < maxRadius && dist >= maxRadius - circleWidth)
                {
                    isInOuterEdge = 1;
                }

                // ----------------------------- Final Color ----------------------------- 
                if (isInLaneEdge > 0 || isInOuterEdge > 0) // エッジ部分
                {
                    col = _LaneEdgeColor;
                }
                else if (isInPI > 0) // 背景部分
                {
                    col = _LaneBackgroundColor;
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
