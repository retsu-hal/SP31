
#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    
    //光源計算
    float4 WorldNormal, Normal;
    
    Normal = float4(In.Normal.xyz, 0.0);
    WorldNormal = mul(Normal, World); //法線を回転
    WorldNormal = normalize(WorldNormal); //法線を正規化
    Out.Normal = WorldNormal; //法線を出力
    
    
    //明るさ、テクスチャ座標の出力
    Out.Diffuse.rgb = In.Diffuse; //頂点色
    Out.TexCoord = In.TexCoord;
}