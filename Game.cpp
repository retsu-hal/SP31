#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "keyboard.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"

#include "Field3D.h"
#include "PolygonModel.h"
#include "VertexDirectionalLighting.h"
#include "PixelDirectionalLighting.h"


//===============================================
//グローバル変数
//===============================================

std::vector<GameObject*> g_GameObjects =
{
	new Camera(),
	new Sprite2D(),
	new Field3D(),
	new PolygonModel(),
	new VertexDirectionalLighting(),
	new PixelDirectionalLighting(),
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
	//3D用マトリクス設定
	{
		SetDepthEnable(true);		//奥行き処理有効
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr && !gameObject->m_Is2D)
				gameObject->Draw();
		}
	}
	// 2D用マトリクス設定
	SetWorldViewProjection2D();
	SetDepthEnable(false);
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && gameObject->m_Is2D)
			gameObject->Draw();
	}
	
}
