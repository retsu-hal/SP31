#pragma once


enum SCENE
{
	SCENE_NONE,
	SCENE_TITLE,
	SCENE_GAME,
	SCENE_RESULT,
};

class Manager
{
public:
	static void Init();
	static void Finalize();
	static void Update();
	static void Draw();

	static void SetScene(SCENE Scene);
};