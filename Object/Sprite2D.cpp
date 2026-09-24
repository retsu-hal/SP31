/*==============================================================================

   頂点管理 [Sprite2D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Sprite2D.h"
#include "Game.h"
#include "sprite.h"
#include "Camera.h"
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

//
//struct  TEST
//{
//	int		IntData;	//4バイト
//	char	CharData;	//1バイト
//	float	FloatData;	//4バイト
//};
//
//TEST	test;

Sprite2D::Sprite2D(const char* materialName, const wchar_t* texturePath)
	: m_MaterialName(materialName)
	, m_TexturePath(texturePath)
{}

//=============================================================================
// 初期化処理
//=============================================================================
void Sprite2D::Init(void)
{
	m_Is2D = true;		//2Dオブジェクトとして描画する

	//char	txt[256] = { 0 };
	//sprintf(txt, "&IntData   => %p \n", &test.IntData);
	//OutputDebugString(txt);
	//sprintf(txt, "&CharData  => %p \n", &test.CharData);
	//OutputDebugString(txt);
	//sprintf(txt, "&FloatData => %p \n", &test.FloatData);
	//OutputDebugString(txt);



	//2Dオブジェクト初期化
	m_Position = XMFLOAT3(SCREEN_WIDTH / 3 / 2, SCREEN_WIDTH / 3 / 2, 0.0f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Size = XMFLOAT2(SCREEN_WIDTH / 3, SCREEN_WIDTH / 3);

	//マテリアル名 → 表の添字
	int index = FindMaterialIndex(m_MaterialName.c_str());
	if (index < 0)
	{
		MessageBoxA(NULL, m_MaterialName.c_str(), "マテリアルが見つかりません（MaterialTable.cpp を確認）", MB_ICONEXCLAMATION | MB_OK);
		index = 0;
	}

	//シェーダー・テクスチャ・パラメータ・ライトを読み込む
	ChangeMaterial(index, true);
}

//=============================================================================
// マテリアル切り替え
//=============================================================================
void Sprite2D::ChangeMaterial(int materialIndex, bool resetParams)
{
	const std::vector<MaterialDesc>& table = GetMaterialTable();
	if (materialIndex < 0 || materialIndex >= (int)table.size()) return;

	m_MaterialIndex = materialIndex;
	const MaterialDesc& desc = table[materialIndex];

	//シェーダー（キャッシュ経由）・追加テクスチャ
	m_Material.Load(&desc);

	//テクスチャ（同名は再利用される）
	m_TexID = TextureLoad(GetTexturePath());

	if (resetParams)
	{
		m_Parameter = desc.Parameter;
		m_Light     = desc.Light;
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void Sprite2D::Uninit(void)
{
	// シェーダーはシェーダーキャッシュが所有しているので、ここでは解放しない
}

//=============================================================================
// 更新処理
//=============================================================================
void Sprite2D::Update(void)
{
	Sprite2D::DrawImGui();
}

//=============================================================================
// 描画処理
//=============================================================================
void Sprite2D::Draw(void)
{
	const MaterialDesc& desc = GetDesc();

	// スポットライト等は Game.cpp の共通ライトを使う
	if (desc.UseGlobalLight)
	{
		m_Light = g_Light;
	}

	// パラメータ・追加テクスチャ・頂点レイアウト・シェーダー設定
	// （スプライトなのでアウトライン用の2パス目は描かない）
	SetParameter(m_Parameter);
	m_Material.Bind();
	SetLight(m_Light);

	//奥行き処理をOFF
	SetDepthEnable(false);

	{//2Dポリゴン1枚ずつで必要な処理

		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(m_TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//ワールド行列をDirectXへセット（拡大縮小 → Z回転 → 平行移動）
		SetWorldMatrix(GetWorldMatrix());

		// ポリゴン描画（マテリアル設定も DrawSprite 内で行う）
		DrawSprite(Size, Color);
	}


}

//=============================================================================
// ImGui
//=============================================================================
void Sprite2D::DrawImGui()
{
	ImGui::Begin("Inspecter");
	ImGui::PushID(this);

	// "###" 以降をIDにして、マテリアルを切り替えても開閉状態を保つ
	char header[128];
	snprintf(header, sizeof(header), "Sprite2D : %s###Sprite2D", GetName());

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
		ImGui::DragFloat2("Position", &m_Position.x, 1.0f);
		ImGui::DragFloat("Rotation", &m_Rotation.z, 0.5f);
		ImGui::DragFloat2("Scale", &m_Scale.x, 0.01f);
		ImGui::DragFloat2("Size", &Size.x, 1.0f);
		ImGui::ColorEdit4("Color", &Color.x);

		// ライト・マテリアル固有のパラメータ
		const MaterialDesc& desc = GetDesc();
		DrawMaterialSettings(desc, m_Light, m_Parameter);

		ImGui::SeparatorText("Resources");
		ImGui::Text("VertexShader: %s", GetVertexShaderPath());
		ImGui::Text("PixelShader : %s", GetPixelShaderPath());
		ImGui::Text("Texture     : %ls", GetTexturePath());
		for (const TextureSlot& slot : desc.ExtraTextures)
		{
			ImGui::Text("Texture(t%u) : %ls", slot.Slot, slot.Path);
		}
	}

	ImGui::PopID();
	ImGui::End();
}
