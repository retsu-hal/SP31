#include "common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ピクセル→光源（いつもと逆）
    float3 lv = Light.Position.xyz - In.WorldPosition.xyz;
    float ld = length(lv);
    lv = normalize(lv);

    // 減衰（PointPixelLightingと同じ式）
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);

    float3 normal = normalize(In.Normal.xyz);

    // 拡散光
    float light = saturate(0.5f + 0.5f * dot(normal, lv));
    light *= ofs;

    float4 albedocolor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse = albedocolor;
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;

    // ピクセル→カメラ（いつもと逆）
    float3 eyev = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    float3 halfv = normalize(eyev + lv);

    float nv = saturate(dot(normal, eyev));
    float nh = saturate(dot(normal, halfv));
    float vh = saturate(dot(eyev, halfv));
    float nl = saturate(dot(normal, lv));

    // 今回はテクスチャではなくImGuiの値
    float rough = saturate(Parameter.x);
    float metallic = saturate(Parameter.y);

    float D = CalculateBeckmann(rough, nh);
    float G = CalculateGeometricDamping(nh, nv, nl, vh);
    float F = CalculateFresnel(metallic, vh);

    float3 specular = max(0.0f, F * D * G / max(nv, 0.0001f)) * albedocolor.rgb;

    outDiffuse.rgb += specular * ofs; // 「=」にするとスペキュラだけ見える
}