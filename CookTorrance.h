#pragma once
#include "PolygonModel.h"
class CookTorrance :public PolygonModel
{
public:
	const char* GetVertexShaderPath() const override { return "CookTorranceVS.cso"; }
	const char* GetPixelShaderPath()  const override { return "CookTorrancePS.cso"; }
	const char* GetName() const override { return "CookTorrance"; }

	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;

protected:
	void DrawImGuiExtra() override;	// 標準インスペクタのヘッダ内に追記する
	XMFLOAT4 m_Parameter;			// x:ざらつき(Roughness)  y:金属感(Metallic)
};