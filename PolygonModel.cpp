#include "PolygonModel.h"
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
void PolygonModel::Init(void)
{
	m_Is2D = false;		//2DオブジェクトフラグOFF

	TexID = TextureLoad(L"asset\\texture\\texture.jpg");

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
	CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");

	//構造体初期化
	m_Position = XMFLOAT3(0.0f, 0.2f, 0.0f);
	m_Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//モデルの読み込み
	Model = ModelLoad("asset\\model\\model.fbx");
	
	//サンプラーの設定
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GetDevice()->CreateSamplerState(&sampDesc, &SamplerState);
};

//=========================================================================================================
// 終了処理
//=========================================================================================================
void PolygonModel::Uninit(void)
{
	VertexLayout->Release();
	VertexShader->Release();
	PixelShader->Release();
	SamplerState->Release();
	ModelRelease(Model);
}

//=========================================================================================================
// 更新処理
//=========================================================================================================
void PolygonModel::Update(void)
{
	if (Keyboard_IsKeyDown(KK_W))
	{
		m_Rotation.x += 0.01f;
	}
	else if(Keyboard_IsKeyDown(KK_S))
	{
		m_Rotation.x -= 0.01f;
	}
}

//=========================================================================================================
// 描画処理
//=========================================================================================================
void PolygonModel::Draw(void)
{
	//
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	{
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		GetDeviceContext()->PSSetSamplers(0, 1, &SamplerState);

		//
		XMMATRIX TranslationMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		//回転行列
		XMMATRIX RotationXMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		//スケーリング行列
		XMMATRIX ScalingMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		//ワールド行列の計算
		XMMATRIX WorldMatrix = ScalingMatrix * RotationXMatrix * TranslationMatrix;
		//
		SetWorldMatrix(WorldMatrix);

		//
		GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//マテリアルの設定
		MATERIAL Material;
		ZeroMemory(&Material, sizeof(MATERIAL));
		Material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(Material);


		LIGHT light;
		ZeroMemory(&light, sizeof(light));
		light.Enable = true;
		light.Direction = XMFLOAT4(0.0f, -1.0f, 1.0f, 0.0f);
		light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		SetLight(light);

		//描画
		ModelDraw(Model);
	}
}