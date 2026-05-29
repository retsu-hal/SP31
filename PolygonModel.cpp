/*==============================================================================

[PolygonModel.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "PolygonModel.h"
#include "texture.h"
#include "model.h"
#include "keyboard.h"
//==============================================================================
//マクロ宣言
//==============================================================================

//==============================================================================
//プロトタイプ宣言
//==============================================================================

//==============================================================================
//グローバル変数
//==============================================================================

//==============================================================================
//初期化処理
//==============================================================================
void PolygonModel::Init(void)
{
	m_Is2D = false;		//2DオブジェクトフラグOFF

	//シェーダー読み込み
	CreateVertexShader(&m_VertexShader, &m_VertexLayout, GetVertexShaderPath());
	CreatePixelShader(&m_PixelShader, GetPixelShaderPath());

	//構造体初期化
	m_Position = XMFLOAT3(0.0f + 0.5f * -1, 0.5f, 0.0f);
	m_Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// テクスチャ読み込み
	m_TexID = TextureLoad(GetTexturePath());

	//モデルの読み込み
	m_Model = ModelLoad(GetModelPath());

	//サンプラーの設定
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GetDevice()->CreateSamplerState(&sampDesc, &m_SamplerState);


	//ライト構造体の初期化
	XMVECTOR	dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	XMStoreFloat4(&m_Light.Direction, dir);//光のベクトル

	m_Light.Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);//点光源の位置
	m_Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//光の色
	m_Light.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);//環境光
	m_Light.PointLightParam = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);//環境光
};
//==============================================================================
//終了処理
//===================m_===========================================================
void PolygonModel::Uninit(void)
{
	if (m_VertexLayout) { m_VertexLayout->Release(); m_VertexLayout = nullptr; }
	if (m_VertexShader) { m_VertexShader->Release(); m_VertexShader = nullptr; }
	if (m_PixelShader) { m_PixelShader->Release(); m_PixelShader = nullptr; }
	if (m_SamplerState) { m_SamplerState->Release(); m_SamplerState = nullptr; }
	if (m_Model) { ModelRelease(m_Model); }
}

//==============================================================================
// 更新処理
//==============================================================================
void PolygonModel::Update(void)
{
	float Rotspeed = 0.5f;
	float Movespeed = 0.01f;
	//回転
	if (Keyboard_IsKeyDown(KK_E))		m_Rotation.x += Rotspeed;
	if(Keyboard_IsKeyDown(KK_Q))		m_Rotation.x -= Rotspeed;
	
	//移動
	if(Keyboard_IsKeyDown(KK_W))  	 m_Position.z+= Movespeed;
	if(Keyboard_IsKeyDown(KK_S))		m_Position.z -= Movespeed;
	if(Keyboard_IsKeyDown(KK_A))		m_Position.x -= Movespeed;
	if(Keyboard_IsKeyDown(KK_D))		m_Position.x += Movespeed;

}

//==============================================================================
//==============================================================================
void PolygonModel::Draw(void)
{
	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	SetLight(m_Light);


	{//3Dポリゴン１つずつの処理
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(m_TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列作成
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		//回転行列作成
		XMMATRIX	RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
		//スケーリング行列作成
		XMMATRIX	ScalingMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		//ワールド行列作成 ※乗算の順番に注意
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
		//DirectXへセット
		SetWorldMatrix(WorldMatrix);

		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//マテリアル設定
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//描画
		ModelDraw(m_Model);
	}
}