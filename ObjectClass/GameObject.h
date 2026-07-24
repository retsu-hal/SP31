#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

// COMインターフェース解放ヘルパー：ポインタを解放してnullを代入する
template<class T> inline void SafeRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

// シーン全体で共有するデフォルトのLIGHT設定を構築する
inline LIGHT MakeDefaultLight()
{
	LIGHT light{};
	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	XMStoreFloat4(&light.Direction, dir);
	light.Position        = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	light.Diffuse         = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.Ambient         = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.SkyColor        = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
	light.GroundColor     = XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f);
	light.PointLightParam = XMFLOAT4(10.0f, 1.0f, 1.0f, 0.0f);
	light.Angle           = XMFLOAT4(XMConvertToRadians(30.0f), 0.0f, 0.0f, 0.0f);
	return light;
}

class GameObject
{
protected:	// 継承元ではアクセス不可だが、子クラスからはアクセス可
	XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_Parameter{ 0.0f, 0.0f, 0.0f, 0.0f };	// x,y,z,w : シェーダーパラメータ
	
	int m_TexID = -1;

	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;
	ID3D11SamplerState* m_SamplerState = nullptr;


	MODEL* m_Model = nullptr;
	LIGHT m_Light;
	
	virtual const char* GetVertexShaderPath() const { return "UnlitTextureVS.cso"; }
	virtual const char* GetPixelShaderPath()  const { return "UnlitTexturePS.cso"; }
	virtual const char* GetModelPath()  const { return "asset\\model\\model.fbx"; }
	virtual const wchar_t* GetTexturePath()  const { return L"asset\\texture\\sura.jpg"; }
	virtual const char* GetName() const { return "GameObject"; }
public:
	virtual ~GameObject() {}

	bool m_Is2D = true;		// true:2D オブジェクト、false:3D オブジェクト

	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update() {};
	virtual void Draw() {};

	virtual void  DrawImGui() {};
	virtual void DrawImGuiExtra() {};

	// このオブジェクトのトランスフォームからS*R*Tのワールド行列を生成する（3D）
	XMMATRIX GetWorldMatrix() const
	{
		XMMATRIX Scaling, Rotation, Translation;
		Scaling = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		Rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_Rotation.x), XMConvertToRadians(m_Rotation.y), XMConvertToRadians(m_Rotation.z));
		Translation = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		return Scaling * Rotation * Translation;
	}
};