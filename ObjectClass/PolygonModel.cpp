/*==============================================================================

[PolygonModel.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "PolygonModel.h"
#include "texture.h"
#include "model.h"
#include "keyboard.h"
//==============================================================================
//マクロ宣言
//==============================================================================

//==============================================================================
//プロトタイプ宣言
//==============================================================================

//==============================================================================
//グローバル変数
//==============================================================================

//==============================================================================
//初期化処理
//==============================================================================
void PolygonModel::Init(void)
{
	m_Is2D = false;		//2DオブジェクトフラグOFF

	//シェーダー読み込み
	CreateVertexShader(&m_VertexShader, &m_VertexLayout, GetVertexShaderPath());
	CreatePixelShader(&m_PixelShader, GetPixelShaderPath());

	//構造体初期化
	m_Position = XMFLOAT3(0.0f , 0.5f, 0.0f);
	m_Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// テクスチャ読み込み
	m_TexID = TextureLoad(GetTexturePath());

	//モデルの読み込み
	m_Model = ModelLoad(GetModelPath());

	//サンプラーの設定
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	GetDevice()->CreateSamplerState(&sampDesc, &m_SamplerState);


	//ライト構造体の初期化
	XMVECTOR	dir = XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f);
	dir = XMVector3Normalize(dir);
	XMStoreFloat4(&m_Light.Direction, dir);//光のベクトル

	m_Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);//点光源の位置
	m_Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);//光の色
	m_Light.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);//環境光
	m_Light.PointLightParam = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);//環境光
};
//==============================================================================
//終了処理
//===================m_===========================================================
void PolygonModel::Uninit(void)
{
	SafeRelease(m_VertexLayout);
	SafeRelease(m_VertexShader);
	SafeRelease(m_PixelShader);
	SafeRelease(m_SamplerState);
	if (m_Model) { ModelRelease(m_Model); }
}

//==============================================================================
// 更新処理
//==============================================================================
void PolygonModel::Update(void)
{
	float Rotspeed = 0.5f;
	float Movespeed = 0.01f;
	//回転
	if (Keyboard_IsKeyDown(KK_E))		m_Rotation.x += Rotspeed;
	if(Keyboard_IsKeyDown(KK_Q))		m_Rotation.x -= Rotspeed;
	
	//移動
	if(Keyboard_IsKeyDown(KK_W))  	 m_Position.z+= Movespeed;
	if(Keyboard_IsKeyDown(KK_S))		m_Position.z -= Movespeed;
	if(Keyboard_IsKeyDown(KK_A))		m_Position.x -= Movespeed;
	if(Keyboard_IsKeyDown(KK_D))		m_Position.x += Movespeed;

	PolygonModel::DrawImGui();
}

//==============================================================================
// 描画処理
//==============================================================================
void PolygonModel::Draw(void)
{
	// 頂点レイアウト・シェーダー設定
	SetParameter(m_Parameter);

	GetDeviceContext()->IASetInputLayout(m_VertexLayout);
	GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	DrawModel();	
}

// ラベル付き3成分DragFloat行を描画する（X/Y/Zを1行に並べ、末尾にラベル）
template<class Vec>
static void DragFloat3Row(const char* id, const char* label, Vec& v)
{
	float w3 = (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;
	ImGui::PushID(id);
	ImGui::PushItemWidth(w3);
	ImGui::DragFloat("##X", &v.x, 0.01f, 0.0f, 0.0f, "X:%.2f"); ImGui::SameLine();
	ImGui::DragFloat("##Y", &v.y, 0.01f, 0.0f, 0.0f, "Y:%.2f"); ImGui::SameLine();
	ImGui::DragFloat("##Z", &v.z, 0.01f, 0.0f, 0.0f, "Z:%.2f");
	ImGui::PopItemWidth();
	ImGui::PopID();
	ImGui::SameLine(); ImGui::Text("%s", label);
}

void PolygonModel::DrawImGui()
{
	ImGui::Begin("Inspecter");
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(GetName()))
	{
				ImGui::SeparatorText("Transform");
		DragFloat3Row("Position", "Position", m_Position);
		DragFloat3Row("Rotation", "Rotation", m_Rotation);
		DragFloat3Row("Scale",    "Scale",    m_Scale);

		ImGui::SeparatorText("Light");
		DragFloat3Row("LightDirection", "Light Direction", m_Light.Direction);
		DragFloat3Row("LightPosition",  "Light Position",  m_Light.Position);

		ImGui::ColorEdit3("Diffuse", &m_Light.Diffuse.x);
		ImGui::ColorEdit3("Ambient", &m_Light.Ambient.x);

		DragFloat3Row("PointLightParam", "Point Light Param", m_Light.PointLightParam);
		
		ImGui::SeparatorText("Resources");
		ImGui::Text("VertexShader: %s", GetVertexShaderPath());
		ImGui::Text("PixelShader : %s", GetPixelShaderPath());
		ImGui::Text("Texture     : %ls", GetTexturePath());
		ImGui::Text("Model       : %s", GetModelPath());

		DrawImGuiExtra();
	}

	ImGui::PopID();
	ImGui::End();
}


void PolygonModel::DrawModel(void)
{
	SetLight(m_Light);

	//テクスチャをセット
	ID3D11ShaderResourceView* tex = GetTexture(m_TexID);
	GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	//ワールド行列作成（※中身は今のまま）
	SetWorldMatrix(GetWorldMatrix());

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	MATERIAL material;
	ZeroMemory(&material, sizeof(MATERIAL));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	ModelDraw(m_Model);
}