#include "common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_TextureRamp : register(t1); // ★ランプ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 lv = In.WorldPosition - Light.Position;
    float ld = length(lv);
    lv = normalize(lv);
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);

    float4 normal = normalize(In.Normal);
    float light = 0.5f - 0.5f * dot(normal.xyz, lv.xyz);

    // 端っこを使わない（Wrap補間で反対側の色が混ざるのを防ぐ）
    light = clamp(light, 0.01f, 0.99f);
    float texv = clamp(Parameter.x, 0.01f, 0.99f); // V = どの行を使うか

    float4 toon = g_TextureRamp.Sample(g_SamplerState, float2(light, texv));
    toon *= ofs;

    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= toon.rgb * In.Diffuse.rgb + Light.Ambient.rgb; // lightの代わりにtoon
    outDiffuse.a *= In.Diffuse.a;

    // 簡易エッジ（Toon3で背面法を使うときは消してOK）
    float3 eyev = normalize(In.WorldPosition.xyz - CameraPosition.xyz);
    if (dot(normal.xyz, eyev) > -0.25f)
    {
        outDiffuse.rgb *= 0.3f;
    }
}