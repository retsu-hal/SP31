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


//===============================================
//グローバル変数
//===============================================

// PolygonModel / Field3D / Sprite2D の第1引数はマテリアル名（MaterialTable.cpp に定義）
// 実行中は ImGui の Inspecter → Material で切り替えられる
// ※カメラは GameObject ではないので InitCamera などを直接呼ぶ
std::vector<GameObject*> g_GameObjects =
{
	//new Sprite2D("UnlitColor"),
	new PolygonModel("SpotLighting",  XMFLOAT3(0.0f, 0.5f, 0.0f)),
	new Field3D("UnlitTexture", XMFLOAT3(0.0f, 0.0f, 0.0f)),
	//new MipMapSprite("UnlitTexture"),
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
	ImGui::Begin("SPOT LIGHT");
	{
		ImGui::ColorEdit3("Diffuse", &g_Light.Diffuse.x);
		ImGui::DragFloat3("Direction", &g_Light.Direction.x, 0.01f);
		ImGui::DragFloat4("Position", &g_Light.Position.x, 0.1f);
		ImGui::DragFloat4("PointLightParam", &g_Light.PointLightParam.x, 0.1f);

		float angle = XMConvertToDegrees(g_Light.Angle.x);
		ImGui::SliderFloat("Cone Angle", &angle, 5.0f, 45.0f, "%.1f");
		g_Light.Angle.x = XMConvertToRadians(angle);

		
	}
	ImGui::End();

}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	//===== 3D描画 =====
	{
		Renderer::BeginPe();
		{
			g_Camera.Draw();				//ビュー・プロジェクション行列をセット
			Renderer::SetDepthEnable(true);		//奥行き処理有効
			for (GameObject* gameObject : g_GameObjects)
			{
				if (gameObject != nullptr && !gameObject->m_Is2D)
					gameObject->Draw();
			}
		}
		Renderer::Clear();	//レンダーターゲットをクリア

		{
			g_Camera.Draw();				//ビュー・プロジェクション行列をセット
			Renderer::SetDepthEnable(true);		//奥行き処理有効
			for (GameObject* gameObject : g_GameObjects)
			{
				if (gameObject != nullptr && !gameObject->m_Is2D)
					gameObject->Draw();
			}
		}
	}

	//===== 2D描画 =====
	{
		Renderer::SetWorldViewProjection2D();
		Renderer::SetDepthEnable(false);
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr && gameObject->m_Is2D)
				gameObject->Draw();
		}
	}
}