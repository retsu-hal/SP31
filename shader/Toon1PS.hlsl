#include "Common.hlsl"

Texture2D g_Texture : register(t0); // テクスチャ0番
SamplerState g_SamplerState : register(s0); // サンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //--- ポイントライトと同じ流れで明るさを計算 ---------------------------
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    // 物体と光源の距離
    float ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);

    // 減衰の計算
    float ofs = saturate(1.0f - ld / Light.PointLightParam.x);
    ofs = max(0.0f, ofs); // 減衰率0未満は0にする

    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);

    // 光源計算（ハーフランバート）
    // lv は「光源→ピクセル」方向なので符号を反転している
    float light = 0.5f - 0.5f * dot(normal.xyz, lv.xyz);
    light = saturate(light);

    //--- 明るさの段階分け（トゥーン） ------------------------------------
    // Parameter.x = Level-1、Parameter.y = Level-2（C言語側から設定）
    if (light < Parameter.x)
    {
        light = 0.4f; // 一番暗い段階（0.0にすると真っ黒になるので避ける）
    }
    else if (light < Parameter.y)
    {
        light = 0.7f; // 中間の段階
    }
    else
    {
        light = 1.0f; // 一番明るい段階
    }
    //---------------------------------------------------------------------

    light *= ofs; // 明るさを減衰させる

    // テクスチャと頂点色の情報を合成（スペキュラーは無し）
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord); // テクセルを取得
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb; // 明るさを乗算
    outDiffuse.a *= In.Diffuse.a; // α値に明るさは関係ない
    
    //簡易エッジ作成
    float4 eyev = In.WorldPosition - CameraPosition; //視線ベクトル作成（カメラ→ピクセル）
    eyev = normalize(eyev); //正規化する

    float d = dot(eyev.xyz, normal.xyz); // 視線とピクセル法線の内積

    if (d > -0.30f)  //適当なしきい値でエッジか否かを判別
    {
        outDiffuse.rgb = float3(0.0f, 0.0f, 0.0f); //エッジなら適当に色を黒くする
    }
}
