/*==============================================================================

[PolygonModel.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------
	シェーダーごとの設定は MaterialTable.cpp の MaterialDesc から読み込む。
==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include <cstdio>
#include "PolygonModel.h"
#include "texture.h"
#include "model.h"
#include "keyboard.h"

//==============================================================================
//コンストラクタ
//==============================================================================
PolygonModel::PolygonModel(const char* materialName, XMFLOAT3 position)
	: m_MaterialName(materialName)
	, m_InitialPosition(position)
{
}

//==============================================================================
//初期化処理
//==============================================================================
void PolygonModel::Init(void)
{
	m_Is2D = false;		//2DオブジェクトフラグOFF

	//構造体初期化
	m_Position = m_InitialPosition;
	m_Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//サンプラーの設定
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GetDevice()->CreateSamplerState(&sampDesc, &m_SamplerState);

	//マテリアル名 → 表の添字
	int index = FindMaterialIndex(m_MaterialName.c_str());
	if (index < 0)
	{
		MessageBoxA(NULL, m_MaterialName.c_str(), "マテリアルが見つかりません（MaterialTable.cpp を確認）", MB_ICONEXCLAMATION | MB_OK);
		index = 0;
	}

	//シェーダー・テクスチャ・モデル・パラメータ・ライトを読み込む
	ChangeMaterial(index, true);
}

//==============================================================================
//マテリアル切り替え
//==============================================================================
void PolygonModel::ChangeMaterial(int materialIndex, bool resetParams)
{
	const std::vector<MaterialDesc>& table = GetMaterialTable();
	if (materialIndex < 0 || materialIndex >= (int)table.size()) return;

	m_MaterialIndex = materialIndex;
	const MaterialDesc& desc = table[materialIndex];

	//シェーダー（キャッシュ経由）・追加テクスチャ
	m_Material.Load(&desc);

	//基本テクスチャ（同名は再利用される）
	m_TexID = TextureLoad(desc.Texture);

	//モデル（違うときだけ読み直す）
	if (m_Model == nullptr || m_LoadedModelPath != desc.Model)
	{
		if (m_Model) { ModelRelease(m_Model); m_Model = nullptr; }
		m_Model = ModelLoad(desc.Model);
		m_LoadedModelPath = desc.Model;
	}

	if (resetParams)
	{
		m_Parameter = desc.Parameter;
		m_Light     = desc.Light;
	}
}

//==============================================================================
//終了処理
//==============================================================================
void PolygonModel::Uninit(void)
{
	// シェーダーはシェーダーキャッシュが所有しているので、ここでは解放しない
	SafeRelease(m_SamplerState);
	if (m_Model) { ModelRelease(m_Model); m_Model = nullptr; }
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

	PolygonModel::DrawImGui();
}

//==============================================================================
// 描画処理
//==============================================================================
void PolygonModel::Draw(void)
{
	const MaterialDesc& desc = GetDesc();

	// スポットライト等は Game.cpp の共通ライトを使う
	if (desc.UseGlobalLight)
	{
		m_Light = g_Light;
	}

	// パラメータ・追加テクスチャ・頂点レイアウト・シェーダー設定
	SetParameter(m_Parameter);
	m_Material.Bind();

	if (desc.HasOutline())
	{
		// ① 通常描画（裏面カリング）
		SetCullMode(D3D11_CULL_BACK);
		DrawModel();

		// ② アウトライン描画（表面カリング＋アウトライン用シェーダー）
		SetCullMode(D3D11_CULL_FRONT);
		m_Material.BindOutline();
		DrawModel();

		// ③ 裏面カリングに戻す
		SetCullMode(D3D11_CULL_BACK);
	}
	else
	{
		DrawModel();
	}
}

void PolygonModel::DrawImGui()
{
	ImGui::Begin("Inspecter");
	ImGui::PushID(this);

	// "###" 以降をIDにして、マテリアルを切り替えても開閉状態を保つ
	char header[128];
	snprintf(header, sizeof(header), "%s###PolygonModel", GetName());

	if (ImGui::CollapsingHeader(header))
	{
		// マテリアル（シェーダー）切り替え
		ImGui::SeparatorText("Material");
		int selected = MaterialCombo(m_MaterialIndex);
		if (selected >= 0)
		{
			ChangeMaterial(selected, true);
		}

		ImGui::SeparatorText("Transform");
		DragFloat3Row("Position", "Position", m_Position);
		DragFloat3Row("Rotation", "Rotation", m_Rotation);
		DragFloat3Row("Scale",    "Scale",    m_Scale);

		// ライト・マテリアル固有のパラメータ
		const MaterialDesc& desc = GetDesc();
		DrawMaterialSettings(desc, m_Light, m_Parameter);

		ImGui::SeparatorText("Resources");
		ImGui::Text("VertexShader: %s", GetVertexShaderPath());
		ImGui::Text("PixelShader : %s", GetPixelShaderPath());
		if (desc.HasOutline())
		{
			ImGui::Text("OutlineVS   : %s", desc.OutlineVertexShader);
			ImGui::Text("OutlinePS   : %s", desc.OutlinePixelShader);
		}
		ImGui::Text("Texture     : %ls", GetTexturePath());
		for (const TextureSlot& slot : desc.ExtraTextures)
		{
			ImGui::Text("Texture(t%u) : %ls", slot.Slot, slot.Path);
		}
		ImGui::Text("Model       : %s", GetModelPath());

		DrawImGuiExtra();
	}

	ImGui::PopID();
	ImGui::End();
}


void PolygonModel::DrawModel(void)
{
	SetLight(m_Light);

	//テクスチャをセット
	ID3D11ShaderResourceView* tex = GetTexture(m_TexID);
	GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	//ワールド行列作成
	SetWorldMatrix(GetWorldMatrix());

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	MATERIAL material;
	ZeroMemory(&material, sizeof(MATERIAL));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	ModelDraw(m_Model);
}
