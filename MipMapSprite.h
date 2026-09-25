#pragma once
#include "Sprite2D.h"

// レンダリングテクスチャ（GetPeTexture）を貼って表示するスプライト
// Parameter.x でミップマップレベルを指定する
class MipMapSprite : public Sprite2D
{
protected:
	int m_PeNo;		// 何番のレンダリングテクスチャを貼るか

public:
	explicit MipMapSprite(const char* materialName = "UnlitTexture", int peNo = 0);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
};