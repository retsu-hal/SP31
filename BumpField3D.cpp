/*==============================================================================

[BumpField3D.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "BumpField3D.h"
#include "texture.h"
#include "imgui/imgui.h"

//==============================================================================
//マクロ宣言
//==============================================================================
#define NUM_VERTEX (4)
#define SIZE (6)

//==============================================================================
//プロトタイプ宣言
//==============================================================================

//==============================================================================
//グローバル変数
//==============================================================================
static VERTEX_3D Box[NUM_VERTEX] =
{
	{
		XMFLOAT3(-SIZE / 2,0.0f,SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(0.0f * SIZE,0.0f * SIZE)
	},

	{
		XMFLOAT3(SIZE / 2,0.0f,SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(1.0f * SIZE,0.0f * SIZE)
	},

	{
		XMFLOAT3(-SIZE / 2,0.0f,-SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(0.0f * SIZE,1.0f * SIZE)
	},

	{
		XMFLOAT3(SIZE / 2,0.0f,-SIZE / 2),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
		XMFLOAT2(1.0f * SIZE,1.0f * SIZE)
	},
};

//==============================================================================
//初期化処理
//==============================================================================
void BumpField3D::Init()
{
	m_Is2D = false;

	// テクスチャ読み込み
	m_TexID = TextureLoad(GetTexturePath());
	m_BumpTexID = TextureLoad(GetBumpTexturePath());

	//シェーダー読み込み
	CreateVertexShader(&m_VertexShader, &m_VertexLayout, GetVertexShaderPath());
	CreatePixelShader(&m_PixelShader, GetPixelShaderPath());

	// 頂点バッファの作成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		GetDevice()->CreateBuffer(&bd, NULL, &m_VertexBuffer);
	}

	//頂点バッファの書き込み先のポインターを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	//頂点データをコピー
	CopyMemory(&vertex[0], &Box[0], sizeof(VERTEX_3D) * NUM_VERTEX);
	//
	GetDeviceContext()->Unmap(m_VertexBuffer, 0);

	//構造体初期化
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// ライト構造体の初期化
	XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	XMStoreFloat4(&m_Light.Direction, dir);//コーンの向き
	m_Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	//拡散光の色
	m_Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);	//環境光の色
	m_Light.SkyColor = XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f);
	m_Light.GroundColor = XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f);
	m_Light.PointLightParam = XMFLOAT4(10.0f, 1.0f, 1.0f, 0.0f);
	m_Light.Angle.x = XMConvertToRadians(30.0f);//コーンの角度
}

//==============================================================================
//終了処理
//==============================================================================
void BumpField3D::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
	m_VertexBuffer->Release();
};

//==============================================================================
//更新処理
//==============================================================================
void BumpField3D::Update()
{
	DrawImGui();
};

//==============================================================================
//描画処理
//==============================================================================
void BumpField3D::Draw()
{
	//
	GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	//
	GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	//
	GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	SetLight(m_Light);
	{
		//
		ID3D11ShaderResourceView* tex = GetTexture(m_TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		tex = GetTexture(m_BumpTexID);
		GetDeviceContext()->PSSetShaderResources(1, 1, &tex);

		//
		XMMATRIX TranslationMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		//
		XMMATRIX RotationXMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
		//	
		XMMATRIX ScalingMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
		// ワールド行列の計算
		XMMATRIX WorldMatrix = ScalingMatrix * RotationXMatrix * TranslationMatrix;
		//
		SetWorldMatrix(WorldMatrix);

		//
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
		//
		GetDeviceContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//
		MATERIAL material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		// 描画
		GetDeviceContext()->Draw(NUM_VERTEX, 0);

	}
};

//==============================================================================
//ImGui描画処理
//==============================================================================
void BumpField3D::DrawImGui(void)
{
	ImGui::Begin("Inspecter");
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(GetName()))
	{
		float w3 = (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;

		ImGui::SeparatorText("Transform");
		ImGui::PushItemWidth(w3);
		ImGui::DragFloat("##PositionX", &m_Position.x, 0.01f, 0.0f, 0.0f, "X:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##PositionY", &m_Position.y, 0.01f, 0.0f, 0.0f, "Y:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##PositionZ", &m_Position.z, 0.01f, 0.0f, 0.0f, "Z:%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Position");

		ImGui::PushItemWidth(w3);
		ImGui::DragFloat("##RotationX", &m_Rotation.x, 0.01f, 0.0f, 0.0f, "X:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##RotationY", &m_Rotation.y, 0.01f, 0.0f, 0.0f, "Y:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##RotationZ", &m_Rotation.z, 0.01f, 0.0f, 0.0f, "Z:%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Rotation");

		ImGui::PushItemWidth(w3);
		ImGui::DragFloat("##ScaleX", &m_Scale.x, 0.01f, 0.0f, 0.0f, "X:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##ScaleY", &m_Scale.y, 0.01f, 0.0f, 0.0f, "Y:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##ScaleZ", &m_Scale.z, 0.01f, 0.0f, 0.0f, "Z:%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Scale");

		ImGui::SeparatorText("Light");
		ImGui::PushItemWidth(w3);
		ImGui::DragFloat("##Light DirectionX", &m_Light.Direction.x, 0.01f, 0.0f, 0.0f, "X:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##Light DirectionY", &m_Light.Direction.y, 0.01f, 0.0f, 0.0f, "Y:%.2f"); ImGui::SameLine();
		ImGui::DragFloat("##Light DirectionZ", &m_Light.Direction.z, 0.01f, 0.0f, 0.0f, "Z:%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine(); ImGui::Text("Light Direction");

		ImGui::ColorEdit3("Diffuse", &m_Light.Diffuse.x);
		ImGui::ColorEdit3("Ambient", &m_Light.Ambient.x);

		ImGui::SeparatorText("Resources");
		ImGui::Text("VertexShader : %s", GetVertexShaderPath());
		ImGui::Text("PixelShader  : %s", GetPixelShaderPath());
		ImGui::Text("Texture      : %ls", GetTexturePath());
		ImGui::Text("BumpTexture  : %ls", GetBumpTexturePath());
	}

	ImGui::PopID();
	ImGui::End();
};