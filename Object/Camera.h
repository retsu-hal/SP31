
#pragma once

//Camera.h
#include "main.h"
#include "renderer.h"


class Camera
{
	public:
		XMFLOAT3	m_Position;		//カメラの座標
		XMFLOAT3	m_AtPosition;		//カメラの注視点
		XMFLOAT3	m_UpVector;		//上方ベクトル
		float		m_Fov;			//視野角
		float		m_Nearclip;		//どこまで近くが見えるか
		float		m_Farclip;		//どこまで遠くが見えるか
		float		m_Rotation;

		XMFLOAT3 m_Offset;

		float m_Yaw;		// 横回転（度）
		float m_Pitch;	// 縦回転（度）
		float m_Distance;	// 注視点からの距離

		void	Init();
		void	Finalize();
		void	Update();
		void	Draw();
		void SetCameraTarget(XMFLOAT3 target);
};


