
#pragma once

//Camera.h
#include "main.h"
#include "renderer.h"


class Camera
{
	public:
		XMFLOAT3	Position;		//カメラの座標
		XMFLOAT3	AtPosition;		//カメラの注視点
		XMFLOAT3	UpVector;		//上方ベクトル
		float		Fov;			//視野角
		float		Nearclip;		//どこまで近くが見えるか
		float		Farclip;		//どこまで遠くが見えるか
		float		Rotation;

		XMFLOAT3 Offset;

		float Yaw;		// 横回転（度）
		float Pitch;	// 縦回転（度）
		float Distance;	// 注視点からの距離
};

void	InitCamera();
void	FinalizeCamera();
void	UpdateCamera();
void	DrawCamera();
void SetCameraTarget(XMFLOAT3 target);

