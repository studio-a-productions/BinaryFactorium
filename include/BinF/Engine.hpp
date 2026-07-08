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
    void ClearFrame(const colour col = black); // clear frame buffer (optional)
    void DrawSprite(const SpritePos&, const SpriteData&);
    void DrawSprite(screen_pos x, screen_pos y, const SpriteData&); // manual sprite pos
    void DrawSprite(screen_pos x, screen_pos y, const colourID* sprite, screen_pos s); // s is the size of both axis
    void DrawSprite(screen_pos x, screen_pos y, const colourID* sprite,
        screen_pos sprite_sx, screen_pos sprite_sy); // basically manual sprite data
    void DrawSpriteStride(screen_pos x, screen_pos y, const colourID* sprite, 
        screen_pos sprite_sx, screen_pos sprite_sy, screen_pos stride); // unsafe function, beware: spooky mem bugs!!!
    void PushFrame();

    // INPUT SYSTEM
    bool ButtonPressed(keycode);  // no "released" variant, just use a simple "!"
    bool ButtonDown(keycode);     // pressed this frame?
    bool ButtonUp(keycode);       // released this frame?
    s16  JoystickX();             // get the X-axis (SDL-style)
    s16  JoystickY();             // get the Y-axis (SDL-style)

    // TIME SYSTEM
    Time DeltaTime();             // time between two updates
    Time ReqTime();               // time since last ReqTime() or since Update()

}