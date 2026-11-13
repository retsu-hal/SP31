#include "common.hlsl"

//==============================================================
// 投影シャドウ：影を受ける側（地面）の頂点シェーダー
//  PixelLightingBlinnPhongVS ＋ ライト空間の座標を出力
//==============================================================
void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp); // 頂点変換

    // 頂点法線をワールド行列で変換して出力
    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0f);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    Out.Diffuse = In.Diffuse;   // 頂点色の出力
    Out.TexCoord = In.TexCoord; // テクスチャ座標の出力

    // ワールド変換した頂点座標を出力
    Out.WorldPosition = mul(In.Position, World);

    // ライト空間の座標へ変換（C++側で転置していないのでここで転置）
    wvp = mul(World, transpose(Light.ViewMatrix));
    wvp = mul(wvp, transpose(Light.ProjectionMatrix));
    Out.LightPosition = mul(In.Position, wvp);
}
