#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // 頂点変換処理（この処理は必ず必要）
    matrix wvp;
    wvp = mul(World, View); // wvp = ワールド行列 * カメラ行列
    wvp = mul(wvp, Projection); // wvp = wvp * プロジェクション行列
    Out.Position = mul(In.Position, wvp); // 変換結果を出力する

    // 頂点法線をワールド行列で回転させる（頂点と同じ回転をさせる）
    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0); // 入力法線ベクトルのwを0としてコピー
    worldNormal = mul(normal, World); // 法線をワールド行列で回転する
    worldNormal = normalize(worldNormal); // 回転後の法線を正規化する
    Out.Normal = worldNormal; // 回転後の法線出力

    Out.Diffuse = In.Diffuse; // 頂点色をそのまま出力
    Out.TexCoord = In.TexCoord; // UV座標をそのまま出力

    // ワールド変換した頂点座標を出力
    Out.WorldPosition = mul(In.Position, World);
}
