/*==============================================================================

    [Field3D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "Field3D.h"
#include "sprite.h"

#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	NUM_VERTEX	(4)
#define	SIZE	(5)

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//頂点データ
/*
      V0------------------V1
      |                  |
     |                  |
    |                  |
   V2-----------------V3
*/

static VERTEX_3D	Box[NUM_VERTEX] =
{
	{//頂点V0 LEFT-TOP
		XMFLOAT3(-SIZE/2, 0.0f, SIZE/2),	//頂点座標
		XMFLOAT3(0.0f,0.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(0.0f*SIZE,0.0f*SIZE)				//テクスチャ座標
	},
	{//頂点V1 RIGHT-TOP
		XMFLOAT3(SIZE / 2, 0.0f, SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,0.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(1.0f* SIZE,0.0f* SIZE)				//テクスチャ座標
	},
	{//頂点V2 LEFT-BOTTOM
		XMFLOAT3(-SIZE / 2, 0.0f, -SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,0.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(0.0f* SIZE,1.0f* SIZE)				//テクスチャ座標
	},

	{//頂点V3 RIGHT-BOTTOM
		XMFLOAT3(SIZE / 2, 0.0f, -SIZE / 2),		//頂点座標
		XMFLOAT3(0.0f,0.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(1.0f* SIZE,1.0f* SIZE)				//テクスチャ座標
	},

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Field3D::Init(void)
{

	//テクスチャ読み込み
	TexID = TextureLoad(L"asset\\texture\\sura.jpg");

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "shader/UnlitTextureVS.cso");
	CreatePixelShader(&PixelShader, "shader/UnlitTexturePS.cso");

	//頂点バッファ作成
	{
		D3D11_BUFFER_DESC	bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		GetDevice()->CreateBuffer(&bd, NULL, &VertexBuffer);

		//頂点バッファの書き込み先のポインターを取得
		D3D11_MAPPED_SUBRESOURCE	msr;
		GetDeviceContext()->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//頂点データをコピー
		CopyMemory(&vertex[0], &Box[0], sizeof(VERTEX_3D) * NUM_VERTEX);
		//書き込み完了
		GetDeviceContext()->Unmap(VertexBuffer, 0);
	}

	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Field3D::Finalize(void)
{
	//作ったものを解放

	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();

	VertexBuffer->Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void Field3D::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void Field3D::Draw(void)
{

	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	{
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列作成
		XMMATRIX	TranslationMatrix =
			XMMatrixTranslation(
				Position.x,
				Position.y,
				Position.z
			);
		//回転行列作成
		XMMATRIX	RotationMatrix =
			XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(Rotate.x),
				XMConvertToRadians(Rotate.y),
				XMConvertToRadians(Rotate.z)
			);
		//スケーリング行列作成
		XMMATRIX	ScalingMatrix =
			XMMatrixScaling(
				Scale.x,
				Scale.y,
				Scale.z
			);
		//ワールド行列作成 ※乗算の順番に注意
		XMMATRIX	WorldMatrix =
			ScalingMatrix *
			RotationMatrix *
			TranslationMatrix;
		//DirectXへセット
		SetWorldMatrix(WorldMatrix);

		//頂点バッファをセット
		UINT	stride = sizeof(VERTEX_3D);
		UINT	offset = 0;
		GetDeviceContext()->IASetVertexBuffers(
			0,
			1,
			&VertexBuffer,
			&stride,
			&offset
		);

		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
		//マテリアル設定
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//描画
		GetDeviceContext()->Draw(NUM_VERTEX, 0);//インデックス無し描画
	}


}
