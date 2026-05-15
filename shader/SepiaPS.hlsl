
#include "Common.hlsl"
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    outDiffuse = In.Diffuse;

    outDiffuse *= g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    //Y（輝度変更）
    float Y;
    Y = outDiffuse.r * 0.299f + outDiffuse.g * 0.587f + outDiffuse.b * 0.114f;
    outDiffuse.rgb = Y;
    
    //セピア調変換
    float4 sepia = float4(0.96f, 0.784f, 0.58f, 1.0f);
    outDiffuse *= sepia;
    
}
