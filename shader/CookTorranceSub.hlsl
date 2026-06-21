//=====================================================
// CookTorranceSub.hlsl
//   クックトランスモデルによる鏡面反射のサブ関数群
//   ・common.hlsl と同様、プロパティで「ビルドから除外＝はい」に設定
//   ・ピクセルシェーダーのファイルからインクルードして使用する
//
//   スペキュラ計算式： I = kct * (D * G * F) / (N・V)
//     D : マイクロファセット分布（Beckmann）
//     F : フレネル反射項（Schlick近似）
//     G : 幾何減衰
//=====================================================


//-----------------------------------------------------
// D：マイクロファセット分布（Beckmann分布関数）
//   smooth … 面の素材による粗さを表すパラメータ（数式の m）
//   nh     … N・H（法線とハーフベクトルの内積）
//-----------------------------------------------------
float CalculateBeckmann(float smooth, float nh)
{
    if (nh <= 0.0f) return 0.0f; // ゼロディバイド対策
    if (smooth <= 0.0f) return 0.0f; // ゼロディバイド対策

    float smo2 = smooth * smooth;
    float nh2 = nh * nh;
    float nh4 = nh2 * nh2;

    // 指数部分の計算
    float ex = -(1.0f - nh2) / (smo2 * nh2);

    // 分母の計算
    float PI = 3.14159265f;
    float bun = PI * smo2 * nh4;

    return exp(ex) / bun;
}


//-----------------------------------------------------
// F：フレネル項（Schlick近似）
//   S(u) = F0 + (1 - F0) * (1 - L・H)^5
//   metal … メタリック値（0:非金属 ～ 1:金属）
//   hv    … H・V（= L・H。ハーフベクトルとの内積）
//-----------------------------------------------------
float CalculateFresnel(float metal, float hv)
{
    // 非金属の基本反射率 0.04 ～ 金属 1.0 へ補間して F0 とする
    metal = lerp(0.04f, 1.0f, metal);
    return metal + (1.0f - metal) * pow(1.0f - hv, 5.0f);
}


//-----------------------------------------------------
// G：幾何減衰率
//   3通りの値のうち一番小さい物を採用する
//   nh … N・H, nv … N・V, nl … N・L, vh … V・H
//-----------------------------------------------------
float CalculateGeometricDamping(float nh, float nv, float nl, float vh)
{
    float G;
    vh = max(vh, 0.0001f); // ゼロディバイド対策
    G = min(1.0f, min(2.0f * nh * nv / vh, 2.0f * nh * nl / vh));

    return G;
}


//-----------------------------------------------------
// クックトランスモデルによるスペキュラ強度
//   I = kct * (D * G * F) / (N・V)
//   N      … ピクセルの法線
//   L      … 光のベクトル（ライトへ向かう方向）
//   V      … 視線ベクトル（カメラへ向かう方向）
//   smooth … 粗さ（m）  metal … メタリック値
//   ※戻り値はスカラー強度。kct（デフューズ色で代用）は
//     呼び出し側で乗算する。スペキュラ値は 0.0 以上とする。
//-----------------------------------------------------
float CookTorranceSpecular(float3 N, float3 L, float3 V, float smooth, float metal)
{
    float3 H = normalize(L + V); // ハーフベクトル

    float nh = saturate(dot(N, H));
    float nv = saturate(dot(N, V));
    float nl = saturate(dot(N, L));
    float vh = saturate(dot(V, H)); // = L・H

    // D・F・G をサブ関数で計算
    float D = CalculateBeckmann(smooth, nh); // 分布
    float F = CalculateFresnel(metal, vh); // フレネル
    float G = CalculateGeometricDamping(nh, nv, nl, vh); // 幾何減衰

    // I = (D * G * F) / (N・V)
    float spec = (D * G * F) / max(nv, 0.0001f); // ゼロディバイド対策

    return max(spec, 0.0f); // スペキュラ値は 0.0 以上
}
