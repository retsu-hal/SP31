#pragma once

#include "renderer.h"

extern LIGHT g_Light;	// 共通ライト（Game.cpp）
extern LIGHT g_ShadowLight;	// 投影シャドウ用ライト（ライトカメラ行列つき）

void InitGame();
void FinalizeGame();
void UpdateGame();
void DrawGame();

void	SetPause(bool);
bool	GetPause();