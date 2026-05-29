#pragma once

#include "PolygonModel.h"
class PixelLightingBlinnPhong : public PolygonModel
{
	public:
	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
};

