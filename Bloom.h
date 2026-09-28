#pragma once
#include "Sprite2D.h"

// RT0 →(輝度抽出)→ RT1 → ミップ生成 → RT0 + RT1のミップ → バックバッファ
// Parameter.x:しきい値  y,z,w:ミップ 2.5 / 4.5 / 6.5 を混ぜる強さ
class Bloom : public Sprite2D
{
protected:
	Material m_MaterialComposite;	// パス3用（パス2は親の m_Material）

	const char* GetTypeName() const override { return "Bloom"; }
	void DrawImGuiExtra() override;

public:
	Bloom() : Sprite2D("BloomLuminance") {}

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override {}	// DrawGame から DrawPass を呼ぶ
	void DrawPass(int pass);	// 0:輝度抽出(RT0を読む)  1:合成(RT0とRT1を読む)
};