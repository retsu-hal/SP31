/*==============================================================================

[Material.h]
	シェーダー1組ぶんの設定を「データ」として持つための仕組み。

	・MaterialDesc : シェーダー名・パラメータ初期値・ライト初期値・追加テクスチャ・
	                 ImGui に出すスライダーなどを1つにまとめた「設定表の1行」
	・Material     : MaterialDesc を元に、実際のシェーダー／テクスチャを読み込んで
	                 描画前にセット(Bind)する実体
	・シェーダーキャッシュ : 同じ .cso を何度も読み込まないための共有置き場

	新しいシェーダーを追加するときは MaterialTable.cpp に1関数(1行)足すだけでよい。
==============================================================================*/
#pragma once

#include <vector>
#include "main.h"
#include "renderer.h"

//------------------------------------------------------------------------------
// 追加テクスチャ（ピクセルシェーダーの t1 以降にセットするもの）
//------------------------------------------------------------------------------
struct TextureSlot
{
	UINT           Slot;	// PSSetShaderResources のスロット番号（0番は基本テクスチャ用）
	const wchar_t* Path;	// テクスチャのパス
};

//------------------------------------------------------------------------------
// ImGui に表示する m_Parameter 用スライダーの定義
//------------------------------------------------------------------------------
struct ParamUI
{
	const char* Label;				// 表示名
	int         Index;				// m_Parameter の何番目か（0:x 1:y 2:z 3:w）
	float       Min;
	float       Max;
	const char* Format    = "%.3f";
	float       DragSpeed = 0.0f;	// 0 なら SliderFloat、0より大きければ DragFloat
};

//------------------------------------------------------------------------------
// マテリアル（シェーダー1組ぶん）の設定
//------------------------------------------------------------------------------
struct MaterialDesc
{
	const char*    Name         = "Material";
	const char*    VertexShader = "UnlitTextureVS.cso";
	const char*    PixelShader  = "UnlitTexturePS.cso";
	//const char*    Model        = "asset\\model\\model.fbx";
	//const wchar_t* Texture      = L"asset\\texture\\sura.jpg";	// t0 にセットする基本テクスチャ

	XMFLOAT4 Parameter{ 0.0f, 0.0f, 0.0f, 0.0f };	// m_Parameter の初期値
	LIGHT    Light{};								// m_Light の初期値

	std::vector<TextureSlot> ExtraTextures;		// t1 以降の追加テクスチャ
	std::vector<ParamUI>     ParamUIs;			// ImGui に出すパラメータ

	bool UseGlobalLight = false;				// true なら描画時に g_Light を使う（スポットライト等）

	// アウトライン（2パス目）用シェーダー。両方指定すると
	// 「裏面カリングで通常描画 → 表面カリングでアウトライン描画」を行う
	const char* OutlineVertexShader = nullptr;
	const char* OutlinePixelShader  = nullptr;

	bool HasOutline() const { return OutlineVertexShader && OutlinePixelShader; }
};

//------------------------------------------------------------------------------
// MaterialDesc から読み込んだシェーダー・テクスチャの実体
//------------------------------------------------------------------------------
class Material
{
public:
	void Load(const MaterialDesc* desc);	// シェーダー（キャッシュ経由）とテクスチャを読み込む
	void Bind() const;						// 追加テクスチャ・入力レイアウト・VS/PS をセット
	void BindOutline() const;				// アウトライン用 VS/PS をセット

	const MaterialDesc* GetDesc() const { return m_Desc; }

private:
	const MaterialDesc* m_Desc = nullptr;

	// シェーダーはキャッシュが所有しているので、ここでは Release しない
	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader*  m_PixelShader  = nullptr;
	ID3D11InputLayout*  m_VertexLayout = nullptr;

	ID3D11VertexShader* m_OutlineVertexShader = nullptr;
	ID3D11PixelShader*  m_OutlinePixelShader  = nullptr;
	ID3D11InputLayout*  m_OutlineVertexLayout = nullptr;

	std::vector<int> m_ExtraTexIDs;			// ExtraTextures と同じ並び
};

//------------------------------------------------------------------------------
// シェーダーキャッシュ（同じ .cso は1回だけ読み込む）
//------------------------------------------------------------------------------
void GetCachedVertexShader(const char* fileName, ID3D11VertexShader** vs, ID3D11InputLayout** layout);
void GetCachedPixelShader(const char* fileName, ID3D11PixelShader** ps);
void ReleaseShaderCache();					// FinalizeGame で呼ぶ

//------------------------------------------------------------------------------
// マテリアル表（MaterialTable.cpp）
//------------------------------------------------------------------------------
const std::vector<MaterialDesc>& GetMaterialTable();
int FindMaterialIndex(const char* name);	// 見つからなければ -1
