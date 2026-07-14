#pragma once

#include "PolygonModel.h"
class PixelLightingBlinnPhong : public PolygonModel
{
	public:
		const char* GetVertexShaderPath() const override { return "PixelLightingBlinnPhongVS.cso"; }
		const char* GetPixelShaderPath()  const override { return "PixelLightingBlinnPhongPS.cso"; }
		const char* GetModelPath()  const override { return "asset\\model\\cube.fbx"; }
		const char* GetName() const override { return "PixelLightingBlinnPhong"; }

		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;
};

