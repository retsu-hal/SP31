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
	virtual const wchar_t* GetTexturePath()  const { return L"asset\\texture\\Toon2.png"; }
	const char* GetName() const override { return "Sprite2D"; }

	protected:
		XMFLOAT2	m_Size;			//サイズ
		XMFLOAT2	m_RampSize;		//★ランプ表示用サイズ
		XMFLOAT3	m_RampPosition;	//★ランプ表示用位置

		float		m_Rotate;			//回転角度（360表現）
		/* m_Parameter は GameObject から継承 */
	public:
		void Init(void)override;
		void Uninit(void)override;
		void Update(void)override;
		void Draw(void)override;


};