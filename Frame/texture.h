/*==============================================================================

   テクスチャ管理 [texture.h]
	
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include <string>
#include <d3d11.h>

class Texture
{
public:
	static void Initialize(ID3D11Device* device);
	static void Finalize(void);
	static int Load(const std::wstring& texture_filename);
	static ID3D11ShaderResourceView* GetTexture(int id);
	static int GetWidth(int id);
	static int GetHeight(int id);
};

#define SAFE_RELEASE(o)      if (o){  (o)->Release(); o = NULL; }