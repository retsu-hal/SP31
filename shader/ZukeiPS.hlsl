
#include "Common.hlsl"
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// 円の半径
float Circle(float2 tex,float radius)
{
    return length(tex) - radius; //円の半径を計算

}

// 円の半径と幅
float ring(float2 tex, float radius, float width)
{
    width = radius - width;
    return abs(length(tex) - radius) + width; //円の半径を計算
}

//ハート型
float heart(float2 tex, float size)
{
    float kubomi = Parameter.z;
    tex.x = 2.0f * tex.x - sign(tex.x) * (-tex.y) * kubomi;
    
    return length(tex) - size; 
}

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float ASPECT = (1280.0f/720.0f);
    /*
    //円の塗りつぶし
    {
        float loop = 1.0f;  //ループ回数(個数) を指定
        float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
        tex.x *= ASPECT;
        
        float radius = Parameter.x;
        float color = Circle(tex, radius);
        
        color = smoothstep(radius, radius + Parameter.y, color);
        
        outDiffuse.rgb = 1.0f - color; //円の内側を白、外側を黒にする
        outDiffuse.a = 1.0f; //アルファ値を1に設定
    }
    */
    
    /*
    //ライン描画の円
    {
        float loop = 1.0f; //ループ回数(個数) を指定
        float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
        tex.x *= ASPECT;
        
        float radius = Parameter.x;
        float width = Parameter.z;
        
        float color = ring(tex, radius, width);
        
        color = smoothstep(radius, radius + Parameter.y, color);
        
        outDiffuse.rgb = 1.0f - color; //円の内側を白、外側を黒にする
        outDiffuse.a = 1.0f; //アルファ値を1に設定
    }
*/
    
    //ハートの描画
    {
        float loop = 1.0f; //ループ回数(個数) を指定
        float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
        //tex.x *= ASPECT;
        
        float size = Parameter.x;
        
        float color = heart(tex, size);
        
        color = smoothstep(size, size + Parameter.y, color);
        
        outDiffuse.rgb = 1.0f - color; //円の内側を白、外側を黒にする
        outDiffuse.a = 1.0f; //アルファ値を1に設定
    }
}
