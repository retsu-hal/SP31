#include "common.hlsl"

Texture2D g_Texture : register(t0); // テクスチャ0番
SamplerState g_SamplerState : register(s0); // サンプラー0番

//==============================================================
// ブリンフォン用 ピクセルシェーダー
//  色 = テクスチャ × (頂点色 × 光の色 × 拡散 + 環境光) + 鏡面反射
//==============================================================
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //------------------------------------------
    // ① 拡散光（ランバート）
    //------------------------------------------
    float4 normal = normalize(In.Normal); // ピクセルの法線を正規化
    float light = -dot(normal.xyz, Light.Direction.xyz); // Light.Direction は「光→面」なので −
    light = saturate(light); // 0〜1 に収める

    // テクスチャ色 × (頂点色 × 光の色 × 明るさ + 環境光)
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a; // αに明るさは関係ないので別計算

    //------------------------------------------
    // ② 鏡面反射（ブリンフォン）
    //------------------------------------------
    // カメラ → ピクセル の視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    // ハーフベクトル（視線 + ライト。どちらも「面へ向かう」向きなので結果も逆向き）
    float3 halfv = eyev + Light.Direction.xyz;
    halfv = normalize(halfv);

    // ハーフベクトルと法線の内積（逆向きなので − を付けて戻す）
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30); // 指数が大きいほど鋭いハイライト

    outDiffuse.rgb += specular; // ハイライトは足しこむ
}