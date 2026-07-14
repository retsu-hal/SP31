#pragma once
#include "PolygonModel.h"
class HemiSphereLighting :public PolygonModel
{
	public:
		const char* GetVertexShaderPath() const override { return "HemiSphereLightingVS.cso"; }
		const char* GetPixelShaderPath()  const override { return "HemiSphereLightingPS.cso"; }
		const char* GetName() const override { return "HemiSphereLighting"; }
		
		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;
};

