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

LIGHT g_Light;
//===============================================
//グローバル変数
//===============================================

std::vector<GameObject*> g_GameObjects =
{
	new Camera(),
	new Sprite2D(),
	new Field3D(),
	//new PolygonModel(),
	//new VertexDirectionalLighting(),
	//new PixelDirectionalLighting(),
	//new PixelLightingBlinnPhong(),
	//new HemiSphereLighting(),
	new PointPixelLighting(),
	new LimLighting(),
};

//ポーズフラグ
static	bool	pause = false;

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
	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	XMStoreFloat4(&g_Light.Direction, dir);
	g_Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);	//拡散光の色
	g_Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);	//環境光の色

	dir = XMVector4Normalize(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4(&g_Light.GroundNormal, dir);

	g_Light.SkyColor = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
	g_Light.GroundColor = XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f);
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

}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	// 2D用マトリクス設定
	SetWorldViewProjection2D();
	SetDepthEnable(false);
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && gameObject->m_Is2D)
			gameObject->Draw();
	}
	
	//3D用マトリクス設定
	{
		SetLight(g_Light);
		SetDepthEnable(true);		//奥行き処理有効
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr && !gameObject->m_Is2D)
				gameObject->Draw();
		}
	}
}
