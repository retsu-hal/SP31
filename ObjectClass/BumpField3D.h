#pragma once

#include "GameObject.h"

class BumpField3D : public GameObject
{
protected:
	int m_BumpTexID;

public:
	const char* GetVertexShaderPath() const override { return "BumpVS.cso"; }
	const char* GetPixelShaderPath()  const override { return "BumpPS.cso"; }
	const wchar_t* GetTexturePath()  const override { return L"asset\\texture\\sura.jpg"; }
	const wchar_t* GetBumpTexturePath() const { return L"asset\\texture\\Normal.png"; }
	const char* GetName() const override { return "BumpField3D"; }
	void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;
	void DrawImGui()override;
};