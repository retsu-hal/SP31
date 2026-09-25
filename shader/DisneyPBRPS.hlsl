#include "common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0); // アルベド
Texture2D g_TextureRoughness : register(t1); // ラフネス
Texture2D g_TextureMetalness : register(t2); // メタリック
SamplerState g_SamplerState : register(s0);

static const float PI = 3.1415926f;

float CalculateDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    float nl = saturate(dot(N, L));
    float nv = saturate(dot(N, V));
    return nl * nv;
}

float CalculateCookTorranceSpecular(float3 L, float3 V, float3 N, float rough, float metallic)
{
    float3 H = normalize(L + V);

    float nh = saturate(dot(N, H));
    float vh = saturate(dot(V, H));
    float nl = saturate(dot(N, L));
    float nv = saturate(dot(N, V));

    float D = CalculateBeckmann(rough, nh);
    float F = CalculateFresnel(metallic, vh);
    float G = CalculateGeometricDamping(nh, nv, nl, vh); // 4番目は vh

    float m = max(PI * nv * nh, 0.0001f);
    return max(F * D * G / m, 0.0f);
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float3 normal = normalize(In.Normal.xyz);

    float4 albedoColor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float3 specColor = albedoColor.rgb;

    // テクスチャから粗さ・金属度を取得（0.5以下は0に切り捨て）
    float rough = saturate(g_TextureRoughness.Sample(g_SamplerState, In.TexCoord).r * 2.0f - 1.0f);
    float metallic = saturate(g_TextureMetalness.Sample(g_SamplerState, In.TexCoord).r * 2.0f - 1.0f);

    float3 eyev = normalize(CameraPosition.xyz - In.WorldPosition.xyz);

    // ライトマシマシ（同じライトをParameter.z回足す＝明るさ×回数）
    float3 lit = 0;
    for (int ligNo = 0; ligNo < (int) Parameter.z; ligNo++)
    {
        float3 lv = normalize(Light.Position.xyz - In.WorldPosition.xyz); // 正規化する

        float diffuseFromFresnel = CalculateDiffuseFromFresnel(normal, lv, eyev);

        // 正規化Lambert
        float nl = saturate(dot(normal, lv));
        float3 light = nl + Light.Diffuse.rgb / PI; // スライドのまま

        float3 diffuse = albedoColor.rgb * diffuseFromFresnel * Light.Diffuse.rgb * light;

        float3 spec = CalculateCookTorranceSpecular(lv, eyev, normal, rough, metallic) * Light.Diffuse.rgb;
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic); // 金属ほど素材色

        lit += diffuse * (1.0f - rough) + spec;
    }

    lit += Light.Ambient.rgb * albedoColor.rgb; // 環境光

    outDiffuse.rgb = lit;
    outDiffuse.a = albedoColor.a * In.Diffuse.a;
}