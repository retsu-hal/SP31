#include "Main.h"
#include "Renderer.h"
#include "Manager.h"
#include "Sprite.h"
#include "Game.h"
#include "keyboard.h"
#include "Manager.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"


//===============================================
//グローバル変数
 
Camera		CameraObject;
Sprite2D	test2D;


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
	InitCamera();

	test2D.Init();
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
	FinalizeCamera();
	test2D.Finalize();

	TextureFinalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		UpdateCamera();
		test2D.Update();

	}

}

//===============================================
//ゲームシーン描画
void DrawGame()
{

	// 2D用マトリクス設定
	{
		SetWorldViewProjection2D();
		test2D.Draw();
	}
	//3D用マトリクス設定
	{
		SetDepthEnable(true);		//奥行き処理有効
		DrawCamera();
	}
}
