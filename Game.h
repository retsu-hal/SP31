#pragma once
#include "renderer.h"

extern LIGHT g_Light;

void InitGame();
void FinalizeGame();
void UpdateGame();
void DrawGame();

void	SetPause(bool);
bool	GetPause();