#include "VertexDirectionalLighting.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include "model.h"
#include "keyboard.h"
//=========================================================================================================
// 〇〇
//=========================================================================================================



//=========================================================================================================
// 初期化処理
//=========================================================================================================
void VertexDirectionalLighting::Init(void)
{
	PolygonModel::Init();

	m_Is2D = false;		//2DオブジェクトフラグOFF

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "VertexDirectionalLightingVS.cso");
	CreatePixelShader(&PixelShader, "VertexDirectionalLightingPS.cso");

	//モデルの位置
	m_Position = XMFLOAT3(0.5f, 0.2f, 0.0f);

	// テクスチャ読み込み
	TexID = TextureLoad(L"asset\\texture\\sura.jpg");

	//モデルの読み込み
	m_Model = ModelLoad("asset\\model\\model.fbx");
};

//=========================================================================================================
// 終了処理
//=========================================================================================================
void VertexDirectionalLighting::Uninit(void)
{
	PolygonModel::Uninit();
}

//=========================================================================================================
// 更新処理
//=========================================================================================================
void VertexDirectionalLighting::Update(void)
{
	PolygonModel::Update();
}

//=========================================================================================================
// 描画処理
//=========================================================================================================
void VertexDirectionalLighting::Draw(void)
{
	PolygonModel::Draw();
}