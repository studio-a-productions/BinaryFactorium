#pragma once

#include "Engine/Colour.hpp"
#include "Engine/Sprite.hpp"

namespace BinF::Engine {
    void Init();

    // RENDER SYSTEM
    void ClearScreen();
    void DrawSprite(const screen_pos x, const screen_pos y, const SpriteData&);
    void DrawSprite(const SpritePos&, const SpriteData&);
    void DisplayFrame();

    // INPUT SYSTEM
    void Update();
    bool ButtonPressed(u8 id);
    u16  JoystickX();
    u16  JoystickY();

}