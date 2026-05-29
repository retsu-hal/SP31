#pragma once
#include "PolygonModel.h"
class LimLighting :public PolygonModel
{
	public:
		const char* GetVertexShaderPath() const override { return "LimLightingVS.cso"; }
		const char* GetPixelShaderPath()  const override { return "LimLightingPS.cso"; }
		const char* GetName() const override { return "LimLighting"; }

		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;
};

