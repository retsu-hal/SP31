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
#include "GameObject.h"		// DragFloat3Row
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
		CreateVertexShader(&entry.VS, &entry.Layout, (std::string("shader/") + fileName).c_str());	// .cso は shader フォルダにある
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
		CreatePixelShader(&shader, (std::string("shader/") + fileName).c_str());
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

//==============================================================================
//ImGui
//==============================================================================
namespace
{
	// m_Parameter の index 番目（0:x 1:y 2:z 3:w）への参照
	float& ParamComponent(XMFLOAT4& p, int index)
	{
		switch (index)
		{
		case 0:  return p.x;
		case 1:  return p.y;
		case 2:  return p.z;
		default: return p.w;
		}
	}
}

int MaterialCombo(int currentIndex)
{
	const std::vector<MaterialDesc>& table = GetMaterialTable();
	int result = -1;
	if (ImGui::BeginCombo("Material", table[currentIndex].Name))
	{
		for (int i = 0; i < (int)table.size(); i++)
		{
			bool selected = (i == currentIndex);
			if (ImGui::Selectable(table[i].Name, selected))
			{
				result = i;
			}
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return result;
}

void DrawMaterialSettings(const MaterialDesc& desc, LIGHT& light, XMFLOAT4& parameter)
{
	ImGui::SeparatorText("Light");
	if (desc.UseGlobalLight)
	{
		ImGui::TextDisabled("Using g_Light (Game.cpp)");	// ImGuiは日本語フォント未設定のため英語
	}
	else
	{
		DragFloat3Row("LightDirection", "Light Direction", light.Direction);
		DragFloat3Row("LightPosition",  "Light Position",  light.Position);

		ImGui::ColorEdit3("Diffuse", &light.Diffuse.x);
		ImGui::ColorEdit3("Ambient", &light.Ambient.x);

		DragFloat3Row("PointLightParam", "Point Light Param", light.PointLightParam);
	}

	// マテリアル固有のパラメータ（MaterialDesc::ParamUIs から自動生成）
	if (!desc.ParamUIs.empty())
	{
		ImGui::SeparatorText(desc.Name);
		for (const ParamUI& ui : desc.ParamUIs)
		{
			float& value = ParamComponent(parameter, ui.Index);
			if (ui.DragSpeed > 0.0f)
				ImGui::DragFloat(ui.Label, &value, ui.DragSpeed, ui.Min, ui.Max, ui.Format);
			else
				ImGui::SliderFloat(ui.Label, &value, ui.Min, ui.Max, ui.Format);
		}
	}
}
