#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// Parameter.xy : 画像サイズ(1280,720)  .z : モザイクの矩形サイズ  .w : 円の半径（後で使う）
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 uv = In.TexCoord;

    //全画面モザイク
    {
        uv *= Parameter.xy; // UV × 解像度 → 画面座標
        uv /= Parameter.z; // ÷ モザイクサイズ → ブロック座標
        uv = floor(uv) * Parameter.z; // 切り捨て × モザイクサイズ → ブロック左上の座標
        uv /= Parameter.xy; // ÷ 解像度 → UVに戻す
        uv = clamp(uv, 0.001f, 0.999f); // ノイズ対策（WRAPで反対側を拾わない）
    }
    
    /*
    //円形マスクモザイク
    {
        float2 len = float2(0.5f, 0.5f); // 中心を画面中央とする
        len -= In.TexCoord.xy; // ピクセルから中心へのベクトル
        len.x *= (Parameter.x / Parameter.y); // アスペクト比補正（まずはこの行なしで試す）
        len = length(len); // ベクトルの長さ

        if (len.x < Parameter.w)                    // 指定半径の内部なら
        {
            uv *= Parameter.xy;
            uv /= Parameter.z;
            uv = floor(uv) * Parameter.z;
            uv /= Parameter.xy;
            uv = clamp(uv, 0.001f, 0.999f);
        }
    }
    */
    
    outDiffuse = g_Texture.SampleLevel(g_SamplerState, uv, 0.0f);
    outDiffuse *= In.Diffuse;
}