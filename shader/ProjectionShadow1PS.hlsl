#include "common.hlsl"

Texture2D g_Texture : register(t0);              // 地面のテクスチャ
Texture2D g_TextureShadow : register(t1);        // シャドウマップ
SamplerState g_SamplerState : register(s0);      // WRAP
SamplerState g_SamplerStateShadow : register(s2); // BORDER（範囲外は白＝影なし）

//==============================================================
// 投影シャドウ：影を受ける側（地面）のピクセルシェーダー
//  点光源（ハーフランバート＋ブリンフォン）×シャドウマップ
//==============================================================
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 光源から物体へのベクトル
    float3 lv = In.WorldPosition.xyz - Light.Position.xyz;
    // 物体と光源の距離
    float ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);

    // 減衰の計算（0未満は0にする）
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    ofs = max(0.0f, ofs);

    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    // 光源計算（ハーフランバート版）
    float light = 0.5f - 0.5f * dot(normal.xyz, lv);
    light *= ofs; // 明るさを減衰

    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; // 明るさを乗算
    outDiffuse.a *= In.Diffuse.a;             // α値に明るさは関係ない

    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    // ハーフベクトルを計算
    float3 halfv = eyev + lv;
    halfv = normalize(halfv);

    // スペキュラーの計算
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);
    outDiffuse.rgb += (specular * ofs);
    // ここまでは普通の光源計算

    // 投影シャドウの処理
    float2 uv = In.LightPosition.xy / In.LightPosition.w; // デバイス座標(-1.0〜1.0)へ変換
    uv *= float2(0.5f, -0.5f);                            // テクスチャ座標(0.0〜1.0)へ変換
    uv += 0.5f;
    // シャドウマップの色を合成
    outDiffuse.rgb *= g_TextureShadow.Sample(g_SamplerStateShadow, uv).rgb;
}
