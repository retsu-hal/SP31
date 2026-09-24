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
#include "GameObject.h"		// MakeDefaultLight

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
	MaterialDesc UnlitTexture()
	{
		return MakeMaterial("UnlitTexture", "UnlitTextureVS.cso", "UnlitTexturePS.cso");
	}

	MaterialDesc VertexDirectionalLighting()
	{
		return MakeMaterial("VertexDirectionalLighting", "VertexDirectionalLightingVS.cso", "VertexDirectionalLightingPS.cso");
	}

	MaterialDesc PixelDirectionalLighting()
	{
		return MakeMaterial("PixelDirectionalLighting", "PixelDirectionalLightingVS.cso", "PixelDirectionalLightingPS.cso");
	}

	MaterialDesc PixelLightingBlinnPhong()
	{
		MaterialDesc m = MakeMaterial("PixelLightingBlinnPhong", "PixelLightingBlinnPhongVS.cso", "PixelLightingBlinnPhongPS.cso");
		m.Model         = "asset\\model\\cube.fbx";
		m.Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		m.Light.Ambient = XMFLOAT4(0.5f, 0.3f, 0.3f, 1.0f);
		return m;
	}

	MaterialDesc HemiSphereLighting()
	{
		MaterialDesc m = MakeMaterial("HemiSphereLighting", "HemiSphereLightingVS.cso", "HemiSphereLightingPS.cso");
		m.Light.Diffuse      = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);	// 拡散光の色
		m.Light.Ambient      = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);	// 環境光の色
		m.Light.GroundNormal = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		m.Light.SkyColor     = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
		m.Light.GroundColor  = XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f);
		return m;
	}

	MaterialDesc PointPixelLighting()
	{
		MaterialDesc m = MakeMaterial("PointPixelLighting", "PointPixelLightingVS.cso", "PointPixelLightingPS.cso");
		m.Light = PointLight();
		return m;
	}

	MaterialDesc LimLighting()
	{
		MaterialDesc m = MakeMaterial("LimLighting", "LimLightingVS.cso", "LimLightingPS.cso");
		m.Light = PointLight();
		m.Light.PointLightParam = XMFLOAT4(3.0f, 3.0f, 0.0f, 1.0f);
		return m;
	}

	MaterialDesc SpotLighting()
	{
		MaterialDesc m = MakeMaterial("SpotLighting", "SpotLightingVS.cso", "SpotLightingPS.cso");
		m.UseGlobalLight = true;	// Game.cpp の g_Light を使う
		return m;
	}

	MaterialDesc CookTorrance()
	{
		MaterialDesc m = MakeMaterial("CookTorrance", "CookTorranceVS.cso", "CookTorrancePS.cso");
		m.Parameter = XMFLOAT4(0.3f, 0.8f, 0.0f, 0.0f);	// x:ざらつき(Roughness) y:金属感(Metallic)
		m.Light     = MakeDefaultLight();
		m.ParamUIs  = {
			{ "Roughness", 0, 0.0f, 1.0f, "%.1f" },
			{ "Metallic",  1, 0.0f, 1.0f, "%.1f" },
		};
		return m;
	}

	MaterialDesc DisneyPBR()
	{
		MaterialDesc m = MakeMaterial("DisneyPBR", "DisneyPBRVS.cso", "DisneyPBRPS.cso");
		m.Parameter = XMFLOAT4(0.5f, 0.5f, 3.0f, 0.0f);	// x:Roughness y:Metallic z:ライト数
		m.Light.Direction       = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		m.Light.Position        = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		m.Light.Diffuse         = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		m.Light.Ambient         = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		m.Light.PointLightParam = XMFLOAT4(2000.0f, 1.5f, 0.0f, 0.0f);
		m.ExtraTextures = {
			{ 1, L"asset\\texture\\Roughness.jpg" },	// t1:粗さ
			{ 2, L"asset\\texture\\Metalness.jpg" },	// t2:金属度
		};
		m.ParamUIs = {
			{ "Light", 2, 1.0f, 15.0f, "%.0f" },
		};
		return m;
	}

	MaterialDesc Toon1()
	{
		MaterialDesc m = MakeMaterial("Toon1", "Toon1VS.cso", "Toon1PS.cso");
		m.Parameter = XMFLOAT4(0.4f, 0.7f, 0.0f, 0.0f);	// x:Level-1 のしきい値 y:Level-2 のしきい値
		m.Light     = PointLight();
		m.ParamUIs  = {
			{ "Level1", 0, 0.0f, 1.0f, "%.2f" },
			{ "Level2", 1, 0.0f, 1.0f, "%.2f" },
		};
		return m;
	}

	MaterialDesc Toon2()
	{
		MaterialDesc m = MakeMaterial("Toon2", "Toon2VS.cso", "Toon2PS.cso");
		m.Parameter     = XMFLOAT4(0.03f, 0.0f, 0.0f, 0.0f);	// x:ランプテクスチャのV座標
		m.Light         = PointLight();
		m.ExtraTextures = { { 1, L"asset\\texture\\Toon2.png" } };	// t1:ランプ(明るさのLUT)
		m.ParamUIs      = { { "Texture V", 0, 0.0f, 1.0f, "%.4f" } };
		return m;
	}

	MaterialDesc Toon3()
	{
		// Toon2 + アウトライン（背面法）
		MaterialDesc m = Toon2();
		m.Name                = "Toon3";
		m.Parameter           = XMFLOAT4(0.03f, 0.02f, 0.0f, 0.0f);	// x:ランプV y:エッジ幅
		m.OutlineVertexShader = "ToonVSEdge.cso";
		m.OutlinePixelShader  = "ToonPSEdge.cso";
		m.ParamUIs = {
			{ "Texture V",       0, 0.0f, 1.0f, "%.3f", 0.001f },
			{ "EdgeModel Scale", 1, 0.0f, 0.5f, "%.3f", 0.001f },
		};
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
		UnlitTexture(),
		VertexDirectionalLighting(),
		PixelDirectionalLighting(),
		PixelLightingBlinnPhong(),
		HemiSphereLighting(),
		PointPixelLighting(),
		LimLighting(),
		SpotLighting(),
		CookTorrance(),
		DisneyPBR(),
		Toon1(),
		Toon2(),
		Toon3(),
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
