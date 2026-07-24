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

	//2Dオブジェクト初期化
	m_Position = XMFLOAT3(SCREEN_WIDTH /2, SCREEN_HEIGHT /2 , 0.0f);
	m_Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_Size = XMFLOAT2(SCREEN_WIDTH , SCREEN_HEIGHT );
	m_Rotate = 0.0f;
	m_Parameter = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);	//X:サイズ, Y:ぼかし, Z:リングの幅, W:未使用

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
	SafeRelease(m_VertexLayout);
	SafeRelease(m_VertexShader);
	SafeRelease(m_PixelShader);
	SafeRelease(m_SamplerState);
}

//=============================================================================
// 更新処理
//=============================================================================
void Sprite2D::Update(void)
{


	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
	ImGui::Begin(GetName());
	{
		ImGui::SliderFloat("Parameter.x", &m_Parameter.x, 0.0f, 1.0f, "%.4f");
		ImGui::SliderFloat("Parameter.y", &m_Parameter.y, 0.0f, 1.0f, "%.4f");
		ImGui::SliderFloat("Parameter.z", &m_Parameter.z, 0.0f, 1.0f, "%.4f");
		ImGui::SliderFloat("Parameter.w", &m_Parameter.w, 0.0f, 1.0f, "%.4f");
	}
	ImGui::End();

}

//=============================================================================
// 描画処理
//=============================================================================
void Sprite2D::Draw(void)
{
	SetParameter(m_Parameter);
	
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
		ID3D11ShaderResourceView* tex = GetPeTexture(m_TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		GetDeviceContext()->PSSetSamplers(0, 1, &m_SamplerState);

		//平行移動行列の作成（表示座標を決める）
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, 0.0f);

		//回転行列（Z回転）行列の作成
		XMMATRIX	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(m_Rotate));

		//スケーリング行列作成（倍率1.0が等倍、0倍はダメ！）
		XMMATRIX	ScalingMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, 1.0f);

		//ワールド行列の作成（ポリゴンの表示の仕方を指定する最終的な行列
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//ワールド行列をDirectXへセット
		SetWorldMatrix(WorldMatrix);

		//GetDeviceContext()->GenerateMips(tex);
	
		// ポリゴン描画
		DrawSprite(m_Size, m_Color);
	}


}
