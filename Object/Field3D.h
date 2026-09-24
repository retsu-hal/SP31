/*==============================================================================

   [Field3D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------
	地面（板ポリゴン1枚）。シェーダーは PolygonModel と同じく
	MaterialDesc（MaterialTable.cpp）から読み込むので、ImGui で切り替えられる。

	例) new Field3D("UnlitTexture", XMFLOAT3(0.0f, 0.0f, 0.0f))
==============================================================================*/
#pragma once


#include <string>
#include "main.h"
#include "renderer.h"
#include "GameObject.h"
#include "Material.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


////////////////
//構造体
////////////////

class Field3D : public GameObject
{
	public:
		explicit Field3D(const char* materialName = "UnlitTexture",
			XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f));

		void Init(void) override;
		void Uninit(void) override;
		void Update(void) override;
		void Draw(void) override;

		void DrawImGui() override;

		// マテリアルを切り替える（resetParams が true ならパラメータ・ライトを初期値に戻す）
		void ChangeMaterial(int materialIndex, bool resetParams = true);

	protected:
		const MaterialDesc& GetDesc() const { return GetMaterialTable()[m_MaterialIndex]; }

		const char*    GetVertexShaderPath() const override { return GetDesc().VertexShader; }
		const char*    GetPixelShaderPath()  const override { return GetDesc().PixelShader; }
		const wchar_t* GetTexturePath()      const override { return GetDesc().Texture; }
		const char*    GetName()             const override { return GetDesc().Name; }

		std::string m_MaterialName;			// コンストラクタで指定された名前（Init で解決）
		int         m_MaterialIndex = 0;	// GetMaterialTable() の添字
		Material    m_Material;
		XMFLOAT3    m_InitialPosition;
};
