/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
#include "World.hpp"

namespace BinF::Game {
    // Do NOT set the values manually!
    class CameraClass {
    public:
        CameraClass();
        CameraClass(WorldAxis, WorldAxis, PixelOffset, PixelOffset);
        // Change the camera's position by using pixel-units
        // Prefer the use of this function o'er SetPosition()
        void Move(PixelOffset, PixelOffset);
        // instantly set the position 
        void SetPosition(WorldAxis, WorldAxis, PixelOffset, PixelOffset);
        
        WorldAxis x, y;
        PixelOffset cx, cy;
    };

    // handled by Game Resources
    // Camera's position is world/pixel based
    extern CameraClass Camera;
}