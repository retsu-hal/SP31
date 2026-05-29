#pragma once
#include "PolygonModel.h"
class LimLighting :public PolygonModel
{
	public:
		const char* GetVertexShaderPath() const override { return "LimLightingVS.cso"; }
		const char* GetPixelShaderPath()  const override { return "LimLightingPS.cso"; }
		
		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;
};

