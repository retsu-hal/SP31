/*==============================================================================

[Material.cpp]
	MaterialDesc を元にシェーダー／テクスチャを読み込み、描画前にセットする。
	シェーダーはファイル名をキーにキャッシュして、同じものを重複して読み込まない。
==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include <string>
#include <unordered_map>
#include "Material.h"
#include "texture.h"

//==============================================================================
//シェーダーキャッシュ
//==============================================================================
namespace
{
	struct VertexShaderEntry
	{
		ID3D11VertexShader* VS     = nullptr;
		ID3D11InputLayout*  Layout = nullptr;
	};

	std::unordered_map<std::string, VertexShaderEntry>  g_VertexShaderCache;
	std::unordered_map<std::string, ID3D11PixelShader*> g_PixelShaderCache;
}

void GetCachedVertexShader(const char* fileName, ID3D11VertexShader** vs, ID3D11InputLayout** layout)
{
	auto it = g_VertexShaderCache.find(fileName);
	if (it == g_VertexShaderCache.end())
	{
		// 初回だけ読み込む（失敗して nullptr のままでも登録し、エラー表示を繰り返さない）
		VertexShaderEntry entry;
		CreateVertexShader(&entry.VS, &entry.Layout, fileName);
		it = g_VertexShaderCache.emplace(fileName, entry).first;
	}
	*vs     = it->second.VS;
	*layout = it->second.Layout;
}

void GetCachedPixelShader(const char* fileName, ID3D11PixelShader** ps)
{
	auto it = g_PixelShaderCache.find(fileName);
	if (it == g_PixelShaderCache.end())
	{
		ID3D11PixelShader* shader = nullptr;
		CreatePixelShader(&shader, fileName);
		it = g_PixelShaderCache.emplace(fileName, shader).first;
	}
	*ps = it->second;
}

void ReleaseShaderCache()
{
	for (auto& pair : g_VertexShaderCache)
	{
		if (pair.second.Layout) pair.second.Layout->Release();
		if (pair.second.VS)     pair.second.VS->Release();
	}
	for (auto& pair : g_PixelShaderCache)
	{
		if (pair.second) pair.second->Release();
	}
	g_VertexShaderCache.clear();
	g_PixelShaderCache.clear();
}

//==============================================================================
//Material
//==============================================================================
void Material::Load(const MaterialDesc* desc)
{
	m_Desc = desc;

	GetCachedVertexShader(desc->VertexShader, &m_VertexShader, &m_VertexLayout);
	GetCachedPixelShader(desc->PixelShader, &m_PixelShader);

	if (desc->HasOutline())
	{
		GetCachedVertexShader(desc->OutlineVertexShader, &m_OutlineVertexShader, &m_OutlineVertexLayout);
		GetCachedPixelShader(desc->OutlinePixelShader, &m_OutlinePixelShader);
	}
	else
	{
		m_OutlineVertexShader = nullptr;
		m_OutlinePixelShader  = nullptr;
		m_OutlineVertexLayout = nullptr;
	}

	// 追加テクスチャ（TextureLoad は同名を再利用するので何度呼んでもよい）
	m_ExtraTexIDs.clear();
	for (const TextureSlot& slot : desc->ExtraTextures)
	{
		m_ExtraTexIDs.push_back(TextureLoad(slot.Path));
	}
}

void Material::Bind() const
{
	if (!m_Desc) return;

	// 追加テクスチャを t1 以降へ（t0 は PolygonModel::DrawModel でセット）
	for (size_t i = 0; i < m_Desc->ExtraTextures.size(); i++)
	{
		ID3D11ShaderResourceView* tex = GetTexture(m_ExtraTexIDs[i]);
		GetDeviceContext()->PSSetShaderResources(m_Desc->ExtraTextures[i].Slot, 1, &tex);
	}

	GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
}

void Material::BindOutline() const
{
	GetDeviceContext()->IASetInputLayout(m_OutlineVertexLayout);
	GetDeviceContext()->VSSetShader(m_OutlineVertexShader, NULL, 0);
	GetDeviceContext()->PSSetShader(m_OutlinePixelShader, NULL, 0);
}
