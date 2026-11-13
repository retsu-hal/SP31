#include "common.hlsl"

//==============================================================
// 投影シャドウ：シャドウマップ作成用 頂点シェーダー（影を描く用）
//  View / Projection にはライトカメラの行列がセットされている
//==============================================================
void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp); // 頂点変換

    // 未使用だが初期化しておく
    Out.WorldPosition = mul(In.Position, World);
    Out.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;
    Out.LightPosition = Out.Position;
}
