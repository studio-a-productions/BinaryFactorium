/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "Engine/Colour.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Input.hpp"
#include "Engine/Time.hpp"
#include "Engine/Math.hpp"
#include "Engine/Renderer.hpp"

namespace BinF::Engine {
    // RESOURCE MANAGER
    void Init();    // init engine systems
    void Update();  // update engine systems (normally once per loop())
    void Exit();    // free engine alloc resources (reserved for potential desktop)

    // RENDER SYSTEM
    void ClearScreen();
    void DrawSprite(const screen_pos x, const screen_pos y, const SpriteData&);
    void DrawSprite(SpritePos*, const SpriteData*);
    void DrawSprite(const screen_pos x, const screen_pos y, const colourID* sprite,
        const screen_pos sprite_sx, const screen_pos sprite_sy);
    void DrawImage(const screen_pos x, const screen_pos y, const colour*image, 
        const screen_pos image_sx, const screen_pos image_sy);
    void PushFrame();

    // INPUT SYSTEM
    bool ButtonPressed(const keycode);  // no "released" variant, just use a simple "!"
    bool ButtonDown(const keycode);     // pressed this frame?
    bool ButtonUp(const keycode);       // released this frame?
    s16  JoystickX();                   // get the X-axis (SDL-style)
    s16  JoystickY();                   // get the Y-axis (SDL-style)

    // TIME SYSTEM
    Time DeltaTime();                   // time between two updates
    Time ReqTime();                     // time since last ReqTime() or since Update()

}