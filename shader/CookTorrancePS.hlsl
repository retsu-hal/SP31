#include "Common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    //ピクセルから光源へのベクトル(いつもと逆)
    float4 lv = Light.Position - In.WorldPosition;
    float4 ld = length(lv); //物体と光源の距離
    lv = normalize(lv); //ベクトルの正規化
    //減衰の計算
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    ofs = max(0, ofs); //減衰率0未満は0にする

    //ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    //光源計算
    float light = 0.5f + 0.5f * dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs; //明るさを減衰させる

    //テクスチャのピクセル色を取得
   // float4 albedocolor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float4 albedocolor = float4(0.7f, 0.6f, 0.3f, 1.0f); //ゴールドクロス的にする場合
    outDiffuse = albedocolor;
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a; //α値に明るさは関係ない

    //ピクセルからカメラへ向かうベクトル(いつもと逆)
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);

    //ハーフベクトルを計算
    float3 halfv = eyev + lv.xyz; //視線ベクトル+ライトベクトル
    halfv = normalize(halfv);

    //スペキュラーの計算
    float3 specular;
    float nv = saturate(dot(normal.xyz, eyev));
    float nh = saturate(dot(normal.xyz, halfv));
    float vh = saturate(dot(eyev, halfv));
    float nl = saturate(dot(normal.xyz, lv.xyz));

    //今回はテクスチャからの情報でなく設定した数値で代用
    float smooth = saturate(Parameter.x);
    float metallic = saturate(Parameter.y);

    float D = CalculateBeckmann(smooth, nh); //面の粗さ
    float G = CalculateGeometricDamping(nh, nv, nl, vh);
    float F = CalculateFresnel(metallic, dot(lv.xyz, halfv));

    specular = max(0.0f, F * D * G / nv) * albedocolor.rgb;

    outDiffuse.rgb += (specular * ofs); // = のみにするとスペキュラだけになり分かりやすい
}