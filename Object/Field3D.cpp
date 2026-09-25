/*==============================================================================

    [Field3D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------
	シェーダーごとの設定は MaterialTable.cpp の MaterialDesc から読み込む。
==============================================================================*/
#include <cstdio>
#include "Field3D.h"
#include "Game.h"

#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	NUM_VERTEX	(4)
#define	SIZE	(5)

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//頂点データ
/*
      V0------------------V1
      |                  |
     |                  |
    |                  |
   V2-----------------V3
*/

static VERTEX_3D	Box[NUM_VERTEX] =
{
	{//頂点V0 LEFT-TOP
		XMFLOAT3(-SIZE/2, 0.0f, SIZE/2),	//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線（上向き：ライティング系シェーダー用）
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(0.0f*SIZE,0.0f*SIZE)				//テクスチャ座標
	},
	{//頂点V1 RIGHT-TOP
		XMFLOAT3(SIZE / 2, 0.0f, SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(1.0f* SIZE,0.0f* SIZE)				//テクスチャ座標
	},
	{//頂点V2 LEFT-BOTTOM
		XMFLOAT3(-SIZE / 2, 0.0f, -SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(0.0f* SIZE,1.0f* SIZE)				//テクスチャ座標
	},

	{//頂点V3 RIGHT-BOTTOM
		XMFLOAT3(SIZE / 2, 0.0f, -SIZE / 2),		//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(1.0f* SIZE,1.0f* SIZE)				//テクスチャ座標
	},

};



Field3D::Field3D(const char* materialName, XMFLOAT3 position)
	: m_MaterialName(materialName)
	, m_InitialPosition(position)
{}

//=============================================================================
// 初期化処理
//=============================================================================
void Field3D::Init(void)
{
	//頂点バッファ作成
	{
		D3D11_BUFFER_DESC	bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		Renderer::GetDevice()->CreateBuffer(&bd, NULL, &m_vertexBuffer);

		//頂点バッファの書き込み先のポインターを取得
		D3D11_MAPPED_SUBRESOURCE	msr;
		Renderer::GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//頂点データをコピー
		CopyMemory(&vertex[0], &Box[0], sizeof(VERTEX_3D) * NUM_VERTEX);
		//書き込み完了
		Renderer::GetDeviceContext()->Unmap(m_vertexBuffer, 0);
	}

	//3Dオブジェクト管理構造体の初期化
	m_Position = m_InitialPosition;
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

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
void Field3D::ChangeMaterial(int materialIndex, bool resetParams)
{
	const std::vector<MaterialDesc>& table = GetMaterialTable();
	if (materialIndex < 0 || materialIndex >= (int)table.size()) return;

	m_MaterialIndex = materialIndex;
	const MaterialDesc& desc = table[materialIndex];

	//シェーダー（キャッシュ経由）・追加テクスチャ
	m_Material.Load(&desc);

	//基本テクスチャ（同名は再利用される）
	m_TexID = Texture::Load(desc.Texture);

	if (resetParams)
	{
		m_Parameter = desc.Parameter;
		m_Light     = desc.Light;
	}
}

//=============================================================================
// 終了処理
//=============================================================================
void Field3D::Uninit(void)
{
	// シェーダーはシェーダーキャッシュが所有しているので、ここでは解放しない
	SafeRelease(m_vertexBuffer);
}

//=============================================================================
// 更新処理
//=============================================================================
void Field3D::Update(void)
{
	Field3D::DrawImGui();
}

//=============================================================================
// 描画処理
//=============================================================================
void Field3D::Draw(void)
{
	const MaterialDesc& desc = GetDesc();

	// スポットライト等は Game.cpp の共通ライトを使う
	if (desc.UseGlobalLight)
	{
		m_Light = g_Light;
	}

	// パラメータ・追加テクスチャ・頂点レイアウト・シェーダー設定
	// （板ポリゴンなのでアウトライン用の2パス目は描かない）
	Renderer::SetParameter(m_Parameter);
	m_Material.Bind();
	Renderer::SetLight(m_Light);

	{
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = Texture::GetTexture(m_TexID);
		Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//ワールド行列をDirectXへセット
		Renderer::SetWorldMatrix(GetWorldMatrix());

		//頂点バッファをセット
		UINT	stride = sizeof(VERTEX_3D);
		UINT	offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(
			0,
			1,
			&m_vertexBuffer,
			&stride,
			&offset
		);

		//プリミティブトポロジーの設定
		Renderer::GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
		//マテリアル設定
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		Renderer::SetMaterial(material);

		//描画
		Renderer::GetDeviceContext()->Draw(NUM_VERTEX, 0);//インデックス無し描画
	}


}

//=============================================================================
// ImGui
//=============================================================================
void Field3D::DrawImGui()
{
	ImGui::Begin("Inspecter");
	ImGui::PushID(this);

	// "###" 以降をIDにして、マテリアルを切り替えても開閉状態を保つ
	char header[128];
	snprintf(header, sizeof(header), "Field3D : %s###Field3D", GetName());

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
		ImGui::Text("Texture     : %ls", GetTexturePath());
		for (const TextureSlot& slot : desc.ExtraTextures)
		{
			ImGui::Text("Texture(t%u) : %ls", slot.Slot, slot.Path);
		}
	}

	ImGui::PopID();
	ImGui::End();
}
