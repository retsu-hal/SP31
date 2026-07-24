#pragma once
#include "PolygonModel.h"

class Toon3 : public PolygonModel
{
public:
	const char* GetVertexShaderPath() const override { return "Toon2VS.cso"; }
	const char* GetPixelShaderPath()  const override { return "Toon2PS.cso"; }
	const char* GetName() const override { return "Toon3"; }

	void Init(void)   override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void)   override;
	//void DrawImGui() override;
	void DrawImGuiExtra() override;

protected:

	/* m_Parameter は GameObject から継承 */	// x:ランプテクスチャのV座標（縦方向の固定位置）

	// 追加テクスチャ（ランプ＝明るさのLUT）
	int m_TexIDRamp = -1;
	const wchar_t* GetRampTexturePath() const { return L"asset\\texture\\Toon2.png"; }
};
