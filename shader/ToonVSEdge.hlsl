#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // ★モデルの拡大（WVP変換の前に行う）
    float4 pos = In.Position;
    float3 n = normalize(In.Normal.xyz);
    pos.xyz += n * Parameter.y; // Parameter.y = エッジの太さ

    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(pos, wvp); // ★拡大後の座標を変換

    float4 normal = float4(In.Normal.xyz, 0.0);
    Out.Normal = normalize(mul(normal, World));

    // ★頂点色を黒で強制出力（αは0にしない！）
    Out.Diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
    Out.TexCoord = In.TexCoord;
    Out.WorldPosition = mul(pos, World);
}