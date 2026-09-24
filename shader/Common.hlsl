//==============================================================================
// Common.hlsl
//   全シェーダー共通の定数バッファ・入出力構造体。
//   並びとスロット番号は renderer.cpp / renderer.h と一致させること。
//==============================================================================

// b0 : ワールド行列
cbuffer WorldBuffer : register(b0)
{
    matrix World;
}

// b1 : ビュー行列
cbuffer ViewBuffer : register(b1)
{
    matrix View;
}

// b2 : プロジェクション行列
cbuffer ProjectionBuffer : register(b2)
{
    matrix Projection;
}

// b3 : マテリアル（renderer.h の MATERIAL）
struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float  Shininess;
    float3 Dummy;       // 16byte境界調整用
};

cbuffer MaterialBuffer : register(b3)
{
    MATERIAL Material;
}

// b4 : ライト（renderer.h の LIGHT）
struct LIGHT
{
    bool   Enable;
    bool3  Dummy;       // 16byte境界調整用
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
    float4 Position;
    float4 PointLightParam;
};

cbuffer LightBuffer : register(b4)
{
    LIGHT Light;
}

// b5 : カメラ位置
cbuffer CameraBuffer : register(b5)
{
    float4 CameraPosition;
}

// b6 : シェーダーごとの汎用パラメータ（SetParameter）
cbuffer ParameterBuffer : register(b6)
{
    float4 Parameter;
}

//------------------------------------------------------------------------------
// 頂点シェーダー入力（renderer.h の VERTEX_3D / renderer.cpp の入力レイアウト）
//------------------------------------------------------------------------------
struct VS_IN
{
    float4 Position : POSITION0;
    float4 Normal   : NORMAL0;
    float4 Diffuse  : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

//------------------------------------------------------------------------------
// ピクセルシェーダー入力
//------------------------------------------------------------------------------
struct PS_IN
{
    float4 Position      : SV_POSITION;
    float4 WorldPosition : POSITION0;
    float4 Normal        : NORMAL0;
    float4 Diffuse       : COLOR0;
    float2 TexCoord      : TEXCOORD0;
};