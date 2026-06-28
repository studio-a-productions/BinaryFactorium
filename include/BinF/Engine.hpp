#pragma once

#include "Engine/Colour.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Input.hpp"
#include "Engine/Time.hpp"
#include "Engine/Math.hpp"
#include "Engine/Renderer.hpp"

namespace BinF::Engine {
    // RESOURCE MANAGER
    void Init(const colour* palette /* indx 0 reserved for empty space */);
    void Update();
    void Exit();

    // RENDER SYSTEM
    void ClearScreen();
    void DrawSprite(const screen_pos x, const screen_pos y, const SpriteData&);
    void DrawSprite(SpritePos*, const SpriteData&);
    void PushFrame();

    // INPUT SYSTEM
    bool ButtonPressed(const keycode); // no "released" variant, is just simple "!"
    bool ButtonDown(const keycode); // pressed this frame?
    bool ButtonUp(const keycode); // released this frame?
    s16  JoystickX();
    s16  JoystickY();

    // TIME SYSTEM
    Time DeltaTime();
    Time ReqTime();

}