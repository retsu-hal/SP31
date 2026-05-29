#pragma once

#include "GameObject.h"

class PolygonModel : public GameObject
{
protected:
	virtual const char* GetVertexShaderPath() const override { return "UnlitTextureVS.cso"; }
	virtual const char* GetPixelShaderPath()  const  override { return "UnlitTexturePS.cso"; }
	virtual const char* GetModelPath()  const override { return "asset\\model\\model.fbx"; }
	virtual const wchar_t* GetTexturePath()  const override{ return L"asset\\texture\\sura.jpg"; }

public:
	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
};