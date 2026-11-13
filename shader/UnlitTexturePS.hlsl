#include "Common.hlsl"
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET, out float4 outDoF : SV_Target1)
{
    outDiffuse = In.Diffuse;
    outDiffuse *= g_Texture.SampleLevel(g_SamplerState, In.TexCoord, Parameter.x);

    outDoF = outDiffuse; 
}