
#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    Out.WorldPosition = mul(In.Position, World);
    
    Out.Normal = In.Normal;
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
   
}