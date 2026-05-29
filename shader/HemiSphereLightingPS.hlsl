#include "common.hlsl"

Texture2D g_Texture : register(t0); //テクスチャ0番
SamplerState g_SamplerState : register(s0); //サンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, Light.Direction.xyz); //光源計算をする
    
    outDiffuse = 1.0f; //とりあえず白
    
    outDiffuse.rgb *= light;
    outDiffuse.a = In.Diffuse.a; //アルファは頂点カラーから取る
    
    //半球ライティング
    float norm = dot(normal, Light.GroundNormal);
    norm = (norm + 1.0f) / 2.0f;
    float3 hemiColor = lerp(Light.GroundColor, Light.SkyColor, norm);
    
    outDiffuse.rgb += hemiColor;
    
    return;
}








