/*==============================================================================

   頂点管理[Sprite2D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once


#include "main.h"
#include "GameObject.h"

class Sprite2D : public GameObject
{
	virtual const char* GetVertexShaderPath() const { return "UnlitTextureVS.cso"; }
	virtual const char* GetPixelShaderPath()  const { return "UnlitTexturePS.cso"; }
	virtual const wchar_t* GetTexturePath()  const { return L"asset\\texture\\img_yuno-sengoku.jpg"; }

	protected:
		XMFLOAT2	Size;			//サイズ

		float		Rotate;			//回転角度（360表現）
	public:
		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;


};