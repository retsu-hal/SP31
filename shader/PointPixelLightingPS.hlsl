#include "Common.hlsl"

Texture2D g_Texture : register(t0);
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
    //減衰率0未満は0にする
    ofs = max(0.0f, ofs);
    //-------------------------------------------------------------
    //法線を正規化
    float4 normal = normalize(In.Normal);
    //光源計算
    float light = -dot(normal.xyz, lv.xyz);
    //lightをサチュレート
    light = saturate(light);
    //明るさを減衰する
    light *= ofs;

    outDiffuse.rgba = g_Texture.Sample(g_SamplerState, In.TexCoord); //テクセルを取得
    //テクセルに明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a; //α値は別処理
    //-------------------------------------------------------------

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
