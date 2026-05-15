#pragma once

#include "GameObject.h"

//==================
// ç\ë¢ëÃ
//==================

class Field3D : public GameObject
{
public:
    void Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;
};