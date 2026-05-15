#pragma once

#include "main.h"
#include "renderer.h"
#include"model.h"
#include "texture.h"
#include "Vector3.h"

class GameObject
{
protected://外部からアクセスできないが、継承したクラスからアクセスできる
	XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	
	int TexID;

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* VertexLayout;
	ID3D11SamplerState* SamplerState;


	MODEL* Model;


public:
	bool m_Is2D = true;		//true:2Dオブジェクト、false:3Dオブジェクト

public:
	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update() {};
	virtual void Draw() {};
};