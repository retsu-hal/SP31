#include "Common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_TextureNormal : register(t1);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    //物体と光源の距離
    float4 ld = length(lv);
    //ベクトルの正規化
    lv = normalize(lv);
    //減衰の計算
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);
    //-------------------------------------------------------------
    //法線マップ取得
    float4 tmpnormal = g_TextureNormal.Sample(g_SamplerState, In.TexCoord);
    //RGB値をスケーリング
    tmpnormal = normalize((tmpnormal * 2.0f)-1.0f);
  
    float3 normal;
    normal.x = tmpnormal.r;
    normal.y = tmpnormal.g;
    normal.z = tmpnormal.b;
    
    float3 N = float3(0.0f, 1.0f, 0.0f);
    float3 T = float3(1.0f, 0.0f, 0.0f);
    float3 B = float3(0.0f, 0.0f, 1.0f);
    float3x3 Rot = float3x3(T, B, N);
    normal = normalize(mul(normal, Rot));
    //-------------------------------------------------------------

    //明るさ計算
    float light = -dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs;
    
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a = In.Diffuse.a;
    
    //カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); //正規化する

    //ハーフベクトル
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);

    float specular = -dot(halfv, normal.xyz); //鏡面反射の計算
    specular = saturate(specular); //値をサチュレート
    specular = pow(specular, 30);

    outDiffuse.rgb += (specular * ofs);

}
