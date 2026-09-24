#pragma once

#include "renderer.h"

extern LIGHT g_Light;	// 共通ライト（Game.cpp）

void InitGame();
void FinalizeGame();
void UpdateGame();
void DrawGame();

void	SetPause(bool);
bool	GetPause();