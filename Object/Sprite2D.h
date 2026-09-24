/*==============================================================================

   頂点管理[Sprite2D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------
	2Dスプライト。シェーダーは PolygonModel / Field3D と同じく
	MaterialDesc（MaterialTable.cpp）から読み込むので、ImGui で切り替えられる。

	例) new Sprite2D("GrayscaleTexture", L"asset\\texture\\texture.jpg")
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

// 位置は m_Position（スクリーン座標）、回転は m_Rotation.z（度）、拡大縮小は m_Scale.x / y を使う
class Sprite2D : public GameObject
{
	protected:
		XMFLOAT4	Color;			//色
		XMFLOAT2	Size;			//サイズ

		const MaterialDesc& GetDesc() const { return GetMaterialTable()[m_MaterialIndex]; }

		const char*    GetVertexShaderPath() const override { return GetDesc().VertexShader; }
		const char*    GetPixelShaderPath()  const override { return GetDesc().PixelShader; }
		const wchar_t* GetTexturePath()      const override { return m_TexturePath ? m_TexturePath : GetDesc().Texture; }
		const char*    GetName()             const override { return GetDesc().Name; }

		std::string    m_MaterialName;			// コンストラクタで指定された名前（Init で解決）
		int            m_MaterialIndex = 0;		// GetMaterialTable() の添字
		Material       m_Material;
		const wchar_t* m_TexturePath;			// nullptr ならマテリアルの基本テクスチャを使う

	public:
		explicit Sprite2D(const char* materialName = "UnlitColor",
			const wchar_t* texturePath = L"asset\\texture\\texture.jpg");

		void Init(void) override;
		void Uninit(void) override;
		void Update(void) override;
		void Draw(void) override;

		void DrawImGui() override;

		// マテリアルを切り替える（resetParams が true ならパラメータ・ライトを初期値に戻す）
		void ChangeMaterial(int materialIndex, bool resetParams = true);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
