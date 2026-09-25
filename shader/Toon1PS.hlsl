#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 lv = In.WorldPosition - Light.Position; // 光→ピクセル
    float ld = length(lv);
    lv = normalize(lv);

    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);

    float4 normal = normalize(In.Normal);

    // ハーフランバート（lvが逆向きなので符号反転）
    float light = 0.5f - 0.5f * dot(normal.xyz, lv.xyz);
    light = saturate(light);

    // ★段階分け（Parameter.x = Level-1, Parameter.y = Level-2）
    if (light < Parameter.x)
        light = 0.4f; // 0.0にしない（真っ黒になる）
    else if (light < Parameter.y)
        light = 0.7f;
    else
        light = 1.0f;

    light *= ofs; // 減衰は段階分けの「後」

    // スペキュラーは入れない
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;

    // 簡易エッジ（資料02）
    float3 eyev = normalize(In.WorldPosition.xyz - CameraPosition.xyz); // カメラ→ピクセル
    float d = dot(normal.xyz, eyev); // 正面≈-1、輪郭≈0
    if (d > -0.30f)
    {
        outDiffuse.rgb *= 0.3f; // 真っ黒なら = float3(0,0,0)
    }
}