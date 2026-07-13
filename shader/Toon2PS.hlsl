#include "Common.hlsl"

Texture2D g_Texture : register(t0); // 絵柄テクスチャ（ドーナツの模様）
Texture2D g_TextureRamp : register(t1); // ① ランプテクスチャ（明るさのLUT）
SamplerState g_SamplerState : register(s0); // サンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
   //光源からピクセルへのベクトル
    float4 lv = Light.Position - In.WorldPosition;
    //物体から広原までの距離
    float4 ld = length(lv);
    //ベクトルの正規化
    lv = normalize(lv);
    
    //減衰の計算
    float ofs = 1.0 - (1.0f / Light.PointLightParam.x) * ld;
    //減衰率0未満は0にする
    ofs = max(0.0f, ofs);
    
    //ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    
    //光源計算
    float light = 0.5f + 0.5f * dot(normal.xyz, lv.xyz);
    //明るさをクランプ
    light = clamp(light, 0.01f, 0.99f); //端っこはサンプルしないようにする

    //サンプル用特殊処理
    float texv = Parameter.x;
    texv = clamp(texv, 0.01f, 0.99f); //端っこはサンプルしないようにする
    
    //lightをテクスチャ座標ｘとして、テクスチャから明るさを取得する
    float4 toon = g_TextureRamp.Sample(g_SamplerState, float2(light, texv));
    toon *= ofs; //減衰率を掛ける
    
    //テクスチャの色を取得する
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord) ;
    outDiffuse.rgb *= toon.rgb * In.Diffuse.rgb + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    
}
