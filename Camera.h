
#pragma once

//Camera.h
#include "main.h"
#include "renderer.h"
#include "GameObject.h"


class Camera : public GameObject
{
	public:
		XMFLOAT3	m_AtPosition;		//カメラの注視点
		XMFLOAT3 	m_UpVector;		//上方ベクトル
		float		m_Fov;			//視野角
		float		m_Nearclip;		//どこまで近くが見えるか
		float		m_Farclip;		//どこまで遠くが見えるか
		float		m_radius;

	public:
		void Init() override;
		void Uninit() override ;
		void Update() override;
		void Draw() override;
};
