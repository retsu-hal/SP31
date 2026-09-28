#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// Parameter.x : Seed  .y : MIN  .z : MAX  .w : POW

//適当なSeed値をもとに疑似乱数を計算する
float Rand(float2 coord)
{
    //frac(x)=xの小数部を返す 乱数は0.0 <= rand < 1.0の範囲
    return frac(sin(dot(coord.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //テクセルの取得
    float4 Tex = g_Texture.Sample(g_SamplerState, In.TexCoord);

    //Parameter.y〜Parameter.zの乱数値を作成
    float noise = clamp(Rand(In.TexCoord + Parameter.x), Parameter.y, Parameter.z);

    //乱数値を輝度としてテクセルへ加算
    Tex.rgb += noise;

    outDiffuse.rgb = saturate(Tex.rgb) * In.Diffuse.rgb;
    outDiffuse.a = Tex.a * In.Diffuse.a;

    //グレースケール化（GrayscalePS.hlsl と同じ式）
    float Y = outDiffuse.r * 0.299f + outDiffuse.g * 0.587f + outDiffuse.b * 0.114f;
    outDiffuse.rgb = Y;

    //コントラスト調整
    outDiffuse.rgb = pow(outDiffuse.rgb, Parameter.www);
}