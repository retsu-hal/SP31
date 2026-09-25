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
}