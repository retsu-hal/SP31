
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
    /*-----------------------------------------
        normal.x=In.Normal.x;
        normal.y=In.Normal.y;
        normal.z=In.Normal.z;
        normal.w=0.0;
    -----------------------------------------
        normal.xyz=In.Normal.xyz;
        normal.w=0.0;
    -----------------------------------------*/
    WorldNormal = mul(Normal, World);                   //法線を回転
    WorldNormal = normalize(WorldNormal);           //法線を正規化
    Out.Normal = WorldNormal;                                //法線を出力
    
    //明るさを計算　光ベクトルと法線の内積計算
    float light = -dot(Light.Direction.xyz,WorldNormal.xyz);
    light = saturate(light); //lightの値を0～1に収める
    //lightに頂点の明るさができる
    
    //明るさ、α値、テクスチャ座標の出力
    Out.Diffuse.rgb = light*In.Diffuse;                //明るさ*頂点色
    Out.Diffuse.a = In.Diffuse.a;         //αは頂点カラーをつかう
    Out.TexCoord = In.TexCoord;   
}