#include "Main.h"
#include "Renderer.h"
#include "Game.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"

#include "Field3D.h"
#include "PolygonModel.h"
#include "VertexDirectionalLighting.h"
#include "PixelDirectionalLighting.h"
#include "PixelLightingBlinnPhong.h"
#include "HemiSphereLighting.h"
#include "PointPixelLighting.h"
#include "LimLighting.h"
#include "SpotLighting.h"
#include "BumpField3D.h"
#include "CookTorrance.h"
#include "DisneyPBR.h"
#include "Toon1.h"
#include "Toon2.h"
#include "Toon3.h"


//===============================================
//グローバル変数
//===============================================

std::vector<GameObject*> g_GameObjects =
{
	new Camera(),
	new Sprite2D(),
	//new Field3D(),
	new BumpField3D(),
	//new PolygonModel(),
	//new VertexDirectionalLighting(),
	//new PixelDirectionalLighting(),
	//new PixelLightingBlinnPhong(),
	//new HemiSphereLighting(),
	//new PointPixelLighting(),
	//new LimLighting(),
	//new SpotLighting(),
	//new CookTorrance(),
	//new DisneyPBR(),
	new Toon1(),
	new Toon2(),
	new Toon3(),
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

	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
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
	//===== パス1：レンダリングテクスチャへ描画 =====
	BeginPe();		//レンダリングテクスチャをレンダリングターゲットにする（緑クリア）
	{
		SetDepthEnable(true);		//奥行き処理有効
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr && !gameObject->m_Is2D)
				gameObject->Draw();		//Camera→BumpField3D→Toon1 の順で描かれる
		}
	}

	//===== パス2：バックバッファへ描画 =====
	Clear();		//★レンダリングターゲットをデフォルトへ戻す（赤クリア）★
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