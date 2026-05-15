
#pragma once

#include "PolygonModel.h"

//==================
// ç\ë¢ëÃ
//==================
class VertexDirectionalLighting : public PolygonModel
{
public:
	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;

};
