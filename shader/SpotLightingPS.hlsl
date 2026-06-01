#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float spot = 0.0f;
    float light = 0.0f;
    float specular = 0.0f;
    
    //ライトからピクセルへのベクトル
    float3 lv = normalize(In.WorldPosition.xyz-Light.Position.xyz);
    //コーンの向きベクトル
    float3 cv= normalize(Light.Direction.xyz);   
    //ライトの向きベクトルとVpの角度
    float angle = acos(dot(lv, cv));
    angle = abs(angle);
    //コーン範囲のチェック
    
    //基本的なスポットライトの計算
    /*if (angle<=Light.Angle.x)
    {
        spot = 1.0f;
        light = 1.0f;
        specular = 0.0f;
    }
    else
    {
        spot = 0.0f;
        light = 0.0f;
        specular = 0.0f;
    }**/

    //工夫版
    if (angle <= Light.Angle.x)
    {
        spot = saturate(1.0f - pow(1.0f / Light.Angle.x * abs(angle), Light.PointLightParam.y));
        float4 normal = normalize(In.Normal);//法線正規化
        light = saturate(-dot(normal.xyz, lv));
        float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
        eyev = normalize(eyev);
        float3 refv = reflect(lv, normal.xyz);
        refv = normalize(refv);
        specular = saturate(-dot(eyev, refv));
        specular = pow(specular,30);
        
        float dist = length(In.WorldPosition.xyz - Light.Position.xyz);
        float ofs = saturate(1.0 - dist / Light.PointLightParam.x);
        spot *= ofs;
    }
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.a = In.Diffuse.a;
    outDiffuse.rgb += (specular * spot);

}
