
#include "Common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
    //頂点変換処理（WVP）
    matrix wvp;
    wvp = mul(World, View);         //wvp = ワールド * カメラ
    wvp = mul(wvp, Projection);     //wvp = wvp * プロジェクション
    Out.Position = mul(In.Position, wvp);

    //頂点法線をワールド行列で回転させる
    float4 normal = float4(In.Normal.xyz, 0.0);
    float4 worldNormal = mul(normal, World);
    Out.Normal = normalize(worldNormal);

    Out.Diffuse  = In.Diffuse;      //頂点色をそのまま
    Out.TexCoord = In.TexCoord;     //UVをそのまま

    //ワールド変換した頂点座標（ライティング計算に使用）
    Out.WorldPosition = mul(In.Position, World);
}
