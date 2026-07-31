/*==============================================================================

[Toon3.cpp]
								トゥーンシェーディング3（テクスチャ版）
								ベース :Toon.cpp
--------------------------------------------------------------------------------
	明るさをテクスチャ座標として、ランプテクスチャ（明るさのLUT）から色を取得し
	それを「明るさ」として乗算する。テクスチャを差し替えるだけで段階数や色味を
	自由に変えられるのが利点。
==============================================================================*/
//==============================================================================
//インクルード
//==============================================================================
#include "Toon3.h"
#include "texture.h"

//==============================================================================
// 初期化処理
//==============================================================================
void Toon3::Init(void)
{
	PolygonModel::Init();

	// ★エッジ用シェーダーを追加で読み込む
	CreateVertexShader(&m_EdgeVS, &m_EdgeLayout, "ToonVSEdge.cso");
	CreatePixelShader(&m_EdgePS, "ToonPSEdge.cso");

	m_Position = XMFLOAT3(1.0f, 0.5f, 0.0f);
	m_Parameter = XMFLOAT4(0.5f, 0.02f, 0.0f, 0.0f);	// x:ランプV  y:エッジ幅
	m_TexIDRamp = TextureLoad(GetRampTexturePath());

	// ライト構造体の初期化
	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	XMStoreFloat4(&m_Light.Direction, dir);
	m_Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_Light.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.PointLightParam = XMFLOAT4(3.0f, 0.0f, 0.0f, 1.0f);
}

//==============================================================================
// 終了処理
//==============================================================================
void Toon3::Uninit(void)
{
	if (m_EdgeLayout) { m_EdgeLayout->Release(); m_EdgeLayout = nullptr; }
	if (m_EdgeVS) { m_EdgeVS->Release();     m_EdgeVS = nullptr; }
	if (m_EdgePS) { m_EdgePS->Release();     m_EdgePS = nullptr; }

	PolygonModel::Uninit();
}

//==============================================================================
// 更新処理
//==============================================================================
void Toon3::Update(void)
{


	PolygonModel::Update();
}

//==============================================================================
// 描画処理
//==============================================================================
void Toon3::Draw(void)
{
	SetParameter(m_Parameter);	
	ID3D11ShaderResourceView* tex = GetTexture(m_TexIDRamp);
	GetDeviceContext()->PSSetShaderResources(1, 1, &tex);

	// ① 通常描画（裏面カリング）
	SetCullMode(D3D11_CULL_BACK);
	PolygonModel::Draw();

	// ② エッジ描画（表面カリング＋エッジ用シェーダーに切り替え）
	SetCullMode(D3D11_CULL_FRONT);
	GetDeviceContext()->VSSetShader(m_EdgeVS, nullptr, 0);
	GetDeviceContext()->PSSetShader(m_EdgePS, nullptr, 0);
	GetDeviceContext()->IASetInputLayout(m_EdgeLayout);
	DrawModel();
	//PolygonModel::Draw();   // ※シェーダーを再セットしない描画関数が必要

	// ③ 裏面カリングに戻す
	SetCullMode(D3D11_CULL_BACK);
}


//==============================================================================
// ImGui描画
//==============================================================================
void Toon3::DrawImGuiExtra()
{
	ImGui::SeparatorText("Toon3");
	ImGui::DragFloat("Texture V", &m_Parameter.x, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("EdgeModel Scale", &m_Parameter.y, 0.001f, 0.0f, 0.5f);
}
