#include "Main.h"
#include "Renderer.h"
#include "Game.h"
#include "Camera.h"
#include "texture.h"

#include	"Sprite2D.h"
#include "Field3D.h"
#include "PolygonModel.h"
#include "Material.h"
#include "MipMapSprite.h"
#include "Horror.h"
#include "Gaussian.h"
#include "Bloom.h"
#include "DoF.h"

#include <algorithm>

//===============================================
//グローバル変数
//===============================================

// PolygonModel / Field3D / Sprite2D の第1引数はマテリアル名（MaterialTable.cpp に定義）
// 実行中は ImGui の Inspecter → Material で切り替えられる
// ※カメラは GameObject ではないので InitCamera などを直接呼ぶ
Gaussian* g_Gaussian = new Gaussian();
Bloom* g_Bloom = new Bloom();
DoF* g_DoF = new DoF();


std::vector<GameObject*> g_GameObjects =
{

	new PolygonModel("UnlitTexture",  XMFLOAT3(0.0f, 0.5f, 0.0f)),
    //new PolygonModel("UnlitTexture", XMFLOAT3(0.6f, 0.5f, 3.0f)),
	new Field3D("ProjectionShadow", XMFLOAT3(0.0f, 0.0f, 0.0f)),	// 投影シャドウを受ける地面
	//new Horror("Horror"),
	//new Sprite2D("UnlitTexture"),
	//g_Gaussian,
	//g_Bloom,
	//g_DoF,
	//new MipMapSprite("UnlitTexture"),
};

Camera g_Camera;	//カメラ
//ポーズフラグ
static	bool	pause = false;

LIGHT g_Light;
LIGHT g_ShadowLight;	// 投影シャドウ用ライト（普通のポイントライト＋ライトカメラ行列）
//===============================================
//ポーズフラグセット
void	SetPause(bool flg)
{
	pause = flg;
}
//===============================================
//ポーズフラグ取得
bool	GetPause()
{
	return pause;
}

//===============================================
//g_Gaussian が g_GameObjects に登録されているか
//（コメントアウトされていればブラーなしで描画する）
static bool IsGaussianEnabled()
{
	return std::find(g_GameObjects.begin(), g_GameObjects.end(), g_Gaussian) != g_GameObjects.end();
}

static bool IsBloomEnabled()
{
	return std::find(g_GameObjects.begin(), g_GameObjects.end(), g_Bloom) != g_GameObjects.end();
}

static bool IsDoFEnabled()
{
    return std::find(g_GameObjects.begin(), g_GameObjects.end(), g_DoF) != g_GameObjects.end();
}

//===============================================
//3Dオブジェクトの描画
static void Draw3DObjects()
{
	g_Camera.Draw();				//ビュー・プロジェクション行列をセット
	Renderer::SetDepthEnable(true);		//奥行き処理有効
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && !gameObject->m_Is2D)
			gameObject->Draw();
	}
}

//===============================================
//シャドウマップ作成（ライトカメラから見た影をRT_SHADOWへ描く）
static void DrawShadowMap()
{
	//t1 にシャドウマップが残っているとRTとして使えないので外す
	ID3D11ShaderResourceView* nullSRV = nullptr;
	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &nullSRV);

	Renderer::BeginPe(RT_SHADOW);	//白でクリア
	Renderer::SetDepthEnable(true);
	Renderer::SetViewMatrix(g_ShadowLight.ViewMatrix);				//ライトカメラ行列セット
	Renderer::SetProjectionMatrix(g_ShadowLight.ProjectionMatrix);	//ライトプロジェクション行列セット
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && !gameObject->m_Is2D)
			gameObject->DrawShadow();	//影を落とすもの（PolygonModel）だけ描かれる
	}
}

//===============================================
//ゲームシーン初期化
void InitGame()
{
	Texture::Initialize(Renderer::GetDevice());
	g_Camera.Init();
	
	for(GameObject	*GameObj:g_GameObjects)
	{
		if (GameObj != nullptr)
		{
			GameObj->Init();
		}
	}


	// ライト構造体の初期化
	g_Light = MakeDefaultLight();

	//スポットライト用
	XMVECTOR dir = XMVector4Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
	XMStoreFloat4(&g_Light.Direction, dir);						//コーンの向き
	g_Light.Position = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	g_Light.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.PointLightParam = XMFLOAT4(10.0f, 1.0f, 0.0f, 0.0f);	//x:距離 y:POW
	g_Light.Angle.x = XMConvertToRadians(30.0f);					//コーン角度

	//投影シャドウ用ライト（普通のポイントライトでOK）
	g_ShadowLight = MakeDefaultLight();
	XMVECTOR sdir = XMVector4Normalize(XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f));
	XMStoreFloat4(&g_ShadowLight.Direction, sdir);
	g_ShadowLight.Position = XMFLOAT4(2.0f, 2.0f, 0.0f, 0.0f);
	g_ShadowLight.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_ShadowLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	g_ShadowLight.PointLightParam = XMFLOAT4(2000.0f, 1.5f, 0.0f, 0.0f);//減衰率
}

//===============================================
//ゲームシーン終了
void FinalizeGame()
{
    if (!IsDoFEnabled())
    {
        delete g_DoF;
    }
    g_DoF = nullptr;

	//未登録の g_Gaussian はここで解放（登録されていれば下のループで解放される）
	if (!IsGaussianEnabled())
	{
		delete g_Gaussian;
	}
	g_Gaussian = nullptr;

	if (!IsBloomEnabled())
	{
		delete g_Bloom;
	}
	g_Bloom = nullptr;

	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Uninit();
			delete gameObject;
		}
	}
	g_GameObjects.clear();

	g_Camera.Finalize();
	ReleaseShaderCache();	// PolygonModel / Field3D / Sprite2D が共有していたシェーダーを解放
	Texture::Finalize();
}

//===============================================
//ゲームシーン更新
void UpdateGame()
{

	if (GetPause() == false)//ポーズ中でなければ更新実行
	{
		g_Camera.Update();
		for (GameObject* gameObject : g_GameObjects)
		{
			if (gameObject != nullptr)
			{
				gameObject->Update();

				g_Camera.SetCameraTarget(g_GameObjects[0]->GetPosition());	//カメラの注視点を更新
				g_Camera.Update();
			}
		}

	}
	// 共通ライト（g_Light）の調整UI
	ImGui::Begin(u8"スポットライト###SPOT LIGHT");
	{
		ImGui::ColorEdit3(u8"拡散光##Diffuse", &g_Light.Diffuse.x);
		ImGui::DragFloat3(u8"向き##Direction", &g_Light.Direction.x, 0.01f);
		ImGui::DragFloat4(u8"位置##Position", &g_Light.Position.x, 0.1f);
		ImGui::DragFloat4(u8"点光源パラメータ##PointLightParam", &g_Light.PointLightParam.x, 0.1f);
		float angle = XMConvertToDegrees(g_Light.Angle.x);	// ラジアン → 度に変換して表示
		ImGui::SliderFloat(u8"照射角##ConeAngle", &angle, 5.0f, 45.0f, "%.1f");
		g_Light.Angle.x = XMConvertToRadians(angle);
	}
	ImGui::End();

	// 投影シャドウ用ライトの位置調整＋ライトカメラ行列の更新
	ImGui::Begin("ProjectionShadow");
	{
		ImGui::SliderFloat("LightPositionX", &g_ShadowLight.Position.x, -3.0f, 3.0f, "%.4f");
		ImGui::SliderFloat("LightPositionY", &g_ShadowLight.Position.y, 1.0f, 5.0f, "%.4f");
		ImGui::SliderFloat("LightPositionZ", &g_ShadowLight.Position.z, -3.0f, 3.0f, "%.4f");

		//シャドウマップの確認用（小さく表示）
		ID3D11ShaderResourceView* shadowMap = Renderer::GetPeTexture(RT_SHADOW);
		ImGui::Image((ImTextureID)(intptr_t)shadowMap, ImVec2(SCREEN_WIDTH / 6.0f, SCREEN_HEIGHT / 6.0f));
	}
	ImGui::End();

	XMVECTOR lpos = XMVectorSet(g_ShadowLight.Position.x, g_ShadowLight.Position.y, g_ShadowLight.Position.z, 0.0f);
	XMVECTOR lat = XMVectorSet(0.001f, 0.0f, 0.001f, 0.0f);	//真下を向かないよう少しずらす
	XMVECTOR lup = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_ShadowLight.ViewMatrix = XMMatrixLookAtLH(lpos, lat, lup);
	g_ShadowLight.ProjectionMatrix = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f
	);

}

//===============================================
//ゲームシーン描画
void DrawGame()
{
	//===== 0.シャドウマップ作成 =====
	DrawShadowMap();

	//===== 3D描画 =====
if (IsDoFEnabled() && IsGaussianEnabled())
    {
        //=====1.MRTで RT0 と RT2 に3D、深度はデプステクスチャへ=====
        Renderer::BeginDoF();
        {
            Draw3DObjects();
        }
        //=====2.RT0を横ブラーしてRT1へ=====
        Renderer::BeginPe(1);
        {
            Renderer::SetWorldViewProjection2D();
            g_Gaussian->DrawPass(0);
        }
        //=====3.RT1を縦ブラーしてRT0へ（ボケ画像完成）=====
        Renderer::BeginPe(2);
        {
            Renderer::SetWorldViewProjection2D();
            g_Gaussian->DrawPass(1);
        }
        //=====4.深度でRT0とRT2をブレンドしてバックバッファへ=====
        Renderer::Clear();
        {
            Renderer::SetWorldViewProjection2D();
            g_DoF->DrawPass();
            g_DoF->DrawDepthDebug();
        }
    }
	else
	{
		//=====2.ブラーなし：3Dをバックバッファへ直接描画==========
		Renderer::Clear();
		{
			Draw3DObjects();
		}
	}

	//=====4.2D描画==========
	Renderer::SetWorldViewProjection2D();
	Renderer::SetDepthEnable(false);	//奥行き処理無効
	for (GameObject* gameObject : g_GameObjects)
	{
		if (gameObject != nullptr && gameObject->m_Is2D)
			gameObject->Draw();
	}
}