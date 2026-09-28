#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s1); // ★CLAMP の1番

// Parameter.x:画面幅  y:画面高さ  z:分散(ボケ強さ)  w:サンプリング間隔
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 step = float2(1.0f / Parameter.x, 0.0f) * Parameter.w; // 横に1マス

    float4 center = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.a = center.a;
    outDiffuse.rgb = Weight[0].x * center.rgb; // 代表点

    [unroll]
    for (int i = 1; i < 8; i++)
    {
        float w = Weight[i / 4][i % 4];
        outDiffuse.rgb += w * g_Texture.Sample(g_SamplerState, In.TexCoord + step * i).rgb; // 右
        outDiffuse.rgb += w * g_Texture.Sample(g_SamplerState, In.TexCoord - step * i).rgb; // 左
    }
}