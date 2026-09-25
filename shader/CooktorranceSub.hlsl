// D：マイクロファセット分布（Beckmann）
//   m：粗さ（大きいほどハイライトが広がる）
float CalculateBeckmann(float m, float nh)
{
    if (nh <= 0.0f)
        return 0.0f; // ゼロ除算対策
    if (m <= 0.0f)
        return 0.0f; // ゼロ除算対策

    float m2 = m * m;
    float nh2 = nh * nh;
    float nh4 = nh2 * nh2;

    float ex = -(1.0f - nh2) / (m2 * nh2); // 指数部分
    float bun = 3.14159265f * m2 * nh4; // 分母

    return exp(ex) / bun;
}

// F：フレネル項（Schlick近似）
float CalculateFresnel(float metal, float vh)
{
    float f0 = lerp(0.04f, 1.0f, metal); // 非金属0.04 〜 金属1.0
    return f0 + (1.0f - f0) * pow(1.0f - vh, 5.0f);
}

// G：幾何減衰
float CalculateGeometricDamping(float nh, float nv, float nl, float vh)
{
    vh = max(vh, 0.0001f); // ゼロ除算対策
    return min(1.0f, min(2.0f * nh * nv / vh, 2.0f * nh * nl / vh));
}