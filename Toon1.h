#pragma once
#include "PolygonModel.h"
class Toon1 :public PolygonModel
{
public:
	virtual const char* GetVertexShaderPath() const override { return "Toon1VS.cso"; }
	virtual const char* GetPixelShaderPath()  const override { return "Toon1PS.cso"; }
	virtual const char* GetModelPath()  const override { return "asset\\model\\model.fbx"; }
	virtual const wchar_t* GetTexturePath()  const override { return L"asset\\texture\\sura.jpg"; }
	virtual const char* GetName() const override { return "Toon"; }
	virtual void Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;
	
	XMFLOAT4 m_Parameter;
};

