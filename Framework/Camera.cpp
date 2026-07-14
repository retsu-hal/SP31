
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"

void Camera::Init()
{
	//カメラの初期化
	m_Is2D = false;
	m_Position = XMFLOAT3(0.0f, 1.0f, -1.5f);	//カメラ基本座標
	m_Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);		//カメラ基本回転
	m_UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);		//カメラの上方ベクトル
	m_AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);	//カメラの注視点座標
	m_Fov = 45.0f;								//画角
	m_Nearclip = 0.5f;							//近面クリップ
	m_Farclip = 1000.0f;							//遠面クリップ
	m_radius = -2.0f;
}

void Camera::Uninit()
{
}

void Camera::Update()
{
	float speed = 0.8f;
	if (Keyboard_IsKeyDown(KK_LEFT))
	{
		m_Rotation.x += speed;
	}
	else if (Keyboard_IsKeyDown(KK_RIGHT))
	{
		m_Rotation.x += -speed;
	}



	float co = cosf(XMConvertToRadians(m_Rotation.x));
	float si = sinf(XMConvertToRadians(m_Rotation.x));

	m_Position.x = m_radius*si;
	m_Position.z = m_radius*co;

}

void Camera::Draw()
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
	SetProjectionMatrix(ProjectionMatrix);

	//カメラ行列を作成
	XMVECTOR	eyev = XMLoadFloat3(&m_AtPosition);
	XMVECTOR	pos = XMLoadFloat3(&m_Position);
	XMVECTOR	up = XMLoadFloat3(&m_UpVector);
	XMMATRIX	ViewMatrix =
		XMMatrixLookAtLH(pos, eyev, up);

	//カメラ行列をセット
	SetViewMatrix(ViewMatrix);
	//カメラ座標セット
	SetCameraPosition(m_Position);
}
