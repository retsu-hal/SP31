/*==============================================================================

   頂点管理 [Sprite2D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Sprite2D.h"
#include "sprite.h"
#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// 初期化処理
//=============================================================================
void Sprite2D::Init(void)
{
	m_Is2D = true;		//2DオブジェクトフラグON
	// テクスチャ読み込み
	m_TexID = TextureLoad(GetTexturePath());
	
	//シェーダー読み込み
	CreateVertexShader(&m_VertexShader, &m_VertexLayout,GetVertexShaderPath());
	CreatePixelShader(&m_PixelShader, GetPixelShaderPath());

	//シェーダー読み込み(グレースケール)
	/*
	CreateVertexShader(&VertexShader, &VertexLayout, "GrayScaleVS.cso");
	CreatePixelShader(&PixelShader, "GrayScalePS.cso");
	*/

	//シェーダー読み込み(セピア調変換）
	/*
	CreateVertexShader(&VertexShader, &VertexLayout, "SepiaVS.cso");
	CreatePixelShader(&PixelShader, "SepiaPS.cso");
	*/
	//2Dオブジェクト初期化
	m_Position = XMFLOAT3(SCREEN_WIDTH / 3/2, SCREEN_HEIGHT / 3/2 , 0.0f);
	m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	Size = XMFLOAT2(SCREEN_WIDTH /3, SCREEN_HEIGHT /3);
	Rotate = 0.0f;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GetDevice()->CreateSamplerState(&sampDesc, &m_SamplerState);
}

//=============================================================================
// 終了処理
//=============================================================================
void Sprite2D::Uninit(void)
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
	m_SamplerState->Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void Sprite2D::Update(void)
{
	

}

//=============================================================================
// 描画処理
//=============================================================================
void Sprite2D::Draw(void)
{

	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//奥行き処理をOFF
	SetDepthEnable(false);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{//2Dポリゴン1枚ずつで必要な処理

		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetPeTexture();
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		GetDeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);

		//平行移動行列の作成（表示座標を決める）
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(
			m_Position.x, m_Position.y, 0.0f);

		//回転行列（Z回転）行列の作成
		XMMATRIX	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(Rotate));

		//スケーリング行列作成（倍率1.0が等倍、0倍はダメ！）
		XMMATRIX	ScalingMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, 1.0f);

		//ワールド行列の作成（ポリゴンの表示の仕方を指定する最終的な行列
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//ワールド行列をDirectXへセット
		SetWorldMatrix(WorldMatrix);

	
		// ポリゴン描画
		DrawSprite(Size, m_Color);
	}


}
