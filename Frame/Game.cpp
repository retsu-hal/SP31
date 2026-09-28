#include "Main.h"
#include "Renderer.h"
#include "Game.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"
#include "Field3D.h"
#include "PolygonModel.h"
#include "Material.h"
#include "MipMapSprite.h"
#include "Horror.h"
#include "Gaussian.h"
#include <algorithm>

//===============================================
//グローバル変数
//===============================================

// PolygonModel / Field3D / Sprite2D の第1引数はマテリアル名（MaterialTable.cpp に定義）
// 実行中は ImGui の Inspecter → Material で切り替えられる
// ※カメラは GameObject ではないので InitCamera などを直接呼ぶ
Gaussian* g_Gaussian = new Gaussian();

std::vector<GameObject*> g_GameObjects =
{

	new PolygonModel("UnlitTexture",  XMFLOAT3(0.0f, 0.5f, 0.0f)),
	new Field3D("UnlitTexture", XMFLOAT3(0.0f, 0.0f, 0.0f)),
	//new Horror("Horror"),
	//new Sprite2D("UnlitTexture"),
	//g_Gaussian,
	new MipMapSprite("UnlitTexture"),
};

Camera g_Camera;	//カメラ
//ポーズフラグ
static	bool	pause = false;

LIGHT g_Light;
//===============================================
//ポーズフラグセット
void	SetPause(bool flg)
{
	pause = flg;
}
//===============================================
//ポーズフラグ取得
bool	GetPause()
{
	return pause;
}

//===============================================
//g_Gaussian が g_GameObjects に登録されているか
//（コメントアウトされていればブラーなしで描画する）
static bool IsGaussianEnabled()
{
	return std::find(g_GameObjects.begin(), g_GameObjects.end(), g_Gaussian) != g_GameObjects.end();
}

//===============================================
//3Dオブジェクトの描画
static void Draw3DObjects()
{
	g_Camera.Draw();				//ビュー・プロジェクション行列をセット
	Renderer::SetDepthEnable(true);		//奥行き処理有効
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && !gameObject->m_Is2D)
			gameObject->Draw();
	}
}

//===============================================
//ゲームシーン初期化
void InitGame()
{
	Texture::Initialize(Renderer::GetDevice());
	g_Camera.Init();
	
	for(GameObject	*GameObj:g_GameObjects)
	{
		if (GameObj != nullptr)
		{
			GameObj->Init();
		}
	}


	// ライト構造体の初期化
	g_Light = MakeDefaultLight();

	//スポットライト用
	XMVECTOR dir = XMVector4Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	XMStoreFloat4(&g_Light.Direction, dir);						//コーンの向き
	g_Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	g_Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.PointLightParam = XMFLOAT4(10.0f, 1.0f, 0.0f, 0.0f);	//x:距離 y:POW
	g_Light.Angle.x = XMConvertToRadians(30.0f);					//コーン角度
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	//未登録の g_Gaussian はここで解放（登録されていれば下のループで解放される）
	if (!IsGaussianEnabled())
	{
		delete g_Gaussian;
	}
	g_Gaussian = nullptr;

	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Uninit();
			delete gameObject;
		}
	}
	g_GameObjects.clear();

	g_Camera.Finalize();
	ReleaseShaderCache();	// PolygonModel / Field3D / Sprite2D が共有していたシェーダーを解放
	Texture::Finalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		g_Camera.Update();
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr)
			{
				gameObject->Update();

				g_Camera.SetCameraTarget(g_GameObjects[0]->GetPosition());	//カメラの注視点を更新
				g_Camera.Update();
			}
		}

	}
	// 共通ライト（g_Light）の調整UI
	ImGui::Begin(u8"スポットライト###SPOT LIGHT");
	{
		ImGui::ColorEdit3(u8"拡散光##Diffuse", &g_Light.Diffuse.x);
		ImGui::DragFloat3(u8"向き##Direction", &g_Light.Direction.x, 0.01f);
		ImGui::DragFloat4(u8"位置##Position", &g_Light.Position.x, 0.1f);
		ImGui::DragFloat4(u8"点光源パラメータ##PointLightParam", &g_Light.PointLightParam.x, 0.1f);
		float angle = XMConvertToDegrees(g_Light.Angle.x);	// ラジアン → 度に変換して表示
		ImGui::SliderFloat(u8"照射角##ConeAngle", &angle, 5.0f, 45.0f, "%.1f");
		g_Light.Angle.x = XMConvertToRadians(angle);
	}
	ImGui::End();

}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	//===== 3D描画 =====
	//=====1.3DをRT0へ==========（MipMapSprite などが RT0 を参照する）
	Renderer::BeginPe(0);
	{
		Draw3DObjects();
	}

	if (IsGaussianEnabled())
	{
		//=====2.RT0を横ブラーしてRT1へ==========
		Renderer::BeginPe(1);
		{
			Renderer::SetWorldViewProjection2D();
			g_Gaussian->DrawPass(0);		//0:横ブラー	1:縦ブラー
		}

		//=====3.RT1を縦ブラーしてバックバッファへ==========
		Renderer::Clear();
		{
			Renderer::SetWorldViewProjection2D();
			g_Gaussian->DrawPass(1);		//0:横ブラー	1:縦ブラー
		}
	}
	else
	{
		//=====2.ブラーなし：3Dをバックバッファへ直接描画==========
		Renderer::Clear();
		{
			Draw3DObjects();
		}
	}

	//=====4.2D描画==========
	Renderer::SetWorldViewProjection2D();
	Renderer::SetDepthEnable(false);	//奥行き処理無効
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && gameObject->m_Is2D)
			gameObject->Draw();
	}
}