/*==============================================================================

   レンダリング管理[renderer.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include "main.h"

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position;	//頂点座標　XMFLOAT3 ＝　float x,y,z
	XMFLOAT3 Normal;	//法線ベクトル 
	XMFLOAT4 Diffuse;	//色  XMFLOAT4 = float x,y,z,w
	XMFLOAT2 TexCoord;	//テクスチャ座標 XMFLOAT2 = float x,y
};

// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;	//アンビエント
	XMFLOAT4	Diffuse;	//デフューズ
	XMFLOAT4	Specular;	//スペキュラ
	XMFLOAT4	Emission;	//エミッシブ
	float		Shininess;	//スペキュラパラメータ
	float		Dummy[3];	//16byte境界調整用パディング
};

struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];//16byte境界用
	XMFLOAT4	Direction;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
	XMFLOAT4	Position;
	XMFLOAT4	PointLightParam;
	XMFLOAT4 Angle;
};

class Renderer
{
public:
	static HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	static void Finalize(void);

	static void Clear(void);
	static void Present(void);

	static ID3D11Device* GetDevice(void);
	static ID3D11DeviceContext* GetDeviceContext(void);

	static void SetDepthEnable(bool Enable);
	static void SetCullMode(D3D11_CULL_MODE CullMode);

	static void SetWorldViewProjection2D(void);
	static void ResetWorldViewProjection3D(void);

	static void SetWorldMatrix(XMMATRIX WorldMatrix);
	static void SetViewMatrix(XMMATRIX ViewMatrix);
	static void SetProjectionMatrix(XMMATRIX ProjectionMatrix);

	static void SetCameraPosition(XMFLOAT3 CameraPosition);

	static void SetParameter(XMFLOAT4 Parameter);

	static void SetMaterial(MATERIAL Material);

	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

	static void SetLight(LIGHT Light);

	// レンダーターゲットの作成
	static ID3D11ShaderResourceView* GetPeTexture(int TexID = 0);
	static void BeginPe(int TexID = 0);
};
