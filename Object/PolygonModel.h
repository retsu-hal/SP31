#pragma once

#include <string>
#include "Game.h"
#include "GameObject.h"
#include "Material.h"

//------------------------------------------------------------------------------
// 3Dモデルを1つ描画するオブジェクト
//   シェーダーごとの違いは MaterialDesc（MaterialTable.cpp）で表すので、
//   シェーダーを増やしても派生クラスを作る必要はない。
//
//   例) new PolygonModel("Toon2", XMFLOAT3(0.5f, 0.5f, 0.0f))
//------------------------------------------------------------------------------
class PolygonModel : public GameObject
{
public:
	explicit PolygonModel(const char* materialName = "UnlitTexture",
	                      XMFLOAT3 position = XMFLOAT3(0.0f, 0.5f, 0.0f));

	void Init(void)override;
	void Uninit(void)override;
	void Update(void)override;
	void Draw(void)override;
	void DrawModel(void);

	void DrawImGui() override;

	// マテリアルを切り替える（resetParams が true ならパラメータ・ライトを初期値に戻す）
	void ChangeMaterial(int materialIndex, bool resetParams = true);

protected:
	const MaterialDesc& GetDesc() const { return GetMaterialTable()[m_MaterialIndex]; }

	const char*    GetVertexShaderPath() const override { return GetDesc().VertexShader; }
	const char*    GetPixelShaderPath()  const override { return GetDesc().PixelShader; }
	const char*    GetModelPath()        const override { return GetDesc().Model; }
	const wchar_t* GetTexturePath()      const override { return GetDesc().Texture; }
	const char*    GetName()             const override { return GetDesc().Name; }

	std::string m_MaterialName;			// コンストラクタで指定された名前（Init で解決）
	int         m_MaterialIndex = 0;	// GetMaterialTable() の添字
	Material    m_Material;
	std::string m_LoadedModelPath;		// 読み込み済みモデル（切り替え時の再読み込み判定用）
	XMFLOAT3    m_InitialPosition;
};
