
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

};

void	InitCamera();
void	FinalizeCamera();
void	UpdateCamera();
void	DrawCamera();

