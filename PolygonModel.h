#pragma once

#include "GameObject.h"

//==================
// ç\ë¢ëÃ
//==================
class PolygonModel : public GameObject
{
protected:
	
public:
	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;

};