#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    // 物体と光源の距離（正規化する前に取る）
    float ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);

    // 減衰の計算
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);
    ofs = max(0, ofs);

    // 拡散光
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs; // 明るさを減衰させる

    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;

    // スペキュラー（ブリンフォン）
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    float3 halfv = eyev + lv.xyz; // 視線ベクトル＋ライトベクトル
    halfv = normalize(halfv);

    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);

    outDiffuse.rgb += (specular * ofs); // スペキュラも減衰させてから加算
    
    /* ---------- リムライティングの処理 ---------- */

    // 光の方向と視線ベクトルの考慮（逆光ほど明るい）
    // lv:光→ピクセル  eyev:カメラ→ピクセル
    // 同じ向き（順光）だと dot=1 → lit=0
    float lit;
    lit = 1.0f - max(0.0f, dot(lv.xyz, eyev.xyz));

    // 輪郭部分ほど明るくする（直角に近いほど明るい）
    // -eyev はピクセル→カメラ。正面は dot=1 → lim=0、輪郭は dot=0 → lim=1
    float lim;
    lim = 1.0f - max(0.0f, dot(normal.xyz, -eyev.xyz));

    // lit と lim の明るさを合成
    lim *= lit;

    // 見え方を調整
    lim = pow(lim, Light.PointLightParam.x);

    // リムの明るさをディフューズに加算
    outDiffuse.rgb += lim;
}