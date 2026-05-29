
#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 OutDiffuse : SV_TARGET)
{

    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, Light.Direction.xyz); //ライトの方向と法線の内積
    light = saturate(light); //lightの値を0～1に収める
 
    
    OutDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    //テクセルに明るさを乗算
    OutDiffuse.rgb *= In.Diffuse.rgb *Light.Diffuse.rgb* light+Light.Ambient.rgb; 
    OutDiffuse.a = In.Diffuse.a; //頂点色のアルファを出力
    
    //カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); //正規化
    
    //ハーフベクトル
    float3 halfv = eyev + Light.Direction.xyz;
    halfv = normalize(halfv); //正規化
    
    float specular = -dot(halfv, normal.xyz); //鏡面反射の計算
    specular = saturate(specular);
    specular = pow(specular, 30); //スペキュラの強さを調整
    
    OutDiffuse.rgb += specular ; //スペキュラを加算

}