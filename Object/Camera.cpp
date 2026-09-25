
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"

void	Camera::Init()
{
	//カメラの初期化
	m_Position = XMFLOAT3(0.0f, 1.0f, -1.5f);	//カメラ基本座標
	m_UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);		//カメラの上方ベクトル
	m_AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);	//カメラの注視点座標
	m_Fov = 45.0f;								//画角
	m_Nearclip = 0.5f;							//近面クリップ
	m_Farclip = 1000.0f;							//遠面クリップ
	m_Rotation = 0.0f;
	m_Offset = XMFLOAT3(0.0f, 1.0f, -1.5f);

	m_Yaw = 0.0f;
	m_Pitch = 30.0f;	// 少し上から見下ろす
	m_Distance = 1.5f;
}

void	Camera::Finalize()
{

}

void	Camera::Update()
{
	float speed = 1.0f;	// 1フレームに回す角度（度）

	if (Keyboard_IsKeyDown(KK_LEFT))	m_Yaw += speed;
	if (Keyboard_IsKeyDown(KK_RIGHT))	m_Yaw -= speed;
	if (Keyboard_IsKeyDown(KK_UP))		m_Pitch += speed;
	if (Keyboard_IsKeyDown(KK_DOWN))	m_Pitch -= speed;

	// 真上・真下まで行くと LookAt が壊れるので制限する
	if (m_Pitch > 80.0f) m_Pitch = 80.0f;
	if (m_Pitch < -10.0f) m_Pitch = -10.0f;

	float yaw = XMConvertToRadians(m_Yaw);
	float pitch = XMConvertToRadians(m_Pitch);

	// 角度と距離からオフセットを作る（Yaw=0 のとき注視点の真後ろ＝-Z側）
	m_Offset.x = -sinf(yaw) * cosf(pitch) * m_Distance;
	m_Offset.y = sinf(pitch) * m_Distance;
	m_Offset.z = -cosf(yaw) * cosf(pitch) * m_Distance;

	m_Position.x = m_AtPosition.x + m_Offset.x;
	m_Position.y = m_AtPosition.y + m_Offset.y;
	m_Position.z = m_AtPosition.z + m_Offset.z;
}

void	Camera::Draw()//3D使用時
{
	//プロジェクション行列を作成
	XMMATRIX	ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(m_Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			m_Nearclip,
			m_Farclip
		);
	//プロジェクション行列をセット
	Renderer::SetProjectionMatrix(ProjectionMatrix);

	//カメラ行列を作成
	XMVECTOR	eyev = XMLoadFloat3(&m_AtPosition);
	XMVECTOR	pos = XMLoadFloat3(&m_Position);
	XMVECTOR	up = XMLoadFloat3(&m_UpVector);
	XMMATRIX	ViewMatrix =
		XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	Renderer::SetViewMatrix(ViewMatrix);
	//カメラ座標セット
	Renderer::SetCameraPosition(m_Position);
}

void Camera::SetCameraTarget(XMFLOAT3 target)
{
	m_AtPosition = target;
}




