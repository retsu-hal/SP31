#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float spot = 0.0f;
    float light = 0.0f;
    float specular = 0.0f;

    //ライトからピクセルへのベクトル
    float3 Vp = normalize(In.WorldPosition.xyz - Light.Position.xyz);
    //ライトの向きベクトルとVpの角度
    float angle = acos(dot(Vp, normalize(Light.Direction.xyz)));

    //コーンの中だけ計算する
    if (angle <= Light.Angle.x)
    {
        //スポットライト明るさを補間する（中心1.0 → 端0.0、POWでイージング）
        spot = saturate(1.0f - pow(abs(angle / Light.Angle.x), Light.PointLightParam.y));

        //フォンで光源計算
        float4 normal = normalize(In.Normal);
        light = saturate(-dot(normal.xyz, Vp));

        float3 eyev = normalize(In.WorldPosition.xyz - CameraPosition.xyz);
        float3 refv = normalize(reflect(Vp, normal.xyz));
        specular = saturate(-dot(eyev, refv));
        specular = pow(specular, 30);

        //ライトからの距離による減衰
        float dist = length(In.WorldPosition.xyz - Light.Position.xyz);
        float ofs = 1.0f - dist / Light.PointLightParam.x;
        ofs = saturate(ofs);
        spot *= ofs;
    }

    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    outDiffuse.rgb += (specular * spot);
}