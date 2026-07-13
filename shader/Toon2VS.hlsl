#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // 頂点変換処理（この処理は必ず必要）
    matrix wvp;
    wvp = mul(World, View); // wvp = ワールド行列 * カメラ行列
    wvp = mul(wvp, Projection); // wvp = wvp * プロジェクション行列
    Out.Position = mul(In.Position, wvp); // 変換結果を出力する

    // 頂点法線をワールド行列で回転させる
    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    Out.Diffuse = In.Diffuse; // 頂点色をそのまま出力
    Out.TexCoord = In.TexCoord; // UV座標をそのまま出力

    // ワールド変換した頂点座標を出力
    Out.WorldPosition = mul(In.Position, World);
}
