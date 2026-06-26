#pragma once

#include "Engine/Colour.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Input.hpp"

namespace BinF::Engine {
    // RESOURCE MANAGER
    void Init(const colour palette[256] /* indx 0 reserved for empty space */);
    void Update();
    void Exit();

    // RENDER SYSTEM
    void ClearScreen();
    void DrawSprite(const screen_pos x, const screen_pos y, const SpriteData&);
    void DrawSprite(const SpritePos&, const SpriteData&);
    void DisplayFrame();

    // INPUT SYSTEM
    bool ButtonPressed(keycode);
    u16  JoystickX();
    u16  JoystickY();

}