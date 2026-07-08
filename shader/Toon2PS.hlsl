#include "Common.hlsl"

Texture2D g_Texture : register(t0); // 絵柄テクスチャ（ドーナツの模様）
Texture2D g_TextureRamp : register(t1); // ① ランプテクスチャ（明るさのLUT）
SamplerState g_SamplerState : register(s0); // サンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //--- ② これまで通り普通に明るさを計算 -------------------------------
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    float ld = length(lv);
    lv = normalize(lv);

    // 減衰の計算
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);
    ofs = max(0.0f, ofs);

    // 法線を正規化して光源計算（ハーフランバート）
    float4 normal = normalize(In.Normal);
    float light = 0.5f - 0.5f * dot(normal.xyz, lv.xyz);
    light = saturate(light);

    light *= ofs; // 距離による減衰（不要ならこの行は外してもよい）

    //--- ③ 明るさをクランプ ----------------------------------------------
    // サンプラーがWRAP設定のため、0.0/1.0ちょうどだと逆側の色と補間されて
    // 色が混ざる。ほんの少し内側へ寄せる（1ピクセル分≒1/256程度）。
    float u = clamp(light, 0.004f, 0.996f);

    //--- ④ 明るさをU座標として、ランプテクスチャから色を取得 ------------
    // V（縦）は Parameter.x で固定（既定0.5）。横一様の画像なら値は何でもよい。
    float4 toon = g_TextureRamp.Sample(g_SamplerState, float2(u, Parameter.x));

    //--- ⑤ 取得した色を「明るさ」として lightの代わりに乗算 -------------
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord); // 絵柄を取得
    outDiffuse.rgb *= In.Diffuse.rgb * toon.rgb + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a; // α値に明るさは関係ない
}
