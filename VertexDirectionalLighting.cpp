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

	//構造体初期化
	m_Position = XMFLOAT3(0.0f + 0.5 * -0, 0.5f, 0.0f);

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "VertexDirectionalLightingVS.cso");
	CreatePixelShader(&PixelShader, "VertexDirectionalLightingPS.cso");

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
	ID3D11ShaderResourceView* texture = GetTexture(TexID);
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	PolygonModel::Draw();
}