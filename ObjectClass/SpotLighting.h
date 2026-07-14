#pragma once
#include "PolygonModel.h"
class SpotLighting :public PolygonModel
{
public:
	const char* GetVertexShaderPath() const override { return "SpotLightingVS.cso"; }
	const char* GetPixelShaderPath()  const override { return "SpotLightingPS.cso"; }
	const char* GetName() const override { return "SpotLighting"; }

	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
	void DrawImGui(void)override;
};

