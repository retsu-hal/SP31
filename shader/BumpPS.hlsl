#include "common.hlsl"

Texture2D g_Texture : register(t0); // 色テクスチャ
Texture2D g_TextureNormal : register(t1); // 法線マップ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    float ld = length(lv); // 正規化する前に距離を取る
    lv = normalize(lv);

    // 減衰
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);

    //---------------- ここから法線マップ ----------------
    // t1 から法線を取得（0～1の色）
    float4 tmpnormal = g_TextureNormal.Sample(g_SamplerState, In.TexCoord);
    // 0～1 → -1～1 に戻して正規化
    tmpnormal = normalize(tmpnormal * 2.0f - 1.0f);

    float3 normal;
    normal.x = tmpnormal.r;
    normal.y = tmpnormal.g;
    normal.z = tmpnormal.b;

    // 地面（真上向き）と仮定したTBN
    float3 N = float3(0.0f, 1.0f, 0.0f);
    float3 T = float3(1.0f, 0.0f, 0.0f);
    float3 B = float3(0.0f, 0.0f, 1.0f);
    float3x3 Rot = float3x3(T, B, N);
    normal = normalize(mul(normal, Rot)); // 法線マップのZ(上) → ワールドのY(上)
    //----------------------------------------------------

    // 拡散光
    float light = -dot(normal, lv.xyz);
    light = saturate(light);
    light *= ofs;

    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;

    // スペキュラ（ブリンフォン）
    float3 eyev = normalize(In.WorldPosition.xyz - CameraPosition.xyz);
    float3 halfv = normalize(eyev + lv.xyz);
    float specular = saturate(-dot(halfv, normal));
    specular = pow(specular, 30);

    outDiffuse.rgb += specular * ofs;
}