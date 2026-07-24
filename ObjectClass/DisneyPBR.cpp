/*==============================================================================

[DisneyPBR.cpp]
													ベース :CookTorrance.cpp
--------------------------------------------------------------------------------
	ディズニーPBR(簡略版)。Roughness / Metalness をテクスチャから
	ピクセルごとに取得し、Parameter.z を使用ライト数として扱う。
==============================================================================*/
//==============================================================================
//インクルード
//==============================================================================
#include "DisneyPBR.h"
#include "texture.h"
#include "imgui/imgui.h"

//==============================================================================
//初期化処理
//==============================================================================
void DisneyPBR::Init(void)
{
	PolygonModel::Init();

	m_Position = XMFLOAT3(2.0f, 0.5f, 0.0f);

	//パラメータ初期値
	m_Parameter = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_Parameter.x = 0.5f; //ざらつき(Roughness)
	m_Parameter.y = 0.5f; //金属感(Metallic)
	m_Parameter.z = 3.0f; //使用するライトの数（デフォルト3個）

	//追加テクスチャのロード（1番:粗さ / 2番:金属度）
	m_TexIDRoughness = TextureLoad(GetRoughnessTexturePath());
	m_TexIDMetalness = TextureLoad(GetMetalnessTexturePath());

	//ライトの色関連パラメータを調整（PDF p4 準拠）
	XMVECTOR dir = XMVector4Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	XMStoreFloat4(&m_Light.Direction, dir);
	m_Light.Position        = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	m_Light.Diffuse         = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	m_Light.Ambient         = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);	
	m_Light.PointLightParam = XMFLOAT4(2000.0f, 1.5f, 0.0f, 0.0f);	
}

//=============================================================================
// 終了処理
//=============================================================================
void DisneyPBR::Uninit(void)
{
	PolygonModel::Uninit();
}

//=============================================================================
// 更新処理（ライトの個数を変更可能にする）
//=============================================================================
void DisneyPBR::Update(void)
{
	PolygonModel::Update();
}

//=============================================================================
// 描画処理
//=============================================================================
void DisneyPBR::Draw(void)
{

	//追加テクスチャを 1番・2番へそれぞれ設定
	//（0番:アルベドは PolygonModel::Draw 内で設定される）
	ID3D11ShaderResourceView* tex = GetTexture(m_TexIDRoughness);
	GetDeviceContext()->PSSetShaderResources(1, 1, &tex);
	tex = GetTexture(m_TexIDMetalness);
	GetDeviceContext()->PSSetShaderResources(2, 1, &tex);

	PolygonModel::Draw();
}

void DisneyPBR::DrawImGuiExtra(void)
{
	ImGui::SliderFloat("Light ", &m_Parameter.z, 1.0f, 15.0f, "%.0f");
}
