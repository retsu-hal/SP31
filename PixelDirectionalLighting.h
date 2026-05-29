#pragma once

#include "PolygonModel.h"


class  PixelDirectionalLighting : public PolygonModel
{
public:
	const char* GetVertexShaderPath() const override { return "PixelDirectionalLightingVS.cso"; }
	const char* GetPixelShaderPath()  const override { return "PixelDirectionalLightingPS.cso"; }

	void Init(void);
	void Finalize(void);
	void Update(void);
	void Draw(void);
};
