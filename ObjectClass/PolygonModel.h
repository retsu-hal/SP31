#pragma once

#include "Game.h"
#include "GameObject.h"

class PolygonModel : public GameObject
{
protected:
	 const char* GetVertexShaderPath() const override { return "UnlitTextureVS.cso"; }
	 const char* GetPixelShaderPath()  const  override { return "UnlitTexturePS.cso"; }
	 const char* GetModelPath()  const override { return "asset\\model\\model.fbx"; }
	 const wchar_t* GetTexturePath()  const override{ return L"asset\\texture\\sura.jpg"; }
	 const char* GetName() const override{ return "PolygonModel"; }

	 ID3D11VertexShader* m_EdgeVS = nullptr;
	 ID3D11PixelShader* m_EdgePS = nullptr;
	 ID3D11InputLayout* m_EdgeLayout = nullptr;

public:
	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
	void DrawModel(void);

	void DrawImGui() override;
};