#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // ★WVP変換の「前」に法線方向へ押し出す
    float4 pos = In.Position;
    pos.xyz += normalize(In.Normal.xyz) * Parameter.y; // y = エッジの太さ

    matrix wvp = mul(mul(World, View), Projection);
    Out.Position = mul(pos, wvp);

    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.0f), World));
    Out.WorldPosition = mul(pos, World);
    Out.TexCoord = In.TexCoord;

    // ★色は強制的に黒。αは0にしない
    Out.Diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
}