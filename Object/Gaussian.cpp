#include <math.h>
#include "Gaussian.h"
#include "sprite.h"		// Sprite::Draw

void Gaussian::Init(void)
{
	Sprite2D::Init();	// "GaussianH" を m_Material に読み込む

	int v = FindMaterialIndex("GaussianV");
	m_MaterialV.Load(&GetMaterialTable()[v]);

	// 画面全体に表示
	m_Position = XMFLOAT3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f);
	Size = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);

	CalcGaussianWeight(m_Parameter.z);
}

void Gaussian::Update(void)
{
	Sprite2D::Update();
}

// ガウス関数でウェイトを作る（合計が1になるよう正規化）
void Gaussian::CalcGaussianWeight(float dispersion)
{
	m_Weight[0] = 1.0f;			// expf(0) = 1
	float total = m_Weight[0];

	for (int i = 1; i < 8; i++)
	{
		m_Weight[i] = (dispersion > 0.0f) ? expf(-0.5f * (float)(i * i) / dispersion) : 0.0f;
		total += 2.0f * m_Weight[i];	// 左右対称なので2倍
	}
	for (int i = 0; i < 8; i++)
	{
		m_Weight[i] /= total;
	}
}

void Gaussian::DrawImGuiExtra()
{
	ImGui::Begin("Gaussian");
	if (ImGui::SliderFloat("Boke Str", &m_Parameter.z, 0.0f, 50.0f, "%.3f"))
	{
		CalcGaussianWeight(m_Parameter.z);
	}
	ImGui::SliderFloat("Step", &m_Parameter.w, 1.0f, 3.0f, "%.1f");
	ImGui::End();
}

void Gaussian::DrawPass(int pass)
{

	Renderer::SetParameter(m_Parameter);
	Renderer::SetWeight(m_Weight);

	if (pass == 0) m_Material.Bind();	// 横
	else           m_MaterialV.Bind();	// 縦

	Renderer::SetDepthEnable(false);

	ID3D11ShaderResourceView* tex = Renderer::GetPeTexture(pass);	// pass0→RT0, pass1→RT1
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

	Renderer::SetWorldMatrix(GetWorldMatrix());
	Sprite::Draw(Size, Color);

	// 描き終わったら t0 を外す（次の BeginPe で同じRTを書き込み先にするため）
	ID3D11ShaderResourceView* nullSRV = nullptr;
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &nullSRV);
}