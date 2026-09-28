#include "Bloom.h"
#include "sprite.h"		// Sprite::Draw

void Bloom::Init(void)
{
	Sprite2D::Init();	// "BloomLuminance" を m_Material に読み込む（m_Parameter の初期値もここで入る）

	int idx = FindMaterialIndex("BloomComposite");
	m_MaterialComposite.Load(&GetMaterialTable()[idx]);

	// 画面全体に表示
	m_Position = XMFLOAT3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f);
	Size = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Bloom::Update(void)
{
	Sprite2D::Update();
}

void Bloom::DrawImGuiExtra()
{
	ImGui::Begin("BLOOM");
	{
		ImGui::SliderFloat(u8"しきい値##Luminance", &m_Parameter.x, 0.0f, 1.0f, "%.4f");
		ImGui::SliderFloat(u8"小さな滲み(Lv2.5)##Blend1", &m_Parameter.y, 0.0f, 2.0f, "%.4f");
		ImGui::SliderFloat(u8"中くらい(Lv4.5)##Blend2", &m_Parameter.z, 0.0f, 2.0f, "%.4f");
		ImGui::SliderFloat(u8"大きな光(Lv6.5)##Blend3", &m_Parameter.w, 0.0f, 2.0f, "%.4f");
	}
	ImGui::End();
}

void Bloom::DrawPass(int pass)
{
	ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();

	Renderer::SetParameter(m_Parameter);

	if (pass == 0) m_Material.Bind();			// 輝度抽出
	else           m_MaterialComposite.Bind();	// 加算合成

	Renderer::SetDepthEnable(false);

	// t0 : 通常シーン（RT0）
	ID3D11ShaderResourceView* tex = Renderer::GetPeTexture(0);
	ctx->PSSetShaderResources(0, 1, &tex);

	if (pass == 1)
	{
		// t1 : 輝度マップ（RT1）。パス2で描き終わった直後なのでここでミップを作る
		tex = Renderer::GetPeTexture(1);
		ctx->GenerateMips(tex);
		ctx->PSSetShaderResources(1, 1, &tex);
	}

	Renderer::SetWorldMatrix(GetWorldMatrix());
	Sprite::Draw(Size, Color);

	// t0, t1 を外す（次フレームの BeginPe で RT として使うため）
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	ctx->PSSetShaderResources(0, 2, nullSRV);
}