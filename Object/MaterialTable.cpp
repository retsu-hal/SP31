/*==============================================================================

[MaterialTable.cpp]
	シェーダー（マテリアル）の設定表。

	■ 新しいシェーダーを追加する手順
	  1. shader フォルダに xxxVS.hlsl / xxxPS.hlsl を作る
	  2. このファイルに MaterialDesc を返す関数を1つ書く
	  3. GetMaterialTable() の表に1行足す
	  4. Game.cpp で new PolygonModel("名前", 位置) する（または ImGui で切り替え）
	  → .h / .cpp のクラスを新しく作る必要はない
==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include <cstring>
#include "Material.h"
#include "GameObject.h"

//==============================================================================
//ライト初期値のひな形
//==============================================================================
namespace
{
	// 旧 PolygonModel::Init で設定していたライト
	LIGHT BaseLight()
	{
		LIGHT light{};
		XMVECTOR dir = XMVector3Normalize(XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f));
		XMStoreFloat4(&light.Direction, dir);						// 光のベクトル
		light.Position        = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);	// 点光源の位置
		light.Diffuse         = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	// 光の色
		light.Ambient         = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);	// 環境光
		light.PointLightParam = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		return light;
	}

	// 点光源系（PointPixelLighting / Toon 系で共通）
	LIGHT PointLight()
	{
		LIGHT light = BaseLight();
		light.Diffuse         = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		light.Ambient         = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		light.PointLightParam = XMFLOAT4(3.0f, 0.0f, 0.0f, 1.0f);	// x:減衰距離
		return light;
	}

	MaterialDesc MakeMaterial(const char* name, const char* vs, const char* ps)
	{
		MaterialDesc m;
		m.Name         = name;
		m.VertexShader = vs;
		m.PixelShader  = ps;
		m.Light        = BaseLight();
		return m;
	}

//==============================================================================
//各マテリアルの定義
//==============================================================================
	MaterialDesc UnlitColor()
	{
		return MakeMaterial("UnlitColor", "UnlitColorVS.cso", "UnlitColorPS.cso");
	}
	
	
	MaterialDesc UnlitTexture()
	{
		return MakeMaterial("UnlitTexture", "UnlitTextureVS.cso", "UnlitTexturePS.cso");
	}

	MaterialDesc GrayscaleTexture()
	{
		return MakeMaterial("GrayscaleTexture", "GrayscaleVS.cso", "GrayscalePS.cso");
	}

	MaterialDesc SepiaTexture()
	{
		return MakeMaterial("SepiaTexture", "SepiaVS.cso", "SepiaPS.cso");
	}

	MaterialDesc PixelDirectionalLighting()
	{
		MaterialDesc m = MakeMaterial("PixelDirectionalLighting", "PixelDirectionalLightingVS.cso", "PixelDirectionalLightingPS.cso");
		m.Parameter=XMFLOAT4(30.0f, 0.0f, 0.0f, 0.0f);
		m.ParamUIs = { { "Specular Power", 0, 1.0f, 128.0f, "%.0f" } };
		return m;
	}
}

//==============================================================================
//マテリアル表
//==============================================================================
const std::vector<MaterialDesc>& GetMaterialTable()
{
	// ★シェーダーを追加したらここに1行足す
	static const std::vector<MaterialDesc> table =
	{
		UnlitColor(),
		UnlitTexture(),
		GrayscaleTexture(),
		SepiaTexture(),
		PixelDirectionalLighting(),
	};
	return table;
}

int FindMaterialIndex(const char* name)
{
	const std::vector<MaterialDesc>& table = GetMaterialTable();
	for (int i = 0; i < (int)table.size(); i++)
	{
		if (strcmp(table[i].Name, name) == 0) return i;
	}
	return -1;
}
