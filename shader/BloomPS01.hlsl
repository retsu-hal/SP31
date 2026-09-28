#include "Common.hlsl"

Texture2D g_Texture : register(t0); // RT0
SamplerState g_SamplerState : register(s1); // CLAMP

// Parameter.x : しきい値
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse;

    // 輝度抽出
    {
        float Luminance = dot(outDiffuse.rgb, float3(0.299f, 0.587f, 0.114f));
        Luminance = pow(Luminance, 1); // 調節用
        if (Luminance < Parameter.x)	// しきい値未満は光らせない
        {
            Luminance = 0.0f;
        }
        Luminance = saturate(Luminance);
        outDiffuse.rgb = Luminance;
    }
}