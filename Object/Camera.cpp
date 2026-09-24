
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"

Camera		g_Camera;	//カメラ管理構造体


void	InitCamera()
{
	//カメラの初期化
	g_Camera.Position = XMFLOAT3(0.0f, 1.0f, -1.5f);	//カメラ基本座標
	g_Camera.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);		//カメラの上方ベクトル
	g_Camera.AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);	//カメラの注視点座標
	g_Camera.Fov = 45.0f;								//画角
	g_Camera.Nearclip = 0.5f;							//近面クリップ
	g_Camera.Farclip = 1000.0f;							//遠面クリップ
	g_Camera.Rotation = 0.0f;
	g_Camera.Offset = XMFLOAT3(0.0f, 1.0f, -1.5f);

	g_Camera.Yaw = 0.0f;
	g_Camera.Pitch = 30.0f;	// 少し上から見下ろす
	g_Camera.Distance = 1.5f;
}

void	FinalizeCamera()
{

}

void	UpdateCamera()
{
	float speed = 1.0f;	// 1フレームに回す角度（度）

	if (Keyboard_IsKeyDown(KK_LEFT))	g_Camera.Yaw += speed;
	if (Keyboard_IsKeyDown(KK_RIGHT))	g_Camera.Yaw -= speed;
	if (Keyboard_IsKeyDown(KK_UP))		g_Camera.Pitch += speed;
	if (Keyboard_IsKeyDown(KK_DOWN))	g_Camera.Pitch -= speed;

	// 真上・真下まで行くと LookAt が壊れるので制限する
	if (g_Camera.Pitch > 80.0f) g_Camera.Pitch = 80.0f;
	if (g_Camera.Pitch < -10.0f) g_Camera.Pitch = -10.0f;

	float yaw = XMConvertToRadians(g_Camera.Yaw);
	float pitch = XMConvertToRadians(g_Camera.Pitch);

	// 角度と距離からオフセットを作る（Yaw=0 のとき注視点の真後ろ＝-Z側）
	g_Camera.Offset.x = -sinf(yaw) * cosf(pitch) * g_Camera.Distance;
	g_Camera.Offset.y = sinf(pitch) * g_Camera.Distance;
	g_Camera.Offset.z = -cosf(yaw) * cosf(pitch) * g_Camera.Distance;

	g_Camera.Position.x = g_Camera.AtPosition.x + g_Camera.Offset.x;
	g_Camera.Position.y = g_Camera.AtPosition.y + g_Camera.Offset.y;
	g_Camera.Position.z = g_Camera.AtPosition.z + g_Camera.Offset.z;
}

void	DrawCamera()//3D使用時
{
	//プロジェクション行列を作成
	XMMATRIX	ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(g_Camera.Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			g_Camera.Nearclip,
			g_Camera.Farclip
		);
	//プロジェクション行列をセット
	SetProjectionMatrix(ProjectionMatrix);

	//カメラ行列を作成
	XMVECTOR	eyev = XMLoadFloat3(&g_Camera.AtPosition);
	XMVECTOR	pos = XMLoadFloat3(&g_Camera.Position);
	XMVECTOR	up = XMLoadFloat3(&g_Camera.UpVector);
	XMMATRIX	ViewMatrix =
		XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	SetViewMatrix(ViewMatrix);
	//カメラ座標セット
	SetCameraPosition(g_Camera.Position);
}

void SetCameraTarget(XMFLOAT3 target)
{
	g_Camera.AtPosition = target;
}




