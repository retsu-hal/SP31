#include <stdlib.h>		// rand
#include "Horror.h"
#include "Game.h"		// g_Light
#include "sprite.h"		// Sprite::Draw

Horror::Horror(const char* materialName, int peNo)
	: Sprite2D(materialName)
	, m_PeNo(peNo)
{}

//=============================================================================
// 初期化
//=============================================================================
void Horror::Init(void)
{
	Sprite2D::Init();	// マテリアル・シェーダーの読み込みは親に任せる

	// 画面全体に表示（位置はスプライトの中心）
	m_Position = XMFLOAT3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f);
	Size = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);

	m_Parameter = XMFLOAT4(0.0f, 0.2f, 0.7f, 1.0f);	// x:Seed y:MIN z:MAX w:POW
}

//=============================================================================
// 更新
//=============================================================================
void Horror::Update(void)
{
	Sprite2D::Update();	// Inspecter

	// Seed値：毎フレーム 0.0〜1.0 のランダム値を加算
	m_Parameter.x += ((float)rand() / RAND_MAX);
	if (m_Parameter.x > 100.0f)
	{
		m_Parameter.x -= 100.0f;
	}

	ImGui::Begin("Horror");
	{
		ImGui::SliderFloat("MIN", &m_Parameter.y, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("MAX", &m_Parameter.z, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("POW", &m_Parameter.w, 1.0f, 30.0f, "%.0f");
	}
	ImGui::End();
}

//=============================================================================
// 描画
//=============================================================================
void Horror::Draw(void)
{
	const MaterialDesc& desc = GetDesc();
	if (desc.UseGlobalLight)
	{
		m_Light = g_Light;
	}

	Renderer::SetParameter(m_Parameter);	// b6 の Parameter へ
	m_Material.Bind();
	Renderer::SetLight(m_Light);

	Renderer::SetDepthEnable(false);

	// レンダリングテクスチャをセット
	ID3D11ShaderResourceView* tex = Renderer::GetPeTexture(m_PeNo);
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	Renderer::SetWorldMatrix(GetWorldMatrix());

	Sprite::Draw(Size, Color);
}