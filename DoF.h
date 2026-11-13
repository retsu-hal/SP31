#pragma once
#include "Sprite2D.h"


class DoF : public Sprite2D
{
protected:
    Material m_MaterialDebug; // 深度確認用（UnlitTexture）

    const char* GetTypeName() const override { return "DoF"; }
    void DrawImGuiExtra() override;

public:
    DoF() : Sprite2D("DoF") {}

    void Init(void) override;
    void Update(void) override;
    void Draw(void) override {} 
    void DrawPass();           
    void DrawDepthDebug();      
};