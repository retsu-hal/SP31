#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // 頂点変換（World → View → Projection）
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);

    // 法線をワールド行列で回転（w=0 にして平行移動を無視）
    float4 normal = float4(In.Normal.xyz, 0.0);
    float4 worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    // 頂点色とUVはそのまま渡す
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;

    // ワールド座標を出力（PSで「カメラ→ピクセル」の視線を作るのに必須）
    Out.WorldPosition = mul(In.Position, World);
}