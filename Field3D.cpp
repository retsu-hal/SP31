#include "Field3D.h"
#include "sprite.h"
#include "texture.h"

//=========================================================================================================
// マクロ定義
//=========================================================================================================
#define NUM_VERTEX (4)
#define SIZE (6)

//=========================================================================================================
// グローバル変数
//=========================================================================================================
static VERTEX_3D Box[NUM_VERTEX] =
{
	{
		XMFLOAT3(-SIZE / 2,0.0f,SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(0.0f * SIZE,0.0f * SIZE)
	},

	{
		XMFLOAT3(SIZE / 2,0.0f,SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(1.0f * SIZE,0.0f * SIZE)
	},

	{
		XMFLOAT3(-SIZE / 2,0.0f,-SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(0.0f * SIZE,1.0f * SIZE)
	},

	{
		XMFLOAT3(SIZE / 2,0.0f,-SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(1.0f * SIZE,1.0f * SIZE)
	},
};

//=========================================================================================================
// 初期化
//=========================================================================================================
void Field3D::Init()
{
	m_Is2D = false;		//2DオブジェクトフラグOFF

	// テクスチャ読み込み
	m_TexID = TextureLoad(L"asset\\texture\\sura.jpg");
	
	//シェーダー読み込み
	CreateVertexShader(&m_VertexShader, &m_VertexLayout, "PixelDirectionalLightingVS.cso");
	CreatePixelShader(&m_PixelShader, "PixelDirectionalLightingPS.cso");

	// 頂点バッファの作成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		GetDevice()->CreateBuffer(&bd, NULL, &m_VertexBuffer);
	}
	
	//頂点バッファの書き込み先のポインターを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	//頂点データをコピー
	CopyMemory(&vertex[0], &Box[0], sizeof(VERTEX_3D) * NUM_VERTEX);
	//
	GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	//構造体初期化
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

}


//=========================================================================================================
// 終了処理
//=========================================================================================================
void Field3D::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
	m_VertexBuffer->Release();
};

//=========================================================================================================
// 更新処理
//=========================================================================================================
void Field3D::Update(void)
{
	
};

//=========================================================================================================
// 描画処理
//=========================================================================================================
void Field3D::Draw(void)
{
	//
	GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//
	GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	//
	GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	{
		//
		ID3D11ShaderResourceView* tex = GetTexture(m_TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//
		XMMATRIX TranslationMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		//
		XMMATRIX RotationXMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		//	
		XMMATRIX ScalingMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		// ワールド行列の計算
		XMMATRIX WorldMatrix = ScalingMatrix * RotationXMatrix * TranslationMatrix;
		//
		SetWorldMatrix(WorldMatrix);

		//
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
		//
		GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//
		MATERIAL material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		// 描画
		GetDeviceContext()->Draw(NUM_VERTEX, 0);

	}
};