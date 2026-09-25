#include "Common.hlsl"

// Sprite2D の Size は (1280/3) × (720/2) なので、その縦横比で補正する
static const float ASPECT = (1280.0f / 3.0f) / (720.0f / 2.0f);

// どの関数も「境界までの距離」を返す（内側はマイナス、外側はプラス）
float Circle(float2 p, float radius)
{
    return length(p) - radius;
}

float Ring(float2 p, float radius, float width)
{
    return abs(length(p) - radius) - width; // 円周からの距離を abs で折り返す
}

float Diamond(float2 p, float size)
{
    return abs(p.x) + abs(p.y) - size; // 縦と横の距離の合計で測る
}

float Heart(float2 p, float size, float kubomi)
{
    p.x = 2.0f * p.x - sign(p.x) * (-p.y) * kubomi; // 上は広げて、下はすぼめる
    return length(p) - size;
}

float Flower(float2 p, float size, float petal)      // 極座標の例
{
    float angle = atan2(p.y, p.x);
    float r = size * abs(cos(angle * petal)); // 角度ごとの境界の距離
    return length(p) - r;
}

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float2 p = In.TexCoord * 2.0f - 1.0f; // 中心を原点に（-1〜1）

    int shape = (int) Parameter.w;
    float size = Parameter.x;
    float edge = max(Parameter.y, 0.0001f); // smoothstep の min と max が同じ値だと結果が不定になるので防ぐ
    float z = Parameter.z;

    if (shape != 3)
        p.x *= ASPECT; // ハートは 2.0f * x で幅を調整しているので補正しない

    float d;
    switch (shape)
    {
        case 0:
            d = Circle(p, size);
            break;
        case 1:
            d = Ring(p, size, z);
            break;
        case 2:
            d = Diamond(p, size);
            break;
        case 3:
            d = Heart(p, size, z);
            break;
        default:
            d = Flower(p, size, z);
            break;
    }

    float color = smoothstep(0.0f, edge, d); // 境界(0)より外側なら 1
    outDiffuse.rgb = 1.0f - color; // 内側を白、外側を黒にする
    outDiffuse.a = 1.0f;
}