#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

class GameObject
{


protected://外部からアクセスできないが、継承したクラスからアクセスできる
	XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	
	int m_TexID = -1;

	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11InputLayout* m_VertexLayout = nullptr;
	ID3D11SamplerState* m_SamplerState = nullptr;


	MODEL* m_Model = nullptr;
	LIGHT m_Light ;
	
	virtual const char* GetVertexShaderPath() const { return "UnlitTextureVS.cso"; }
	virtual const char* GetPixelShaderPath()  const { return "UnlitTexturePS.cso"; }
	virtual const char* GetModelPath()  const { return "asset\\model\\model.fbx"; }
	virtual const wchar_t* GetTexturePath()  const { return L"asset\\texture\\sura.jpg"; }
	virtual const char* GetName() const { return "GameObject"; }
public:
	virtual ~GameObject() {}

	bool m_Is2D = true;		//true:2Dオブジェクト、false:3Dオブジェクト

	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update() {};
	virtual void Draw() {};
};