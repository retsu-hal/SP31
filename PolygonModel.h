#pragma once

#include "GameObject.h"

class PolygonModel : public GameObject
{
protected:
	 const char* GetVertexShaderPath() const override { return "UnlitTextureVS.cso"; }
	 const char* GetPixelShaderPath()  const  override { return "UnlitTexturePS.cso"; }
	 const char* GetModelPath()  const override { return "asset\\model\\model.fbx"; }
	 const wchar_t* GetTexturePath()  const override{ return L"asset\\texture\\sura.jpg"; }
	 const char* GetName() const override{ return "PolygonModel"; }
public:
	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
};