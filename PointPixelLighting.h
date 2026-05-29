#pragma once
#include "PolygonModel.h"
class PointPixelLighting :public PolygonModel
{
	public:
		const char* GetVertexShaderPath() const override { return "PointPixelLightingVS.cso"; }
		const char* GetPixelShaderPath()  const override { return "PointPixelLightingPS.cso"; }
	
		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;
};

