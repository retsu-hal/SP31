#pragma once
#include "PolygonModel.h"

//==============================================================================
// ディズニーPBR（拡散反射＋クックトランス鏡面）
//   ・CookTorrance と同じく PolygonModel を継承
//   ・Roughness / Metalness をテクスチャでピクセルごとに指定
//   ・Parameter.z を「使用するライトの数」として使う
//==============================================================================
class DisneyPBR : public PolygonModel
{
public:
	const char* GetVertexShaderPath() const override { return "DisneyPBRVS.cso"; }
	const char* GetPixelShaderPath()  const override { return "DisneyPBRPS.cso"; }
	const char* GetName() const override { return "DisneyPBR"; }

	void Init(void)   override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void)   override;
	void DrawImGuiExtra(void) override;

protected:
	// x:ざらつき(Roughness)  y:金属感(Metallic)  z:ライト数
	/* m_Parameter は GameObject から継承 */

	// 追加したテクスチャオブジェクトの変数
	int m_TexIDRoughness = -1;
	int m_TexIDMetalness = -1;

	// 追加テクスチャのパス（asset\texture へ配置）
	const wchar_t* GetRoughnessTexturePath() const { return L"asset\\texture\\Roughness.jpg"; }
	const wchar_t* GetMetalnessTexturePath() const { return L"asset\\texture\\Metalness.jpg"; }
};
