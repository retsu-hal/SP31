#include "Common.hlsl"

Texture2D g_TextureBlur : register(t0); // RT0 ボケ画像
Texture2D g_TextureScene : register(t1); // RT2 通常シーン
Texture2D g_TextureDepth : register(t2); // 深度
SamplerState g_SamplerState : register(s1); // CLAMP

// Parameter.x:ピント位置  y:ピント幅
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 blur = g_TextureBlur.SampleLevel(g_SamplerState, In.TexCoord, 0);
    float4 scene = g_TextureScene.SampleLevel(g_SamplerState, In.TexCoord, 0);
    float4 depth = g_TextureDepth.SampleLevel(g_SamplerState, In.TexCoord, 0);

    float d = abs(Parameter.x - depth.r);
    if (d > Parameter.y)
        d = 0.0f; // 完全ピンボケ
    else
        d = 1.0f - lerp(0.0f, 1.0f, d / Parameter.y); // 1に近いほどピントが合ってる

    float4 color = lerp(blur, scene, smoothstep(0.0f, 1.0f, d));
    outDiffuse.rgb = color.rgb;
    outDiffuse.a = scene.a;
    outDiffuse *= In.Diffuse;
}