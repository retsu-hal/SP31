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

	MaterialDesc UnlitTextureMipMap()
	{
		MaterialDesc m = MakeMaterial("UnlitTexture", "UnlitTextureVS.cso", "UnlitTexturePS.cso");
		m.ParamUIs = { { "MipMap Level", 0, 0.0f, 7.0f, "%.0f" } };	// x:ミップレベル
		return m;
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
		m.Parameter = XMFLOAT4(30.0f, 0.0f, 0.0f, 0.0f);	// x:スペキュラの強さ、y:スペキュラの色、z:スペキュラの光沢度
		m.ParamUIs = { { "Specular Power", 0, 1.0f, 128.0f, "%.0f" } };
		return m;
	}

	MaterialDesc PixelLightingBlinnPhong()
	{
		MaterialDesc m = MakeMaterial("PixelLightingBlinnPhong", "PixelLightingBlinnPhongVS.cso", "PixelLightingBlinnPhongPS.cso");
		m.Model = "asset\\model\\cube.fbx";
		m.Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);		// 光の色
		m.Light.Ambient = XMFLOAT4(0.5f, 0.3f, 0.3f, 1.0f);	// 環境光
		return m;
	}

	MaterialDesc SpotLighting()
	{
		MaterialDesc m = MakeMaterial("SpotLighting", "SpotLightingVS.cso", "SpotLightingPS.cso");
		m.UseGlobalLight = true;	// g_Light を使う
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
		m.Light = PointLight();                          // 点光源の設定を流用
		m.Parameter = XMFLOAT4(3.0f, 0.0f, 0.0f, 0.0f);      // x:リムの pow 指数
		m.ParamUIs = { { "Rim Power", 0, 1.0f, 10.0f, "%.1f" } };
		return m;
	}

	MaterialDesc RGBShift()
	{
		MaterialDesc m = MakeMaterial("RGBShift", "UnlitTextureVS.cso", "RGBShiftPS.cso");
		m.Parameter = XMFLOAT4(0.01f, 0.01f, 0.0f, 0.0f);	// x:シフト量
		m.ParamUIs = {
			{ "RShift", 0, 0.0f, 0.5f, "%.4f" },
			{ "BShift", 1, 0.0f, 0.5f, "%.4f" },
		};
		return m;
	}

	MaterialDesc CookTorrance()
	{
		// VSはポイントライトのものをそのまま使う
		MaterialDesc m = MakeMaterial("CookTorrance", "PointPixelLightingVS.cso", "CookTorrancePS.cso");
		m.Model = "asset\\model\\ball.fbx";
		m.Light = PointLight();
		m.Parameter = XMFLOAT4(0.3f, 0.8f, 0.0f, 0.0f);	// x:ざらつき y:金属感
		m.ParamUIs = {
			{ "Roughness", 0, 0.05f, 1.0f, "%.2f" },	// 0だとBeckmannが0を返すので下限0.05
			{ "Metallic",  1, 0.0f,  1.0f, "%.2f" },
		};
		return m;
	}

	MaterialDesc DisneyPBR()
	{
		MaterialDesc m = MakeMaterial("DisneyPBR", "PointPixelLightingVS.cso", "DisneyPBRPS.cso");
		m.Model = "asset\\model\\ball.fbx";
		m.Light = PointLight();
		m.Light.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
		m.Light.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);	// 少し大きめ
		m.Light.PointLightParam = XMFLOAT4(2000.0f, 1.5f, 0.0f, 0.0f);	// 減衰率
		m.Parameter = XMFLOAT4(0.5f, 0.5f, 3.0f, 0.0f);	// z:ライト数
		m.ExtraTextures = {
			{ 1, L"asset\\texture\\Roughness.jpg" },	// t1
			{ 2, L"asset\\texture\\Metalness.jpg" },	// t2
		};
		m.ParamUIs = { { "Light", 2, 1.0f, 15.0f, "%.0f" } };
		return m;
	}

	MaterialDesc Bump()
	{
		MaterialDesc m = MakeMaterial("Bump", "PointPixelLightingVS.cso", "BumpPS.cso");
		m.UseGlobalLight = true;	// g_Light（SPOT LIGHTウィンドウ）で光源を動かす
		m.ExtraTextures = {
			{ 1, L"asset\\texture\\Normal.png" },	// t1：法線マップ
		};
		return m;
	}

	MaterialDesc Toon1()
	{
		MaterialDesc m = MakeMaterial("Toon1", "PointPixelLightingVS.cso", "Toon1PS.cso");
		m.Light = PointLight();
		m.Parameter = XMFLOAT4(0.4f, 0.7f, 0.0f, 0.0f);   // x:Level-1 y:Level-2
		m.ParamUIs = {
			{ "Level-1", 0, 0.0f, 1.0f, "%.2f" },
			{ "Level-2", 1, 0.0f, 1.0f, "%.2f" },
		};
		return m;
	}

	MaterialDesc Toon2()
	{
		MaterialDesc m = MakeMaterial("Toon2", "PointPixelLightingVS.cso", "Toon2PS.cso");
		m.Light = PointLight();
		m.Parameter = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);   // x:Texture V
		m.ExtraTextures = { { 1, L"asset\\texture\\Toon.png" } };   // t1
		m.ParamUIs = { { "Texture V", 0, 0.0f, 1.0f, "%.4f" } };
		return m;
	}

	MaterialDesc Toon3()
	{
		MaterialDesc m = Toon2();          // 本体はToon2を流用
		m.Name = "Toon3";
		m.Parameter = XMFLOAT4(0.0f, 0.02f, 0.0f, 0.0f);   // x:Texture V  y:エッジ太さ
		m.OutlineVertexShader = "ToonEdgeVS.cso";
		m.OutlinePixelShader = "ToonEdgePS.cso";
		m.ParamUIs = {
			{ "Texture V",       0, 0.0f, 1.0f, "%.4f" },
			{ "EdgeModel Scale", 1, 0.0f, 0.5f, "%.3f", 0.001f },  // DragFloat
		};
		return m;
	}

	MaterialDesc Zukei()
	{
		MaterialDesc m = MakeMaterial("Zukei", "UnlitTextureVS.cso", "ZukeiPS.cso");
		m.Parameter = XMFLOAT4(0.8f, 0.01f, 0.05f, 0.0f);	// x:サイズ y:ぼかし幅 z:幅/くぼみ/花びら w:図形番号
		m.ParamUIs = {
			{ "Size",  0, 0.0f, 1.0f, "%.3f" },
			{ "Edge",  1, 0.0f, 0.3f, "%.3f" },
			{ "Z (Ring:width / Heart:kubomi / Flower:petal)", 2, 0.0f, 6.0f, "%.3f", 0.005f },	// DragFloat
			{ "Shape (0:円 1:リング 2:ひし形 3:ハート 4:花)", 3, 0.0f, 4.0f, "%.0f" },
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
		UnlitColor(),
		UnlitTexture(),
		GrayscaleTexture(),
		SepiaTexture(),
		PixelDirectionalLighting(),
		PixelLightingBlinnPhong(),
		SpotLighting(),
		PointPixelLighting(),
		LimLighting(),
		RGBShift(),
		CookTorrance(),
		DisneyPBR(),
		Bump(),
		Toon1(),
		Toon2(),
		Toon3(),
		Zukei(),
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
