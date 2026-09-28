#pragma once
#include "Sprite2D.h"

// レンダリングテクスチャ（GetPeTexture）を全画面に貼り、ホラー風ノイズをかけるスプライト
// Parameter.x:Seed  y:MIN  z:MAX  w:POW
class Horror : public Sprite2D
{
protected:
	int m_PeNo;		// 何番のレンダリングテクスチャを貼るか

	void DrawImGuiExtra() override;
	const char* GetTypeName() const override { return "Horror"; }

public:
	explicit Horror(const char* materialName = "Horror", int peNo = 0);

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
};