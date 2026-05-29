#pragma once

#include "GameObject.h"

class Field3D : public GameObject
{
public:
	const char* GetVertexShaderPath() const override{ return "UnlitTextureVS.cso"; }
	const char* GetPixelShaderPath()  const override{ return "UnlitTexturePS.cso"; }
	const wchar_t* GetTexturePath()  const override{ return L"asset\\texture\\sura.jpg"; }
	const char* GetName() const override { return "Field3D"; }
    void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;
};