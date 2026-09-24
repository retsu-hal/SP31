#pragma once

#include "main.h"
#include "renderer.h"
#include <string>
#include <vector>
#include <algorithm>

struct MODEL;	// model.h

// COMオブジェクトを解放して nullptr にする
template<class T>
inline void SafeRelease(T*& p)
{
	if (p) { p->Release(); p = nullptr; }
}

// ライトの初期値（Game.cpp の g_Light などで使う）
inline LIGHT MakeDefaultLight()
{
	LIGHT light{};
	light.Enable = TRUE;
	XMStoreFloat4(&light.Direction, XMVector3Normalize(XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f)));
	light.Position        = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	light.Diffuse         = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.Ambient         = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.PointLightParam = XMFLOAT4(3.0f, 0.0f, 0.0f, 1.0f);
	return light;
}

// ImGui：ラベル付き3成分DragFloat行（X/Y/Zを1行に並べ、末尾にラベル）
template<class Vec>
inline void DragFloat3Row(const char* id, const char* label, Vec& v)
{
	float w3 = (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;
	ImGui::PushID(id);
	ImGui::PushItemWidth(w3);
	ImGui::DragFloat("##X", &v.x, 0.01f, 0.0f, 0.0f, "X:%.2f"); ImGui::SameLine();
	ImGui::DragFloat("##Y", &v.y, 0.01f, 0.0f, 0.0f, "Y:%.2f"); ImGui::SameLine();
	ImGui::DragFloat("##Z", &v.z, 0.01f, 0.0f, 0.0f, "Z:%.2f");
	ImGui::PopItemWidth();
	ImGui::PopID();
	ImGui::SameLine(); ImGui::Text("%s", label);
}

class GameObject
{
private:
protected://外部からアクセスできないが、継承したクラスからアクセスできる
	XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };	// 親から見た位置（親がいなければワールド座標）
	XMFLOAT3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };
	bool m_Destroy = false;
	int m_Layer = 1;	// レイヤー番号
	float m_CameraZ = 0.0f;	//ソート用Z値

	ID3D11Buffer*				m_vertexBuffer = nullptr;	// 頂点バッファ
	ID3D11InputLayout*			m_VertexLayout = nullptr;	// 頂点レイアウト
	ID3D11VertexShader*			m_VertexShader = nullptr;	// 頂点シェーダー
	ID3D11PixelShader*			m_PixelShader = nullptr;	// ピクセルシェーダー
	ID3D11Buffer*				m_indexBuffer = nullptr;	// インデックスバッファ
	ID3D11ShaderResourceView*	m_Texture = nullptr;		// テクスチャ

	// 3Dモデル描画用（PolygonModel で使う）
	MODEL*				m_Model = nullptr;			// モデル
	int					m_TexID = -1;				// テクスチャID
	ID3D11SamplerState*	m_SamplerState = nullptr;	// サンプラー
	LIGHT				m_Light{};					// ライト
	XMFLOAT4			m_Parameter{ 0.0f, 0.0f, 0.0f, 0.0f };	// シェーダーに渡す汎用パラメータ

	// ImGui のリソース表示用（派生クラスで上書きする）
	virtual const char*    GetVertexShaderPath() const { return ""; }
	virtual const char*    GetPixelShaderPath()  const { return ""; }
	virtual const char*    GetModelPath()        const { return ""; }
	virtual const wchar_t* GetTexturePath()      const { return L""; }
	virtual const char*    GetName()             const { return "GameObject"; }
	virtual void DrawImGuiExtra() {}	// DrawImGui の最後に項目を足したいとき用

public:
	bool m_Is2D = false;	// true なら 2D（スクリーン座標）で描画する

	virtual ~GameObject() {}

	//----------------------------------------------------------
	// Transform
	//----------------------------------------------------------
	void SetPosition(const XMFLOAT3& position) { m_Position = position; }
	XMFLOAT3 GetPosition() const { return m_Position; }
	void SetRotation(const XMFLOAT3& rotation) { m_Rotation = rotation; }
	XMFLOAT3 GetRotation() const { return m_Rotation; }
	void SetScale(const XMFLOAT3& scale) { m_Scale = scale; }
	XMFLOAT3 GetScale() const { return m_Scale; }

	virtual void Init(void) {}
	virtual void Uninit(void) {}
	virtual void Update(void) {}
	virtual void Draw(void) {}
	virtual void DrawImGui() {}

	// 拡大縮小 → 回転（度） → 平行移動 のワールド行列
	XMMATRIX GetWorldMatrix() const
	{
		XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		XMMATRIX rot   = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(m_Rotation.x),
			XMConvertToRadians(m_Rotation.y),
			XMConvertToRadians(m_Rotation.z));
		XMMATRIX trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		return scale * rot * trans;
	}
};
