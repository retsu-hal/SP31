#pragma once
#include "Sprite2D.h"

// RT0 →(横ブラー)→ RT1 →(縦ブラー)→ バックバッファ
// Parameter.x:画面幅 y:画面高さ z:分散(ボケ強さ) w:サンプリング間隔
class Gaussian : public Sprite2D
{
protected:
	Material m_MaterialV;		// 縦ブラー用（横は親の m_Material）
	float    m_Weight[8] = {};	// [0]=中心, [1]〜[7]=1〜7px 離れた位置

	void CalcGaussianWeight(float dispersion);
	const char* GetTypeName() const override { return "Gaussian"; }
	void DrawImGuiExtra() override;

public:
	Gaussian() : Sprite2D("GaussianH") {}

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override {}	// 通常の2Dループでは描かない（DrawGame から DrawPass を呼ぶ）
	void DrawPass(int pass);	// 0:横(RT0を読む)  1:縦(RT1を読む)
};