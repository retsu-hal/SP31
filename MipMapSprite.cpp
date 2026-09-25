#include "MipMapSprite.h"
#include "Game.h"		// g_Light
#include "sprite.h"		// DrawSprite

MipMapSprite::MipMapSprite(const char* materialName, int peNo)
	: Sprite2D(materialName)	// テクスチャパスは既定値のまま（Initでの読み込み用）
	, m_PeNo(peNo)
{}

//=============================================================================
// 初期化
//=============================================================================
void MipMapSprite::Init(void)
{
	Sprite2D::Init();	// マテリアル・シェーダーの読み込みは親に任せる

	// 画面の1/3サイズで左上に表示
	m_Position = XMFLOAT3(SCREEN_WIDTH / 3 / 2, SCREEN_HEIGHT / 3 / 2, 0.0f);
	Size = XMFLOAT2(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 3);

	m_Parameter = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);	// x = ミップレベル
}

//=============================================================================
// 更新
//=============================================================================
void MipMapSprite::Update(void)
{
	Sprite2D::Update();	// Inspecter（位置・サイズ・マテリアル切替）

	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
	ImGui::Begin("MIPMAP");
	{
		ImGui::SliderFloat("MipMap Level", &m_Parameter.x, 0.0f, 7.0f, "%.0f");
	}
	ImGui::End();
}

//=============================================================================
// 描画
//=============================================================================
void MipMapSprite::Draw(void)
{
	const MaterialDesc& desc = GetDesc();
	if (desc.UseGlobalLight)
	{
		m_Light = g_Light;
	}

	Renderer::SetParameter(m_Parameter);	// Parameter.x = ミップレベル → シェーダーの b6 へ
	m_Material.Bind();
	Renderer::SetLight(m_Light);

	Renderer::SetDepthEnable(false);

	//レンダリングテクスチャをセット
	ID3D11ShaderResourceView* tex = Renderer::GetPeTexture(m_PeNo);
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	Renderer::SetWorldMatrix(GetWorldMatrix());

	//ミップマップ作成（テクスチャへの書き込みが終わった後・描画の前）
	Renderer::GetDeviceContext()->GenerateMips(tex);

	Sprite::Draw(Size, Color);
}