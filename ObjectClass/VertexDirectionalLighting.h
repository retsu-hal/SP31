#pragma once

#include "PolygonModel.h"

class VertexDirectionalLighting : public PolygonModel
{
public:
	const char* GetVertexShaderPath() const override { return "VertexDirectionalLightingVS.cso"; }
	const char* GetPixelShaderPath()  const override { return "VertexDirectionalLightingPS.cso"; }
	const char* GetName() const override { return "VertexDirectionalLighting"; }

	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
};
