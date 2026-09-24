/*==============================================================================

   頂点管理 [Sprite2D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Sprite2D.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

//
//struct  TEST
//{
//	int		IntData;	//4バイト
//	char	CharData;	//1バイト
//	float	FloatData;	//4バイト
//};
//
//TEST	test;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Sprite2D::Init(void)
{

	//char	txt[256] = { 0 };
	//sprintf(txt, "&IntData   => %p \n", &test.IntData);
	//OutputDebugString(txt);
	//sprintf(txt, "&CharData  => %p \n", &test.CharData);
	//OutputDebugString(txt);
	//sprintf(txt, "&FloatData => %p \n", &test.FloatData);
	//OutputDebugString(txt);



	// テクスチャ読み込み
	TexID = TextureLoad(L"asset\\texture\\texture.jpg");

	//シェーダー読み込み
//	CreateVertexShader(&VertexShader, &VertexLayout, "shader/UnlitColorVS.cso");
//	CreatePixelShader(&PixelShader, "shader/UnlitColorPS.cso");
	CreateVertexShader(&VertexShader, &VertexLayout, "shader/GrayscaleVS.cso");
	CreatePixelShader(&PixelShader, "shader/GrayscalePS.cso");

	//2Dオブジェクト初期化
	Position = XMFLOAT3(SCREEN_WIDTH / 3 / 2, SCREEN_WIDTH / 3 / 2, 0.0f);
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Scale = XMFLOAT2(1.0f, 1.0f);
	Size = XMFLOAT2(SCREEN_WIDTH / 3, SCREEN_WIDTH / 3);
	Rotate = 0.0f;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Sprite2D::Finalize(void)
{
	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();

}

//=============================================================================
// 更新処理
//=============================================================================
void Sprite2D::Update(void)
{


}

//=============================================================================
// 描画処理
//=============================================================================
void Sprite2D::Draw(void)
{

	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	//奥行き処理をOFF
	SetDepthEnable(false);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{//2Dポリゴン1枚ずつで必要な処理

		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列の作成（表示座標を決める）
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(
			Position.x, Position.y, 0.0f);

		//回転行列（Z回転）行列の作成
		XMMATRIX	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(Rotate));

		//スケーリング行列作成（倍率1.0が等倍、0倍はダメ！）
		XMMATRIX	ScalingMatrix = XMMatrixScaling(Scale.x,Scale.y, 1.0f);

		//ワールド行列の作成（ポリゴンの表示の仕方を指定する最終的な行列
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//ワールド行列をDirectXへセット
		SetWorldMatrix(WorldMatrix);

		// ポリゴン描画
		DrawSprite(Size, Color);
	}


}
