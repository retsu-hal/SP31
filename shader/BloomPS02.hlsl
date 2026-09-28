#include "Common.hlsl"

SamplerState g_SamplerState : register(s1); // CLAMP
Texture2D g_Texture : register(t0); // RT0 通常シーン
Texture2D g_Texture1 : register(t1); // RT1 輝度マップ（ミップ付き）

// Parameter.y,z,w : ミップ 2.5 / 4.5 / 6.5 を混ぜる強さ
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.SampleLevel(g_SamplerState, In.TexCoord, 0);

    // ミップの3段をブレンド（中間値でレベル間も補間される）
    float3 Luminance = 0;
    Luminance += g_Texture1.SampleLevel(g_SamplerState, In.TexCoord, 2.5f).rgb * Parameter.y;
    Luminance += g_Texture1.SampleLevel(g_SamplerState, In.TexCoord, 4.5f).rgb * Parameter.z;
    Luminance += g_Texture1.SampleLevel(g_SamplerState, In.TexCoord, 6.5f).rgb * Parameter.w;

    outDiffuse.rgb += saturate(Luminance);
    outDiffuse *= In.Diffuse;
}