#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float2 uv = In.TexCoord;

    // R：左に Parameter.x だけずらした場所から .r だけ取る
    outDiffuse.r = g_Texture.Sample(g_SamplerState, float2(uv.x - Parameter.x, uv.y)).r;
    // G：正規の座標
    outDiffuse.g = g_Texture.Sample(g_SamplerState, float2(uv.x, uv.y)).g;
    // B：右に Parameter.y だけずらした場所から .b だけ取る
    outDiffuse.b = g_Texture.Sample(g_SamplerState, float2(uv.x + Parameter.y, uv.y)).b;
    // A：正規の座標
    outDiffuse.a = g_Texture.Sample(g_SamplerState, float2(uv.x, uv.y)).a;
    
    // 左右端の外側は黒にする
    float2 uvR = float2(uv.x - Parameter.x, uv.y);
    outDiffuse.r = g_Texture.Sample(g_SamplerState, uvR).r;
    if (uvR.x < 0.0f)
        outDiffuse.r = 0.0f; // 左端の外

    float2 uvB = float2(uv.x + Parameter.y, uv.y);
    outDiffuse.b = g_Texture.Sample(g_SamplerState, uvB).b;
    if (uvB.x > 1.0f)
        outDiffuse.b = 0.0f; // 右端の外

    // 頂点の色を合成
    outDiffuse *= In.Diffuse;
}