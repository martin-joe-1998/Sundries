struct VOut 
{ 
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0; 
};

// Unity のスタイルで、vertex shader の input を構造体化しようとしたが、.shader のフレームワークではそれが許されないらしい…
VOut VShader(float3 position : POSITION, float2 uv : TEXCOORD0, float4 color : COLOR) 
{ 
    VOut output; 

    output.position = float4(position, 1.0);
    output.uv = uv;
    output.color = color; 

    return output; 
} 

// この shader は、コメントの文字化けを回避するために UTF-8 を使っています！そのままダウンロードして使うと、Visual Studioではコンパイルできなくなります！
// なので、サブスクリーンで見ながらの手入力や、コピペすることをお勧めします！
float4 PShader(float4 position : SV_POSITION, float2 uv : TEXCOORD0, float4 color : COLOR0) : SV_TARGET 
{ 
    // 適当に黒のベースカラーを決める
    float4 finalCol = float4(0.0, 0.0, 0.0, 1.0);

    // センター位置を決める
    float2 center = float2(0.5, 0.5);
    // UV を [0, 1] から [-0.5, 0.5] へ変換する。今回はテクスチャーを使わないので、後で戻す必要がない
    float2 newUV = uv - center;
    // 普通のxy平面を想像して、theta は原点からある点に繋いだ直線とx軸の正の方向との角度(ラジアンから変換した)
    float theta = degrees(atan2(newUV.y, newUV.x));
    // [-180, 180) -> [0, 360)
    theta = (theta < 0) ? (theta + 360) : theta; // [0, 360)

    // 円環の幅、内側半径と外側半径を決める
    float circleWidth = 0.05;
    float innerRadius = 0.45;
    float outerRadius = innerRadius + circleWidth;

    // 平面上の各ピクセルから中心位置への距離を計算
    float dist = length(newUV);
    // 円環の範囲を判定
    float isInRing = step(innerRadius, dist) * step(dist, outerRadius);

    // 円環の範囲内なら、頂点の色を自動線形補間した色を出力
    if (isInRing > 0.0)
    {
        finalCol = color;
    }
    // 範囲外なら、現在のピクセルを捨てる
    else
    {
        clip(-1);
    }

    // この Shader は今、楕円を描画しています！
    // なぜ円ではなく楕円なのかというと、cbuffer から送られるはずの行列による行列変換をしていないからです。
    // VShader の output.position = float4(position, 1.0); は行列変換なしで、
    // 物体空間の頂点をそのままスクリーン空間にマッピングしたため、円はWindowの縦横幅に依存するものになっています。
    // cbuffer を使って、行列変換をして3D描画をしてみよう！注意点として、[World] Matrix ではスケールを100以上に設定することを推奨します。
    // じゃないと、一ピクセルのスライムを再現することになるっ…
    return finalCol; 
}