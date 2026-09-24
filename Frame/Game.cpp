#include "Main.h"
#include "Renderer.h"
#include "Game.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"
#include "Field3D.h"
#include "PolygonModel.h"
#include "Material.h"


//===============================================
//グローバル変数
//===============================================

// PolygonModel / Field3D / Sprite2D の第1引数はマテリアル名（MaterialTable.cpp に定義）
// 実行中は ImGui の Inspecter → Material で切り替えられる
// ※カメラは GameObject ではないので InitCamera などを直接呼ぶ
std::vector<GameObject*> g_GameObjects =
{
	//new Sprite2D("UnlitColor", L"asset\\texture\\texture.jpg"),
	new PolygonModel("UnlitTexture",  XMFLOAT3(0.0f, 0.5f, 0.0f)),
	new Field3D("UnlitTexture", XMFLOAT3(0.0f, 0.0f, 0.0f)),
};

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
//ゲームシーン初期化
void InitGame()
{
	TextureInitialize(GetDevice());
	InitCamera();
	
	for(GameObject	*GameObj:g_GameObjects)
	{
		if (GameObj != nullptr)
		{
			GameObj->Init();
		}
	}


	// ライト構造体の初期化
	g_Light = MakeDefaultLight();
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Uninit();
			delete gameObject;
		}
	}
	g_GameObjects.clear();

	FinalizeCamera();
	ReleaseShaderCache();	// PolygonModel / Field3D / Sprite2D が共有していたシェーダーを解放
	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		UpdateCamera();
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr)
			{
				gameObject->Update();
			}
		}

	}
	// 共通ライト（g_Light）の調整UI
	/*ImGui::Begin("SPOT LIGHT");
	{
		ImGui::ColorEdit3("Diffuse", &g_Light.Diffuse.x);
		ImGui::DragFloat3("Direction", &g_Light.Direction.x, 0.01f);
		ImGui::DragFloat4("Position", &g_Light.Position.x, 0.1f);
		ImGui::DragFloat4("PointLightParam", &g_Light.PointLightParam.x, 0.1f);

		float angle = XMConvertToDegrees(g_Light.Angle.x);
		ImGui::SliderFloat("Cone Angle", &angle, 5.0f, 45.0f, "%.1f");
		g_Light.Angle.x = XMConvertToRadians(angle);

		
	}
	ImGui::End();*/

}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	//===== 3D描画 =====（バックバッファのクリアは main.cpp の Draw で行う）
	{
		DrawCamera();				//ビュー・プロジェクション行列をセット
		SetDepthEnable(true);		//奥行き処理有効
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr && !gameObject->m_Is2D)
				gameObject->Draw();
		}
	}

	//===== 2D描画 =====
	{
		SetWorldViewProjection2D();
		SetDepthEnable(false);
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr && gameObject->m_Is2D)
				gameObject->Draw();
		}
	}
}