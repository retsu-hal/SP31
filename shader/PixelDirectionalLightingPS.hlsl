#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float4 normal = normalize(In.Normal);

    // ランバート反射
    float light = -dot(normal.xyz, Light.Direction.xyz);
    light = saturate(light);

    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a *= In.Diffuse.a;

    // フォン鏡面反射
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz; // カメラ→ピクセル
    eyev = normalize(eyev);

    float3 refv = reflect(Light.Direction.xyz, normal.xyz); // 反射ベクトル
    refv = normalize(refv);

    float specular = -dot(refv, eyev);
    specular = saturate(specular);
    specular = pow(specular, max(Parameter.x, 1.0)); // 初期値30

    outDiffuse.rgb += specular; // ランバートの結果に加算
}