
#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    
    //法線をワールド空間へ回転
    float4 normal = float4(In.Normal.xyz, 0.0f);
    float4 worldNormal = mul(normal, World);
    Out.Normal = normalize(worldNormal); //法線を正規化
    
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
   
    Out.WorldPosition = mul(In.Position, World); //ワールド空間での頂点座標を計算
}