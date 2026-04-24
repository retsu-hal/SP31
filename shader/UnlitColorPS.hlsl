
#include "Common.hlsl"

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
	outDiffuse = In.Diffuse;
}



