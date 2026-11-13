#include "DoF.h"
#include "sprite.h" // Sprite::Draw

void DoF::Init(void)
{
    Sprite2D::Init(); // "DoF" を m_Material に読み込む

    int idx = FindMaterialIndex("UnlitTexture");
    m_MaterialDebug.Load(&GetMaterialTable()[idx]);

    // 画面全体に表示
    m_Position = XMFLOAT3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f);
    Size = XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DoF::Update(void)
{
    Sprite2D::Update();
}

void DoF::DrawImGuiExtra()
{
    ImGui::Begin("DepthOfField");
    ImGui::SliderFloat("Focus", &m_Parameter.x, 0.0f, 1.0f, "%.4f"); // ピント位置
    ImGui::SliderFloat("DoF", &m_Parameter.y, 0.001f, 0.5f, "%.4f"); // ピント幅
    ImGui::End();
}

void DoF::DrawPass()
{
    ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();

    Renderer::SetParameter(m_Parameter);
    m_Material.Bind();
    Renderer::SetDepthEnable(false);

    // t0:ボケ(RT0) t1:通常(RT2) t2:深度
    ID3D11ShaderResourceView* srv[3] = {
        Renderer::GetPeTexture(2),
        Renderer::GetPeTexture(0),
        Renderer::GetDepthStencil(),
    };
    ctx->PSSetShaderResources(0, 3, srv);

    Renderer::SetWorldMatrix(GetWorldMatrix());
    Sprite::Draw(Size, Color);

    // 次フレームで RT / デプスとして使うので外す
    ID3D11ShaderResourceView* nullSRV[3] = {};
    ctx->PSSetShaderResources(0, 3, nullSRV);
}

void DoF::DrawDepthDebug()
{
    ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();

    Renderer::SetParameter(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)); // UnlitTexture の x はミップレベル
    m_MaterialDebug.Bind();
    Renderer::SetDepthEnable(false);

    ID3D11ShaderResourceView* tex = Renderer::GetDepthStencil();
    ctx->PSSetShaderResources(0, 1, &tex);

    // 画面の 1/3 サイズで左上に
    Renderer::SetWorldMatrix(XMMatrixTranslation(SCREEN_WIDTH / 6.0f, SCREEN_HEIGHT / 6.0f, 0.0f));
    Sprite::Draw(XMFLOAT2(SCREEN_WIDTH / 3.0f, SCREEN_HEIGHT / 3.0f), Color);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);
}